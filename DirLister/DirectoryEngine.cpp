// DirLister - directory scan and output formatting engine.
// Change log:
// - 2026-04-22: Added documentation comments and hardened CSV/settings handling.

#include "DirectoryEngine.h"

namespace Upp {

namespace {

using ScanEntry = DirectoryEntry;

Vector<String> SplitPatterns(const String& text, bool case_sensitive)
{
    Vector<String> out;
    Vector<String> parts = Split(text, ';');
    for(const String& raw : parts) {
        String part = TrimBoth(raw);
        if(!part.IsEmpty())
            out.Add(case_sensitive ? part : ToLower(part));
    }
    return out;
}

bool MatchWildcardI(const char* pattern, const char* text, bool case_sensitive)
{
    while(*pattern) {
        if(*pattern == '*') {
            pattern++;
            if(!*pattern)
                return true;
            while(*text) {
                if(MatchWildcardI(pattern, text, case_sensitive))
                    return true;
                text++;
            }
            return false;
        }
        int pc = (byte)*pattern;
        int tc = (byte)*text;
        if(!case_sensitive) {
            pc = ToLower(pc);
            tc = ToLower(tc);
        }
        if(*pattern != '?' && pc != tc)
            return false;
        if(!*text)
            return false;
        pattern++;
        text++;
    }
    return *text == 0;
}

bool MatchesAnyPattern(const Vector<String>& patterns, const String& text, bool case_sensitive)
{
    if(patterns.IsEmpty())
        return true;
    String lower = case_sensitive ? text : ToLower(text);
    for(const String& pattern : patterns)
        if(MatchWildcardI(pattern, lower, case_sensitive))
            return true;
    return false;
}

bool MatchesAnyContains(const Vector<String>& patterns, const String& text, bool case_sensitive)
{
    if(patterns.IsEmpty())
        return true;
    String candidate = case_sensitive ? text : ToLower(text);
    for(const String& pattern : patterns)
        if(candidate.Find(pattern) >= 0)
            return true;
    return false;
}

bool MatchesConfiguredPattern(const Vector<String>& patterns,
                             const String& text,
                             bool case_sensitive,
                             PatternMode mode)
{
    if(mode == PatternMode::Contains)
        return MatchesAnyContains(patterns, text, case_sensitive);
    return MatchesAnyPattern(patterns, text, case_sensitive);
}

String NormalizeSlashes(String path, SlashMode mode)
{
    if(mode == SlashMode::Unix) {
        path.Replace("\\", "/");
        return path;
    }
#ifdef PLATFORM_WIN32
    path.Replace("/", "\\");
    return path;
#else
    path.Replace("\\", "/");
    return path;
#endif
}

String TrimExtension(String name)
{
    int pos = name.ReverseFind('.');
    if(pos <= 0)
        return name;
    return name.Left(pos);
}

String DateString(const Time& tm)
{
    if(IsNull(tm))
        return String();
    return Format("%04d-%02d-%02d", tm.year, tm.month, tm.day);
}

double UnitMultiplier(SizeUnit unit)
{
    switch(unit) {
    case SizeUnit::Bytes: return 1.0;
    case SizeUnit::Kilobytes: return 1024.0;
    case SizeUnit::Megabytes: return 1024.0 * 1024.0;
    case SizeUnit::Gigabytes: return 1024.0 * 1024.0 * 1024.0;
    }
    return 1.0;
}

DirectoryScanSettings NormalizeSettings(DirectoryScanSettings settings)
{
    settings.recursive_depth = max(settings.recursive_depth, 0);
    for(ScanFilterRule& rule : settings.filter_rules) {
        rule.min_size = max(0.0, rule.min_size);
        rule.max_size = max(0.0, rule.max_size);
        if(rule.max_size > 0 && rule.min_size > rule.max_size) Swap(rule.min_size, rule.max_size);
        if(!IsNull(rule.modified_from) && !IsNull(rule.modified_to) && rule.modified_to < rule.modified_from)
            Swap(rule.modified_from, rule.modified_to);
    }

    return settings;
}

int CompareEntries(const ScanEntry& a, const ScanEntry& b, const DirectoryScanSettings& settings)
{
    auto compare_key = [&](DirSortKey key) {
        switch(key) {
        case DirSortKey::Size:
            return SgnCompare(a.size, b.size);
        case DirSortKey::Modified:
            return SgnCompare(a.modified, b.modified);
        case DirSortKey::Type:
            return Upp::CompareNoCase(a.extension, b.extension);
        case DirSortKey::Unsorted:
            return 0;
        case DirSortKey::Name:
        default:
            return Upp::CompareNoCase(a.name, b.name);
        }
    };

    if(settings.dir_placement == DirPlacement::DirsFirst && a.is_dir != b.is_dir)
        return a.is_dir ? -1 : 1;
    if(settings.dir_placement == DirPlacement::DirsLast && a.is_dir != b.is_dir)
        return a.is_dir ? 1 : -1;

    int cmp = compare_key(settings.sort_key);
    if(cmp == 0)
        cmp = compare_key(settings.secondary_sort_key);

    if(cmp == 0)
        cmp = Upp::CompareNoCase(a.name, b.name);
    if(cmp == 0)
        cmp = Upp::CompareNoCase(a.relative_path, b.relative_path);
    return settings.reverse_sort ? -cmp : cmp;
}

String JsonEscape(const String& text)
{
    String out;
    for(int i = 0; i < text.GetCount(); i++) {
        int c = text[i];
        switch(c) {
        case '\\': out << "\\\\"; break;
        case '"': out << "\\\""; break;
        case '\n': out << "\\n"; break;
        case '\r': out << "\\r"; break;
        case '\t': out << "\\t"; break;
        default: out.Cat(c); break;
        }
    }
    return out;
}

String CsvEscape(const String& text)
{
    String escaped;
    escaped.Reserve(text.GetCount() + 8);
    escaped << '"';
    for(int i = 0; i < text.GetCount(); i++) {
        int c = text[i];
        if(c == '"')
            escaped << '"';
        escaped.Cat(c);
    }
    escaped << '"';
    return escaped;
}

void CollectEntries(Vector<ScanEntry>& out,
                    const String& root,
                    const String& current,
                    int depth,
                    const DirectoryScanSettings& settings)
{
    Vector<ScanEntry> siblings;
    FindFile ff(AppendFileName(current, "*"));
    while(ff) {
        String name = ff.GetName();
        if(name == "." || name == "..") {
            ff.Next();
            continue;
        }

        ScanEntry entry;
        entry.full_path = AppendFileName(current, name);
        entry.relative_path = entry.full_path.Mid(root.GetCount());
        if(entry.relative_path.StartsWith("\\") || entry.relative_path.StartsWith("/"))
            entry.relative_path = entry.relative_path.Mid(1);
        entry.name = name;
        entry.extension = ToLower(GetFileExt(name));
        entry.is_dir = ff.IsFolder();
        entry.hidden = ff.IsHidden();
        entry.size = entry.is_dir ? 0 : ff.GetLength();
        entry.modified = ff.GetLastWriteTime();
        entry.depth = depth;

        siblings.Add(entry);
        ff.Next();
    }
    if(settings.enable_sorting)
        Sort(siblings, [&](const ScanEntry& a, const ScanEntry& b) { return CompareEntries(a, b, settings) < 0; });
    Vector<int> matched;
    matched.SetCount(settings.filter_rules.GetCount(), 0);
    for(const ScanEntry& entry : siblings) {
        if(entry.hidden && !settings.show_hidden)
            continue;
        bool selected = true;
        bool descend = true;
        if(settings.enable_filtering) {
            for(int i = 0; i < settings.filter_rules.GetCount(); i++) {
                const ScanFilterRule& rule = settings.filter_rules[i];
                if((rule.target == FilterTarget::Files && entry.is_dir) ||
                   (rule.target == FilterTarget::Directories && !entry.is_dir) ||
                   (rule.level > 0 && rule.level != depth + 1))
                    continue;
                bool match;
                if(rule.kind == FilterKind::SizeRange) {
                    // Folder sizes are not measured; never treat them as zero-byte files.
                    if(entry.is_dir) continue;
                    double bytes = (double)entry.size, scale = UnitMultiplier(rule.size_unit);
                    match = (rule.min_size <= 0 || bytes >= rule.min_size * scale) &&
                            (rule.max_size <= 0 || bytes <= rule.max_size * scale);
                }
                else if(rule.kind == FilterKind::DateRange) {
                    if(IsNull(entry.modified)) continue;
                    Date date(entry.modified.year, entry.modified.month, entry.modified.day);
                    match = (IsNull(rule.modified_from) || date >= rule.modified_from) &&
                            (IsNull(rule.modified_to) || date <= rule.modified_to);
                }
                else
                    match = MatchesConfiguredPattern(SplitPatterns(rule.patterns, rule.case_sensitive),
                                                     entry.name, rule.case_sensitive, rule.mode);
                if(rule.action == FilterAction::Include) {
                    selected = selected && match;
                }
                else if(rule.action == FilterAction::Exclude && match) {
                    selected = false;
                    // Metadata on a parent does not describe its descendants.
                    descend = rule.kind != FilterKind::Name;
                    break;
                }
                else if(rule.action == FilterAction::FirstMatches && match && selected) {
                    if(matched[i]++ >= max(0, rule.limit)) {
                        selected = false;
                        descend = false;
                        break;
                    }
                }
            }
        }
        if(selected &&
           (entry.is_dir ? settings.include_directories : settings.include_files))
            out.Add(entry);
        if(entry.is_dir && descend && settings.recursive && depth < settings.recursive_depth)
            CollectEntries(out, root, entry.full_path, depth + 1, settings);
    }
}

Vector<DirectoryOutputLine> BuildTextLines(const Vector<ScanEntry>& entries, const DirectoryScanSettings& settings)
{
    Vector<DirectoryOutputLine> out;
    for(const ScanEntry& entry : entries) {
        String line;

        String path_or_name;
        if(settings.show_path) {
            path_or_name = entry.full_path;
            if(!settings.show_extension && !entry.is_dir)
                path_or_name = TrimExtension(path_or_name);
        }
        else {
            path_or_name = settings.show_extension || entry.is_dir ? entry.relative_path : TrimExtension(entry.relative_path);
            if(entry.is_dir)
                path_or_name << "/";
        }
        line << NormalizeSlashes(path_or_name, settings.slash_mode);

        if(settings.show_size && !entry.is_dir)
            line << "  [" << Format64(entry.size) << " B]";
        if(settings.show_date && !IsNull(entry.modified))
            line << "  [" << DateString(entry.modified) << "]";

        DirectoryOutputLine& l = out.Add();
        l.text = line;
        l.is_dir = entry.is_dir;
    }
    if(out.IsEmpty())
        out.Add().text = "No matching entries.";
    return out;
}

// Build a trie so missing filtered ancestors remain explicit context, never
// disconnected indentation. Children retain the scanner's sibling ordering.
struct TreeNode {
    String name;
    int entry = -1;
    Vector<int> children;
};

Vector<DirectoryOutputLine> BuildTreeLines(const Vector<ScanEntry>& entries, const DirectoryScanSettings& settings)
{
    Vector<DirectoryOutputLine> out;
    out.Add().text = NormalizeSlashes(settings.source_directory, settings.slash_mode);
    Array<TreeNode> nodes;
    nodes.Add();
    for(int i = 0; i < entries.GetCount(); i++) {
        String path = entries[i].relative_path;
        path.Replace("\\", "/");
        Vector<String> parts = Split(path, '/');
        int parent = 0;
        for(const String& part : parts) {
            int child = -1;
            for(int n : nodes[parent].children)
                if(nodes[n].name == part) { child = n; break; }
            if(child < 0) {
                child = nodes.GetCount();
                nodes.Add().name = part;
                nodes[parent].children.Add(child);
            }
            parent = child;
        }
        nodes[parent].entry = i;
    }
    auto emit = [&](auto&& self, int parent, const String& prefix) -> void {
        for(int i = 0; i < nodes[parent].children.GetCount(); i++) {
            int n = nodes[parent].children[i];
            const TreeNode& node = nodes[n];
            bool last = i + 1 == nodes[parent].children.GetCount();
            const ScanEntry* entry = node.entry >= 0 ? &entries[node.entry] : nullptr;
            DirectoryOutputLine& line = out.Add();
            line.is_dir = !entry || entry->is_dir;
            line.text = prefix + (last ? "`-- " : "|-- ");
            line.text << (entry && !entry->is_dir && !settings.show_extension ? TrimExtension(node.name) : node.name);
            if(line.is_dir) line.text << "/";
            if(!entry) line.text << " [context]";
            if(entry && settings.show_path) line.text << "  [" << NormalizeSlashes(entry->full_path, settings.slash_mode) << "]";
            if(entry && settings.show_size && !entry->is_dir) line.text << "  [" << Format64(entry->size) << " B]";
            if(entry && settings.show_date) line.text << "  [" << DateString(entry->modified) << "]";
            self(self, n, prefix + (last ? "    " : "|   "));
        }
    };
    emit(emit, 0, String());
    if(entries.IsEmpty()) out.Add().text = "No matching entries.";
    return out;
}

String BuildTextOutput(const Vector<ScanEntry>& entries, const DirectoryScanSettings& settings)
{
    String out;
    Vector<DirectoryOutputLine> lines = settings.output_format == OutputFormat::Tree
        ? BuildTreeLines(entries, settings) : BuildTextLines(entries, settings);
    for(const DirectoryOutputLine& line : lines)
        out << line.text << "\n";
    return out;
}

String BuildCsvOutput(const Vector<ScanEntry>& entries, const DirectoryScanSettings& settings)
{
    String out = "type,path,name,extension,size,modified\n";
    for(const ScanEntry& entry : entries) {
        String display_path = settings.show_path ? entry.full_path : (entry.relative_path.IsEmpty() ? entry.name : entry.relative_path);
        if(!settings.show_extension && !entry.is_dir)
            display_path = TrimExtension(display_path);
        display_path = NormalizeSlashes(display_path, settings.slash_mode);
        out << (entry.is_dir ? "directory" : "file") << ','
            << CsvEscape(display_path) << ','
            << CsvEscape(settings.show_extension || entry.is_dir ? entry.name : TrimExtension(entry.name)) << ','
            << CsvEscape(entry.extension) << ','
            << entry.size << ','
            << CsvEscape(DateString(entry.modified)) << "\n";
    }
    return out;
}

String BuildJsonOutput(const Vector<ScanEntry>& entries, const DirectoryScanSettings& settings)
{
    String out = "[\n";
    for(int i = 0; i < entries.GetCount(); i++) {
        const ScanEntry& entry = entries[i];
        String display_path = settings.show_path ? entry.full_path : (entry.relative_path.IsEmpty() ? entry.name : entry.relative_path);
        if(!settings.show_extension && !entry.is_dir)
            display_path = TrimExtension(display_path);
        display_path = NormalizeSlashes(display_path, settings.slash_mode);
        out << "  {"
            << "\"type\":\"" << (entry.is_dir ? "directory" : "file") << "\"," 
            << "\"path\":\"" << JsonEscape(display_path) << "\"," 
            << "\"name\":\"" << JsonEscape(settings.show_extension || entry.is_dir ? entry.name : TrimExtension(entry.name)) << "\"," 
            << "\"extension\":\"" << JsonEscape(entry.extension) << "\"," 
            << "\"size\":" << entry.size << ','
            << "\"modified\":\"" << DateString(entry.modified) << "\"}"
            << (i + 1 < entries.GetCount() ? "," : "") << "\n";
    }
    out << "]\n";
    return out;
}

}

Vector<DirectoryEntry> DirectoryEngine::Scan(const DirectoryScanSettings& settings)
{
    DirectoryScanSettings normalized = NormalizeSettings(settings);
    String source = TrimBoth(normalized.source_directory);
    // Remove trailing separators except a filesystem root, for correct relative paths.
    while(source.GetCount() > 3 && (source.EndsWith("/") || source.EndsWith("\\")))
        source.Trim(source.GetCount() - 1);
    Vector<DirectoryEntry> entries;
    if(DirectoryExists(source))
        CollectEntries(entries, source, source, 0, normalized);
    return entries;
}

String DirectoryEngine::Generate(const DirectoryScanSettings& settings)
{
    String source = TrimBoth(settings.source_directory);
    if(source.IsEmpty()) return "Source directory is empty.\n";
    if(!DirectoryExists(source)) return Format("Source directory does not exist: %s\n", source);
    Vector<DirectoryEntry> entries = Scan(settings);
    switch(settings.output_format) {
    case OutputFormat::Csv: return BuildCsvOutput(entries, settings);
    case OutputFormat::Json: return BuildJsonOutput(entries, settings);
    default: return BuildTextOutput(entries, settings);
    }
}

Vector<DirectoryOutputLine> DirectoryEngine::GenerateTextLines(const DirectoryScanSettings& settings)
{
    String source = TrimBoth(settings.source_directory);
    if(source.IsEmpty() || !DirectoryExists(source)) {
        Vector<DirectoryOutputLine> out;
        out.Add().text = source.IsEmpty() ? String("Source directory is empty.")
            : Format("Source directory does not exist: %s", source);
        return out;
    }
    Vector<DirectoryEntry> entries = Scan(settings);
    return settings.output_format == OutputFormat::Tree ? BuildTreeLines(entries, settings) : BuildTextLines(entries, settings);
}

}
