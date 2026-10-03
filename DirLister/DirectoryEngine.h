// DirLister - directory scan and output formatting engine.
// Change log:
// - 2026-04-22: Added file documentation and clearer type comments.

#ifndef _DirLister_DirectoryEngine_h_
#define _DirLister_DirectoryEngine_h_

#include <Core/Core.h>

namespace Upp {

// Primary sort key used after directory enumeration completes.
enum class DirSortKey : byte {
    Name,
    Size,
    Modified,
    Type,
    Unsorted,
};

// Controls whether directories are mixed with files or grouped.
enum class DirPlacement : byte {
    Inline,
    DirsFirst,
    DirsLast,
};

// Normalizes output path separators for display/export.
enum class SlashMode : byte {
    Native,
    Unix,
};

// Selects the final renderer used for the listing output.
enum class OutputFormat : byte {
    Text,
    Csv,
    Json,
    Tree,
};

// Units used by the size filter controls.
enum class SizeUnit : byte {
    Bytes,
    Kilobytes,
    Megabytes,
    Gigabytes,
};

enum class PatternMode : byte {
    Glob,
    Contains,
};

// Stack rules apply in order. Include intersects the current selection; Exclude
// and FirstMatches also prune directory traversal. Limits reset for each parent.
enum class FilterAction : byte { Include, Exclude, FirstMatches };
enum class FilterTarget : byte { Files, Directories, Both };
enum class FilterKind : byte { Name, SizeRange, DateRange };
struct ScanFilterRule : Moveable<ScanFilterRule> {
    FilterKind kind = FilterKind::Name;
    FilterAction action = FilterAction::Exclude;
    FilterTarget target = FilterTarget::Directories;
    PatternMode mode = PatternMode::Glob;
    String patterns;
    bool case_sensitive = false;
    int level = 0; // 0 = any; 1 = immediate children of the source
    int limit = 3;
    double min_size = 0, max_size = 0; // 0 = unbounded; applies only to files
    SizeUnit size_unit = SizeUnit::Kilobytes;
    Date modified_from, modified_to; // inclusive bounds; Null = unbounded
};

struct DirectoryEntry : Moveable<DirectoryEntry> {
    String full_path, relative_path, name, extension;
    bool is_dir = false, hidden = false;
    int64 size = 0;
    Time modified;
    int depth = 0;
};

// Aggregates all user-selected scan, filtering, sorting, and rendering options.
struct DirectoryScanSettings {
    String source_directory;
    WithDeepCopy<Vector<ScanFilterRule>> filter_rules;

    bool recursive = true;
    int  recursive_depth = 2; // extra levels below immediate source children
    bool include_directories = true;
    bool include_files = true;
    bool show_hidden = false;
    bool reverse_sort = false;
    bool enable_sorting = true;
    bool enable_filtering = false;

    DirSortKey sort_key = DirSortKey::Name;
    DirSortKey secondary_sort_key = DirSortKey::Type;
    DirPlacement dir_placement = DirPlacement::DirsFirst;
    SlashMode slash_mode = SlashMode::Native;
    OutputFormat output_format = OutputFormat::Text;

    bool show_path = false;
    bool show_size = false;
    bool show_date = false;
    bool show_extension = true;
};

struct DirectoryOutputLine : Moveable<DirectoryOutputLine> {
    String text;
    bool   is_dir = false;
};

// Stateless engine that scans a directory tree and renders the result.
class DirectoryEngine {
public:
    // Validates settings, collects entries, sorts them, and renders the chosen format.
    static Vector<DirectoryEntry> Scan(const DirectoryScanSettings& settings);
    static String Generate(const DirectoryScanSettings& settings);
    static Vector<DirectoryOutputLine> GenerateTextLines(const DirectoryScanSettings& settings);
};

}

#endif
