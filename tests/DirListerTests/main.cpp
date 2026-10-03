#include <Core/Core.h>
#include "../../DirLister/DirectoryEngine.h"
using namespace Upp;
int checks = 0;
void Check(bool ok, const char* message) {
    if(!ok) { Cout() << "FAIL: " << message << '\n'; SetExitCode(1); Exit(); }
    checks++;
}
ScanFilterRule Rule(FilterAction action, const char* pattern, int limit = 3) {
    ScanFilterRule r;
    r.action = action; r.patterns = pattern; r.limit = limit;
    return r;
}
int CountNamed(const Vector<DirectoryEntry>& entries, const char* prefix) {
    int n = 0;
    for(const auto& e : entries) if(e.name.StartsWith(prefix)) n++;
    return n;
}
CONSOLE_APP_MAIN {
    String root = AppendFileName(GetTempPath(), "DirLister-tests/scan-fixture");
    RealizeDirectory(root);
    for(const char* parent : {"a", "b"}) {
        for(int n = 1; n <= 5; n++) {
            String path = AppendFileName(root, Format("%s/BB_%02d/child/deeper", parent, n));
            RealizeDirectory(path);
        }
        RealizeDirectory(AppendFileName(root, String(parent) + "/ordinary"));
        SaveFile(AppendFileName(root, String(parent) + "/sample.txt"), "text");
    }
    for(int n = 0; n < 350; n++) RealizeDirectory(AppendFileName(root, Format("bulk/F%03d", n)));
    DirectoryScanSettings s;
    s.source_directory = root; s.include_files = false; s.recursive_depth = 2;
    Vector<DirectoryEntry> all = DirectoryEngine::Scan(s);
    Check(all.GetCount() == 375, "all 375 directories survive scan");
    Index<String> paths;
    for(const auto& e : all) paths.FindAdd(e.relative_path);
    Check(paths.GetCount() == all.GetCount(), "unique full paths");
    s.enable_sorting = false; s.recursive = false;
    Vector<String> enumerated;
    FindFile ff(AppendFileName(root, "*"));
    while(ff) { if(ff.IsFolder() && ff.GetName() != "." && ff.GetName() != "..") enumerated.Add(ff.GetName()); ff.Next(); }
    auto unsorted = DirectoryEngine::Scan(s);
    Check(unsorted.GetCount() == enumerated.GetCount(), "unsorted count");
    for(int i = 0; i < unsorted.GetCount(); i++) Check(unsorted[i].name == enumerated[i], "disabled sorting preserves enumeration order");
    auto unsorted_lines = DirectoryEngine::GenerateTextLines(s);
    Check(unsorted_lines[0].text == enumerated[0] + String(DIR_SEP, 1), "text preview honors disabled sorting");
    s.enable_sorting = true; s.recursive = true;
    s.output_format = OutputFormat::Json;
    Check(ParseJSON(DirectoryEngine::Generate(s)).GetCount() == all.GetCount(), "JSON includes every scanned entry");
    s.output_format = OutputFormat::Text;
    s.enable_filtering = true;
    s.filter_rules.Add(Rule(FilterAction::FirstMatches, "BB_*", 3));
    auto limited = DirectoryEngine::Scan(s);
    Check(CountNamed(limited, "BB_") == 6, "three shot folders per parent");
    Check(CountNamed(limited, "child") == 6, "excess shot branches pruned");
    Check(CountNamed(limited, "ordinary") == 2, "nonmatching siblings retained");
    Check(CountNamed(limited, "F") == 350, "limit only affects matching names");
    s.reverse_sort = true;
    limited = DirectoryEngine::Scan(s);
    Check(limited[0].name == "bulk", "reverse sibling ordering");
    Check(limited.Top().relative_path.StartsWith("a"), "reverse traversal retains hierarchy");
    Check(DirectoryEngine::Generate(s).Find("BB_05") >= 0 && DirectoryEngine::Generate(s).Find("BB_01") < 0,
          "first N follows reverse ordering");
    s.reverse_sort = false;
    s.filter_rules.Add(Rule(FilterAction::Exclude, "BB_02;BB_03"));
    limited = DirectoryEngine::Scan(s);
    Check(CountNamed(limited, "BB_") == 2, "stack order: exclusions after limit");
    Swap(s.filter_rules[0], s.filter_rules[1]);
    limited = DirectoryEngine::Scan(s);
    Check(CountNamed(limited, "BB_") == 6, "exclusions before limit refill available slots");
    s.filter_rules.Clear();
    auto exclude = Rule(FilterAction::Exclude, "BB_"); exclude.mode = PatternMode::Contains;
    s.filter_rules.Add(exclude);
    limited = DirectoryEngine::Scan(s);
    Check(CountNamed(limited, "BB_") == 0 && CountNamed(limited, "child") == 0, "contains exclusion prunes subtrees");
    s.filter_rules.Clear();
    auto include = Rule(FilterAction::Include, "BB_*"); include.level = 2;
    s.filter_rules.Add(include);
    Check(CountNamed(DirectoryEngine::Scan(s), "ordinary") == 0, "level-scoped include");
    s.filter_rules.Clear();
    auto cap = Rule(FilterAction::FirstMatches, "*", 1); cap.level = 1;
    s.filter_rules.Add(cap);
    limited = DirectoryEngine::Scan(s);
    Check(limited[0].name == "a" && CountNamed(limited, "BB_") == 5, "level one limit leaves deeper levels unrestricted");
    s.enable_filtering = false;
    Check(DirectoryEngine::Scan(s).GetCount() == all.GetCount(), "filter master switch bypasses stack");
    s.filter_rules.Clear(); s.output_format = OutputFormat::Tree;
    String tree = DirectoryEngine::Generate(s);
    Check(tree.Find("|-- a/") >= 0 && tree.Find("|   |-- BB_01/") >= 0, "tree line art and sibling ancestry");
    Check(tree.Find("F349/") >= 0, "tree not truncated beyond 200");
    s.output_format = OutputFormat::Text;
    Check(DirectoryEngine::Generate(s).Find("a") >= 0 && DirectoryEngine::Generate(s).Find("BB_01") >= 0, "flat text includes relative paths");
    s.enable_filtering = true; s.filter_rules.Add(Rule(FilterAction::Include, "BB_*")); s.output_format = OutputFormat::Tree;
    tree = DirectoryEngine::Generate(s);
    Check(tree.Find("a/ [context]") >= 0, "filtered ancestors explicitly shown as context");
    s.filter_rules.Clear(); s.filter_rules.Add(Rule(FilterAction::Exclude, "*.txt"));
    s.filter_rules[0].target = FilterTarget::Files; s.include_files = true;
    Check(CountNamed(DirectoryEngine::Scan(s), "sample") == 0, "file stack exclusion");
    s.enable_filtering = false; s.recursive = false;
    Check(DirectoryEngine::Scan(s).GetCount() == 3, "nonrecursive scan only lists immediate children");
    s.recursive = true; s.recursive_depth = 0;
    Check(DirectoryEngine::Scan(s).GetCount() == 3, "depth zero lists immediate children");
    s.recursive_depth = 1;
    Check(DirectoryEngine::Scan(s).GetCount() == 367, "depth one adds one descendant level");
    s.recursive_depth = 2; s.source_directory << "/";
    Check(DirectoryEngine::Scan(s)[0].relative_path == "a", "trailing slash preserves relative paths");
    String metadata_root = AppendFileName(GetTempPath(), "DirLister-tests/metadata-fixture");
    RealizeDirectory(AppendFileName(metadata_root, "sub"));
    SaveFile(AppendFileName(metadata_root, "a1.txt"), "1");
    SaveFile(AppendFileName(metadata_root, "a4.txt"), "1234");
    SaveFile(AppendFileName(metadata_root, "b10.txt"), "0123456789");
    SaveFile(AppendFileName(metadata_root, "sub/child.txt"), "123456");
    Check(SetFileTime(AppendFileName(metadata_root, "a1.txt"), TimeToFileTime(Time(2020, 1, 1))), "fixture date 2020");
    Check(SetFileTime(AppendFileName(metadata_root, "a4.txt"), TimeToFileTime(Time(2024, 1, 1))), "fixture date 2024");
    Check(SetFileTime(AppendFileName(metadata_root, "b10.txt"), TimeToFileTime(Time(2026, 1, 1))), "fixture date 2026");
    FindFile folder_info(AppendFileName(metadata_root, "sub"));
    Time folder_time = folder_info.GetLastWriteTime();
    Date folder_date(folder_time.year, folder_time.month, folder_time.day);
    Check(SetFileTime(AppendFileName(metadata_root, "sub/child.txt"), TimeToFileTime(Time(2026, 1, 1))), "fixture child date");
    DirectoryScanSettings m;
    m.source_directory = metadata_root; m.enable_filtering = true; m.recursive = false;
    ScanFilterRule size;
    size.kind = FilterKind::SizeRange; size.action = FilterAction::Include;
    size.target = FilterTarget::Files; size.size_unit = SizeUnit::Bytes; size.min_size = 4; size.max_size = 10;
    m.filter_rules.Add(size);
    auto metadata = DirectoryEngine::Scan(m);
    Check(CountNamed(metadata, "a1") == 0 && CountNamed(metadata, "a4") == 1 && CountNamed(metadata, "b10") == 1,
          "inclusive size bounds in stack");
    Check(CountNamed(metadata, "sub") == 1, "file size step leaves directories alone");
    m.filter_rules[0].action = FilterAction::Exclude;
    Check(CountNamed(DirectoryEngine::Scan(m), "a1") == 1 && CountNamed(DirectoryEngine::Scan(m), "a4") == 0,
          "outside size range");
    m.filter_rules[0].action = FilterAction::Include;
    m.filter_rules[0].min_size = 10; m.filter_rules[0].max_size = 4;
    Check(DirectoryEngine::Scan(m).GetCount() == 3, "reversed size bounds normalized per step");
    m.filter_rules[0] = size;
    m.filter_rules[0].min_size = 4.0 / 1024; m.filter_rules[0].max_size = 10.0 / 1024;
    m.filter_rules[0].size_unit = SizeUnit::Kilobytes;
    Check(DirectoryEngine::Scan(m).GetCount() == 3, "fractional unit conversion");
    m.filter_rules[0] = size;
    auto first_file = Rule(FilterAction::FirstMatches, "*.txt", 1); first_file.target = FilterTarget::Files;
    m.filter_rules.Add(first_file);
    Check(CountNamed(DirectoryEngine::Scan(m), "a4") == 1, "size before first N selects first eligible file");
    Swap(m.filter_rules[0], m.filter_rules[1]);
    Check(DirectoryEngine::Scan(m).GetCount() == 1, "first N before size filters the chosen sample");
    ScanFilterRule date;
    date.kind = FilterKind::DateRange; date.action = FilterAction::Include; date.target = FilterTarget::Both;
    date.modified_from = Date(2024, 1, 1); date.modified_to = Date(2026, 1, 1);
    m.filter_rules.Clear(); m.filter_rules.Add(date);
    Check(DirectoryEngine::Scan(m).GetCount() == 2, "inclusive date range in stack");
    m.recursive = true;
    Check(CountNamed(DirectoryEngine::Scan(m), "child") == 1, "date-filtered parent still traversed");
    m.filter_rules[0].modified_from = Date(2026, 1, 1); m.filter_rules[0].modified_to = Date(2024, 1, 1);
    Check(DirectoryEngine::Scan(m).GetCount() == 3, "reversed date bounds normalized per step");
    m.filter_rules[0].action = FilterAction::Exclude;
    m.filter_rules[0].modified_from = folder_date; m.filter_rules[0].modified_to = folder_date;
    Check(CountNamed(DirectoryEngine::Scan(m), "sub") == 0 && CountNamed(DirectoryEngine::Scan(m), "child") == 1,
          "outside date range removes parent without pruning child dates");
    m.enable_filtering = false;
    Check(DirectoryEngine::Scan(m).GetCount() == 5, "master switch bypasses metadata stack too");

    String archive = "I:/archive/fbb/BB_job/prod/work";
    if(DirectoryExists(archive)) {
        s.source_directory = archive; s.include_files = false;
        auto actual = DirectoryEngine::Scan(s);
        Cout() << "Archive depth 2 directories: " << actual.GetCount() << '\n';
        Check(actual.GetCount() > 200, "archive output exceeds reported cutoff");
        Index<String> unique;
        for(const auto& e : actual) unique.FindAdd(e.full_path);
        Check(unique.GetCount() == actual.GetCount(), "archive scan contains no duplicated full paths");
        SaveFile(AppendFileName(GetTempPath(), "DirLister-tests/archive-tree.txt"), DirectoryEngine::Generate(s));
        s.enable_filtering = true; s.filter_rules.Clear(); s.filter_rules.Add(Rule(FilterAction::FirstMatches, "BB_*", 3));
        Cout() << "Archive with BB_* first-three rule: " << DirectoryEngine::Scan(s).GetCount() << '\n';
        SaveFile(AppendFileName(GetTempPath(), "DirLister-tests/archive-sample-tree.txt"), DirectoryEngine::Generate(s));
    }
    Cout() << "PASS: " << checks << " checks\n";
}
