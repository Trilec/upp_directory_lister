// DirLister - directory scan and output formatting engine.
// Change log:
// - 2026-04-22: Added documentation comments and hardened CSV/settings handling.

#include "DirectoryEngine.h"

namespace Upp {

namespace {

// In-memory representation of one discovered file-system entry.
struct ScanEntry : Moveable<ScanEntry> {
    String full_path;
    String relative_path;
    String name;
    String extension;
    bool   is_dir = false;
    bool   hidden = false;
    int64  size = 0;
    Time   modified;
    int    depth = 0;
};

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

bool TypePatternMatch(bool is_dir,
                      const Vector<String>& file_patterns,
                      const Vector<String>& dir_patterns,
                      const String& text,
                      bool file_case_sensitive,
                      bool dir_case_sensitive,
                      PatternMode file_mode,
                      PatternMode dir_mode)
{
    if(is_dir)
        return dir_patterns.IsEmpty() || MatchesConfiguredPattern(dir_patterns, text, dir_case_sensitive, dir_mode);
    return file_patterns.IsEmpty() || MatchesConfiguredPattern(file_patterns, text, file_case_sensitive, file_mode);
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
    settings.min_size = max(settings.min_size, 0.0);
    settings.max_size = max(settings.max_size, 0.0);

    if(settings.max_size > 0 && settings.min_size > settings.max_size)
        Swap(settings.min_size, settings.max_size);
    if(!IsNull(settings.modified_from) && !IsNull(settings.modified_to) && settings.modified_to < settings.modified_from)
        Swap(settings.modified_from, settings.modified_to);

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

bool PassDateFilter(const ScanEntry& entry, const DirectoryScanSettings& settings)
{
    if(!settings.enable_date_filter || IsNull(entry.modified))
        return true;

    Date d(entry.modified.year, entry.modified.month, entry.modified.day);
    if(!IsNull(settings.modified_from) && d < settings.modified_from)
        return false;
    if(!IsNull(settings.modified_to) && d > settings.modified_to)
        return false;
    return true;
}

bool PassSizeFilter(const ScanEntry& entry, const DirectoryScanSettings& settings)
{
    if(entry.is_dir || !settings.enable_size_filter)
        return true;

    double scale = UnitMultiplier(settings.size_unit);
    if(settings.min_size > 0 && (double)entry.size < settings.min_size * scale)
        return false;
    if(settings.max_size > 0 && (double)entry.size > settings.max_size * scale)
        return false;
    return true;
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
                    const DirectoryScanSettings& settings,
                    const Vector<String>& file_patterns,
                    const Vector<String>& dir_patterns)
{
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

        bool pattern_match = TypePatternMatch(entry.is_dir,
                                              file_patterns,
                                              dir_patterns,
                                              entry.name,
                                              settings.file_case_sensitive,
                                              settings.dir_case_sensitive,
                                              settings.file_pattern_mode,
                                              settings.dir_pattern_mode);

        if((!entry.hidden || settings.show_hidden) && pattern_match && PassDateFilter(entry, settings) && PassSizeFilter(entry, settings)) {
            if(entry.is_dir && settings.include_directories)
                out.Add(entry);
            if(!entry.is_dir && settings.include_files)
                out.Add(entry);
        }

        if(entry.is_dir && settings.recursive && depth < settings.recursive_depth)
            CollectEntries(out, root, entry.full_path, depth + 1, settings, file_patterns, dir_patterns);

        ff.Next();
    }
}

Vector<DirectoryOutputLine> BuildTextLines(const Vector<ScanEntry>& entries, const DirectoryScanSettings& settings)
{
    Vector<DirectoryOutputLine> out;
    for(const ScanEntry& entry : entries) {
        String line;
        line << String(' ', max(0, entry.depth) * 2);

        String path_or_name;
        if(settings.show_path)
            path_or_name = entry.relative_path.IsEmpty() ? entry.name : entry.relative_path;
        else {
            path_or_name = settings.show_extension || entry.is_dir ? entry.name : TrimExtension(entry.name);
            if(entry.is_dir)
                path_or_name << "/";
        }
        line << NormalizeSlashes(path_or_name, settings.slash_mode);

        if(settings.show_extension && !entry.is_dir && !entry.extension.IsEmpty())
            line << "  [" << entry.extension << "]";
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

String BuildTextOutput(const Vector<ScanEntry>& entries, const DirectoryScanSettings& settings)
{
    String out;
    Vector<DirectoryOutputLine> lines = BuildTextLines(entries, settings);
    for(const DirectoryOutputLine& line : lines)
        out << line.text << "\n";
    return out;
}

String BuildCsvOutput(const Vector<ScanEntry>& entries, const DirectoryScanSettings& settings)
{
    String out = "type,path,name,extension,size,modified\n";
    for(const ScanEntry& entry : entries) {
        String display_path = NormalizeSlashes(entry.relative_path.IsEmpty() ? entry.name : entry.relative_path, settings.slash_mode);
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
        String display_path = NormalizeSlashes(entry.relative_path.IsEmpty() ? entry.name : entry.relative_path, settings.slash_mode);
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

String DirectoryEngine::Generate(const DirectoryScanSettings& settings)
{
    DirectoryScanSettings normalized = NormalizeSettings(settings);
    String source = TrimBoth(normalized.source_directory);
    if(source.IsEmpty())
        return "Source directory is empty.\n";
    if(!DirectoryExists(source))
        return Format("Source directory does not exist: %s\n", source);

    Vector<ScanEntry> entries;
    CollectEntries(entries,
                   source,
                   source,
                   0,
                   normalized,
                   SplitPatterns(normalized.file_patterns, normalized.file_case_sensitive),
                   SplitPatterns(normalized.directory_patterns, normalized.dir_case_sensitive));

    Sort(entries, [&](const ScanEntry& a, const ScanEntry& b) { return CompareEntries(a, b, normalized) < 0; });

    switch(normalized.output_format) {
    case OutputFormat::Csv:
        return BuildCsvOutput(entries, normalized);
    case OutputFormat::Json:
        return BuildJsonOutput(entries, normalized);
    case OutputFormat::Text:
    default:
        return BuildTextOutput(entries, normalized);
    }
}

Vector<DirectoryOutputLine> DirectoryEngine::GenerateTextLines(const DirectoryScanSettings& settings)
{
    DirectoryScanSettings normalized = NormalizeSettings(settings);
    String source = TrimBoth(normalized.source_directory);
    if(source.IsEmpty()) {
        Vector<DirectoryOutputLine> out;
        DirectoryOutputLine& line = out.Add();
        line.text = "Source directory is empty.";
        return out;
    }
    if(!DirectoryExists(source)) {
        Vector<DirectoryOutputLine> out;
        DirectoryOutputLine& line = out.Add();
        line.text = Format("Source directory does not exist: %s", source);
        return out;
    }

    Vector<ScanEntry> entries;
    CollectEntries(entries,
                   source,
                   source,
                   0,
                   normalized,
                   SplitPatterns(normalized.file_patterns, normalized.file_case_sensitive),
                   SplitPatterns(normalized.directory_patterns, normalized.dir_case_sensitive));

    Sort(entries, [&](const ScanEntry& a, const ScanEntry& b) { return CompareEntries(a, b, normalized) < 0; });
    return BuildTextLines(entries, normalized);
}

}
