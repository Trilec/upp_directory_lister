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
};

// Units used by the size filter controls.
enum class SizeUnit : byte {
    Bytes,
    Kilobytes,
    Megabytes,
    Gigabytes,
};

// Aggregates all user-selected scan, filtering, sorting, and rendering options.
struct DirectoryScanSettings {
    String source_directory;
    String file_patterns;
    String directory_patterns;

    bool recursive = true;
    int  recursive_depth = 2;
    bool include_directories = true;
    bool include_files = true;
    bool show_hidden = false;
    bool reverse_sort = false;

    bool enable_size_filter = false;
    double min_size = 0;
    double max_size = 0;
    SizeUnit size_unit = SizeUnit::Kilobytes;

    bool enable_date_filter = false;
    Date modified_from;
    Date modified_to;

    DirSortKey sort_key = DirSortKey::Name;
    DirSortKey secondary_sort_key = DirSortKey::Type;
    DirPlacement dir_placement = DirPlacement::DirsFirst;
    SlashMode slash_mode = SlashMode::Native;
    OutputFormat output_format = OutputFormat::Text;

    bool show_path = true;
    bool show_size = true;
    bool show_date = true;
    bool show_extension = false;
};

// Stateless engine that scans a directory tree and renders the result.
class DirectoryEngine {
public:
    // Validates settings, collects entries, sorts them, and renders the chosen format.
    static String Generate(const DirectoryScanSettings& settings);
};

}

#endif
