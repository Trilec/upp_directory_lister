// DirLister - main desktop window for scanning, previewing, and exporting listings.
// Change log:
// - 2026-04-22: Added documentation comments and fixed status text handling.

#include "MainWindow.h"

namespace Upp {

namespace {

using namespace DirListerTheme;

class HelpDialog : public TopWindow {
public:
    typedef HelpDialog CLASSNAME;

    HelpDialog()
    {
        Title("DirLister Help");
        Sizeable().Zoomable();
        SetRect(0, 0, DPI(900), DPI(720));

        Add(shell_);
        shell_.Add(title_);
        shell_.Add(doc_panel_);
        shell_.Add(close_);
        doc_panel_.Add(doc_);

        title_.SetTitle("DirLister Help")
              .SetSubTitle("How to use setup, rename, transfer, and apply flows")
              .SetCopyText(String())
              .SetMedia(ICON_DESIGN_SETTINGS_48(), Size(DPI(16), DPI(16)))
              .SetMediaSide(UiAlign::LEFT)
              .SetMediaReserve(DPI(16));
        title_.ShowRule(false).ShowBottomLine(false).EnableHover(false).SetSelectable(false);

        close_.SetText("Close");
        close_.WhenAction << [=] { Close(); };

        doc_.EnableRich().SetSelectable().SetAlign(UiAlign::LEFT, UiAlign::TOP);
        auto heading = [&](const String& text, Color ink = BlueText()) {
            doc_.AddTextSpan(text, ink, true);
            doc_.AddNewlineSpan();
        };
        auto body = [&](const String& text, Color ink = Text()) {
            doc_.AddTextSpan(text, ink);
            doc_.AddNewlineSpan();
        };
        auto gap = [&] { doc_.AddNewlineSpan(); };

        heading("Overview");
        body("DirLister helps you filter a folder, generate a clean listing, preview rename rules, and copy matching files to another location.");
        body("The normal workflow is: choose a source directory, set file and directory filters, generate a listing, then optionally rename or transfer the filtered result.");
        gap();

        heading("How Filtering Works", BlueText());
        body("Files and directories are filtered independently.");
        body("- File Pattern only affects files");
        body("- Directory Pattern only affects directories");
        body("- You can include files, directories, or both");
        body("- Rename and Transfer also use the active Filter settings");
        gap();

        heading("Pattern Modes", BlueText());
        body("Both files and directories have their own matching mode.");
        body("- Glob: wildcard matching such as *.cpp, src*, data_??.json", BlueText());
        body("- Contains: simple substring matching such as log, temp, 2026", BlueText());
        body("Examples:");
        body("- File Glob: *.cpp matches C++ source files");
        body("- File Contains: log matches any filename containing log");
        body("- Directory Glob: DOC* matches folders starting with DOC");
        body("- Directory Contains: art matches folders containing art anywhere in the name");
        body("Case Sensitive can also be enabled separately for files and directories.");
        gap();

        heading("1. Filter / Listing");
        body("Use the Filter page to choose a source directory, apply file and directory rules, and generate a clean listing.");
        body("- File Pattern only filters files");
        body("- Directory Pattern only filters directories");
        body("- File Mode and Directory Mode support Glob and Contains matching");
        body("- Case Sensitive can be set independently for files and directories");
        body("- Size Threshold and Date Range can further narrow the result");
        body("- Sorting & Structure controls ordering and how directories are grouped");
        body("- Display Options control whether path, size, date, and extension are shown");
        gap();

        heading("Why It Is Helpful", GreenText());
        body("DirLister makes it easy to produce a copy/paste-ready inventory of a project tree, media folder, backup source, or archive.");
        gap();

        heading("Example");
        body("Source: D:/projects/source");
        body("File Pattern: *.cpp;*.h");
        body("Recursive: On, Depth: 2");
        body("Result: a formatted list of source files and folders ready for export or review.");
        gap();

        heading("2. Rename", GreenText());
        body("The Rename page lets you build a process stack and preview the result before applying it.");
        body("Rename only works on entries allowed by the current Filter settings.");
        body("Supported process types include Search & Replace, Case Transform, Alphanumeric Only, Numbering, Prefix, Extension Replace, Insert Left, and Insert Right.");
        body("How to use it:");
        body("- Choose a process type");
        body("- Fill in the parameters");
        body("- Use Add to push it into the stack");
        body("- Drag rows in the stack to change order");
        body("- Enter a sample name in the preview input if needed");
        body("- Review the sample results from the current source directory");
        body("- Click Apply Rename to rename matching entries in the active source directory");
        gap();

        heading("Safety Notes", AmberText());
        body("- Rename asks for confirmation before applying");
        body("- The app uses a two-phase rename pass to reduce collision problems");
        body("- Preview first if you are changing extensions or using numbering");
        gap();

        heading("Rename Example");
        body("Process stack:");
        body("1) Search & Replace: space -> _");
        body("2) Case: lower");
        body("3) Prefix: archived_");
        body("Result: My File.TXT becomes archived_my_file.txt");
        gap();

        heading("3. Transfer", AmberText());
        body("The Transfer page copies files and folders from the active source directory to a target directory.");
        body("Transfer also uses the current Filter settings, so only matching entries are copied.");
        body("Options include Preserve Tree, Flatten Files, Verify MD5 Hashes, and conflict handling.");
        body("Conflict handling:");
        body("- Auto-Increment: creates a new name if the target exists");
        body("- Overwrite Existing: replaces the target file");
        body("- Skip Existing: leaves existing files unchanged");
        gap();

        heading("Transfer Example");
        body("Source: D:/photos/2026");
        body("Target: E:/backup/photos");
        body("Preserve Tree: On");
        body("Conflict: Auto-Increment");
        body("Result: the folder tree is recreated in the backup target and name collisions are preserved safely.");
        gap();

        heading("Preview / Apply Flow");
        body("- Generate List previews the directory contents in the main output area");
        body("- Rename preview shows how stacked operations will change names");
        body("- Apply Rename performs the actual rename after confirmation");
        body("- Apply Transfer performs the actual copy after confirmation");
        body("- Operation reports are written to the main output panel");
        gap();

        heading("Good Practice");
        body("- Use shallow depth first when testing a new setup");
        body("- Preview rename stacks on a sample directory before applying broadly");
        body("- Use Skip Existing or Auto-Increment when copying into an existing archive");
        body("- Keep Linux Slashes enabled if you need copy/paste-friendly paths for tools or documentation");
    }

    virtual void Layout() override
    {
        shell_.SetRect(GetSize());
        int m = DPI(14);
        title_.SetRect(m, m, GetSize().cx - m * 2, DPI(34));
        close_.SetRect(GetSize().cx - m - DPI(90), GetSize().cy - m - DPI(30), DPI(90), DPI(30));
        doc_panel_.SetRect(m, DPI(56), GetSize().cx - m * 2, GetSize().cy - DPI(112));
        doc_.SetRect(DPI(10), DPI(10), doc_panel_.GetSize().cx - DPI(20), doc_panel_.GetSize().cy - DPI(20));
    }

    virtual void Paint(Draw& w) override
    {
        w.DrawRect(GetSize(), BodyBg());
    }

    void ApplyTheme()
    {
        shell_.SetStyle(MakePanelStyle(BodyBg(), BodyBg(), 0, 14));
        doc_panel_.SetStyle(MakePanelStyle(BodyBg(), Border(), 4, 0));

        UiTitleCard::Style title_style = UiTheme::ResolveTitleCard();
        for(int i = 0; i < 4; i++) {
            title_style.palette.face[i] = UiFill::None();
            title_style.palette.frame[i] = Null;
            title_style.palette.ink[i] = White();
        }
        title_style.transparent = true;
        title_style.metrics.face_enabled = false;
        title_style.metrics.frame_enabled = false;
        title_style.metrics.content_margin = Rect(0, 0, 0, 0);
        title_style.title_font = AppSans(18, true);
        title_style.subtitle_font = AppSans(9, false);
        title_style.subtitle_color = BlueText();
        title_style.media_side = UiAlign::LEFT;
        title_style.media_gap = DPI(8);
        title_.SetStyle(title_style);

        {
            UiLabel::Style s = MakeLabelStyle(Text(), UiLabelRole::Body, false);
            for(int i = 0; i < 4; i++) {
                s.palette.face[i] = UiFill::Solid(BodyBg());
                s.palette.frame[i] = Null;
                s.palette.ink[i] = Text();
            }
            s.align_h = UiAlign::LEFT;
            s.align_v = UiAlign::TOP;
            s.font = AppSans(10);
            s.metrics.text_font = s.font;
            s.metrics.use_text_font = true;
            s.metrics.face_enabled = true;
            s.metrics.frame_enabled = false;
            s.metrics.content_margin = Rect(0, 0, 0, 0);
            s.transparent = false;
            doc_.SetStyle(s);
        }
        close_.SetStyle(MakeActionStyle(false));
    }

private:
    UiPanel shell_;
    UiTitleCard title_;
    UiPanel doc_panel_;
    UiLabel doc_;
    UiButton close_;
};

// Human-readable descriptions for the rename operator editor.
String RenameTypeDescription(RenameStepType type)
{
    switch(type) {
    case RenameStepType::FindReplace: return "Find and replace matching text.";
    case RenameStepType::Case: return "Transform case across the entire name.";
    case RenameStepType::Alnum: return "Strip non-alphanumeric characters except extension dot.";
    case RenameStepType::Numbering: return "Replace a token with padded sequence numbers.";
    case RenameStepType::Prefix: return "Add text at the start of the name.";
    case RenameStepType::Extension: return "Force the file extension to a new value.";
    case RenameStepType::InsertLeft: return "Insert text at an index from the left.";
    case RenameStepType::InsertRight: return "Insert text at an index from the right.";
    }
    return String();
}

String GetStepData(const RenameStep& step, const char* key, const String& def = String())
{
    int i = step.data.Find(key);
    return i >= 0 ? step.data[i] : def;
}

void SetStepData(RenameStep& step, const char* key, const String& value)
{
    int i = step.data.Find(key);
    if(i >= 0)
        step.data[i] = value;
    else
        step.data.Add(key, value);
}

String NormalizePathSlashes(String path, bool linux_slashes)
{
    if(linux_slashes)
        path.Replace("\\", "/");
    else
        path.Replace("/", "\\");
    return path;
}

RenameStepType SelectedRenameType(const UiDropdown& ddl)
{
    return (RenameStepType)(int)ddl.GetSelectedData();
}

RenameStep CopyStep(const RenameStep& src)
{
    RenameStep out;
    out.id = src.id;
    out.type = src.type;
    for(int i = 0; i < src.data.GetCount(); i++)
        out.data.Add(src.data.GetKey(i), src.data[i]);
    return out;
}

struct FilePlanItem : Moveable<FilePlanItem> {
    String source_path;
    String relative_path;
    String source_name;
    String target_name;
    String target_path;
    bool   is_dir = false;
};

Vector<String> SplitPatternsText(const String& text, bool case_sensitive)
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

bool MatchWildcardI2(const char* pattern, const char* text, bool case_sensitive)
{
    while(*pattern) {
        if(*pattern == '*') {
            pattern++;
            if(!*pattern)
                return true;
            while(*text) {
                if(MatchWildcardI2(pattern, text, case_sensitive))
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

bool MatchesPatternSet(const Vector<String>& patterns, const String& text, bool case_sensitive)
{
    if(patterns.IsEmpty())
        return true;
    String candidate = case_sensitive ? text : ToLower(text);
    for(const String& pattern : patterns)
        if(MatchWildcardI2(pattern, candidate, case_sensitive))
            return true;
    return false;
}

bool MatchesContainsSet(const Vector<String>& patterns, const String& text, bool case_sensitive)
{
    if(patterns.IsEmpty())
        return true;
    String candidate = case_sensitive ? text : ToLower(text);
    for(const String& pattern : patterns)
        if(candidate.Find(pattern) >= 0)
            return true;
    return false;
}

bool MatchesConfiguredSet(const Vector<String>& patterns,
                         const String& text,
                         bool case_sensitive,
                         PatternMode mode)
{
    if(mode == PatternMode::Contains)
        return MatchesContainsSet(patterns, text, case_sensitive);
    return MatchesPatternSet(patterns, text, case_sensitive);
}

bool TypePatternMatch2(bool is_dir,
                       const Vector<String>& file_patterns,
                       const Vector<String>& dir_patterns,
                       const String& text,
                       bool file_case_sensitive,
                       bool dir_case_sensitive,
                       PatternMode file_mode,
                       PatternMode dir_mode)
{
    if(is_dir)
        return dir_patterns.IsEmpty() || MatchesConfiguredSet(dir_patterns, text, dir_case_sensitive, dir_mode);
    return file_patterns.IsEmpty() || MatchesConfiguredSet(file_patterns, text, file_case_sensitive, file_mode);
}

String MakeRelativePath(const String& root, const String& full)
{
    String rel = full.Mid(root.GetCount());
    if(rel.StartsWith("\\") || rel.StartsWith("/"))
        rel = rel.Mid(1);
    return rel;
}

void CollectTransferEntries(Vector<FilePlanItem>& out,
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

        bool is_dir = ff.IsFolder();
        bool hidden = ff.IsHidden();
        if(hidden && !settings.show_hidden) {
            if(is_dir && settings.recursive && depth < settings.recursive_depth)
                CollectTransferEntries(out, root, AppendFileName(current, name), depth + 1, settings, file_patterns, dir_patterns);
            ff.Next();
            continue;
        }

        bool pattern_match = TypePatternMatch2(is_dir,
                                              file_patterns,
                                              dir_patterns,
                                              name,
                                              settings.file_case_sensitive,
                                              settings.dir_case_sensitive,
                                              settings.file_pattern_mode,
                                              settings.dir_pattern_mode);
        String full = AppendFileName(current, name);
        String rel = MakeRelativePath(root, full);

        if(pattern_match) {
            FilePlanItem item;
            item.source_path = full;
            item.relative_path = rel;
            item.source_name = name;
            item.is_dir = is_dir;
            if(is_dir && settings.include_directories)
                out.Add(item);
            if(!is_dir && settings.include_files)
                out.Add(item);
        }

        if(is_dir && settings.recursive && depth < settings.recursive_depth)
            CollectTransferEntries(out, root, full, depth + 1, settings, file_patterns, dir_patterns);

        ff.Next();
    }
}

void CollectRenameEntries(Vector<FilePlanItem>& out,
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

        bool is_dir = ff.IsFolder();
        bool hidden = ff.IsHidden();
        String full = AppendFileName(current, name);
        FilePlanItem item;
        item.source_path = full;
        item.relative_path = MakeRelativePath(root, full);
        item.source_name = name;
        item.is_dir = is_dir;

        bool pattern_match = TypePatternMatch2(is_dir,
                                              file_patterns,
                                              dir_patterns,
                                              name,
                                              settings.file_case_sensitive,
                                              settings.dir_case_sensitive,
                                              settings.file_pattern_mode,
                                              settings.dir_pattern_mode);
        bool include_type = (is_dir && settings.include_directories) || (!is_dir && settings.include_files);
        bool pass_size = true;
        bool pass_date = true;
        if(settings.enable_size_filter && !is_dir) {
            double scale = settings.size_unit == SizeUnit::Bytes ? 1.0
                         : settings.size_unit == SizeUnit::Kilobytes ? 1024.0
                         : settings.size_unit == SizeUnit::Megabytes ? 1024.0 * 1024.0
                         : 1024.0 * 1024.0 * 1024.0;
            int64 len = ff.GetLength();
            if(settings.min_size > 0 && (double)len < settings.min_size * scale) pass_size = false;
            if(settings.max_size > 0 && (double)len > settings.max_size * scale) pass_size = false;
        }
        if(settings.enable_date_filter) {
            Time tm = ff.GetLastWriteTime();
            if(!IsNull(tm)) {
                Date d(tm.year, tm.month, tm.day);
                if(!IsNull(settings.modified_from) && d < settings.modified_from) pass_date = false;
                if(!IsNull(settings.modified_to) && d > settings.modified_to) pass_date = false;
            }
        }

        if((!hidden || settings.show_hidden) && include_type && pattern_match && pass_size && pass_date)
            out.Add(item);

        if(is_dir && settings.recursive && depth < settings.recursive_depth)
            CollectRenameEntries(out, root, full, depth + 1, settings, file_patterns, dir_patterns);

        ff.Next();
    }
}

String UniqueTargetPath(String path)
{
    if(!FileExists(path) && !DirectoryExists(path))
        return path;
    String dir = GetFileFolder(path);
    String name = GetFileName(path);
    String base = GetFileTitle(name);
    String ext = GetFileExt(name);
    for(int i = 1;; i++) {
        String next = AppendFileName(dir, Format("%s-%d%s", base, i, ext));
        if(!FileExists(next) && !DirectoryExists(next))
            return next;
    }
}

bool CopyFileVerified(const String& src, const String& dst, bool verify_hash)
{
    if(!FileCopy(src, dst))
        return false;
    if(!verify_hash)
        return true;
    return LoadFile(src) == LoadFile(dst);
}

}

MainWindow::MainWindow()
{
    Title("DirLister Pro");
    Sizeable().Zoomable();
    SetRect(0, 0, DPI(1280), DPI(760));

    UiThemeContext ctx;
    ctx.preset = UiThemePreset::Rounded;
    ctx.mode = UiThemeMode::Dark;
    UiTheme::SetContext(ctx);

    BuildUi();
    ApplyTheme();
    ResetRenameModel();
    SetSidebarPage(0);
    UpdateStatus("READY", false);
    UpdateFooterPath();
}

void MainWindow::BuildUi()
{
    // Build the static control tree once. Per-control geometry is handled in Layout().
    Add(sidebar_panel_);
    Add(main_panel_);

    sidebar_panel_.Add(title_card_);
    sidebar_panel_.Add(version_badge_);
    sidebar_panel_.Add(source_label_);
    sidebar_panel_.Add(source_edit_);
    sidebar_panel_.Add(source_browse_);
    sidebar_panel_.Add(source_history_);
    sidebar_panel_.Add(nav_panel_);
    nav_panel_.Add(nav_setup_button_);
    nav_panel_.Add(nav_rename_button_);
    nav_panel_.Add(nav_transfer_button_);
    nav_panel_.Add(scan_filter_badge_);
    sidebar_panel_.Add(setup_page_);
    sidebar_panel_.Add(rename_page_);
    sidebar_panel_.Add(transfer_page_);

    main_panel_.Add(generate_button_);
    main_panel_.Add(abort_button_);
    main_panel_.Add(help_button_);
    main_panel_.Add(exit_button_);
    main_panel_.Add(output_format_);
    main_panel_.Add(slash_mode_);
    main_panel_.Add(state_label_);
    main_panel_.Add(output_copy_button_);
    main_panel_.Add(output_copy_label_);
    main_panel_.Add(output_panel_);
    main_panel_.Add(footer_meta_);
    main_panel_.Add(footer_path_);
    output_panel_.Add(output_scroll_panel_);
    output_scroll_panel_.Content().Add(output_edit_);
    output_scroll_panel_.SetScrollMode(UIPANELSCROLL_VERTICAL);

    title_card_.SetTitle("DirLister Pro")
               .SetSubTitle("Directory listing and renaming tool")
               .SetCopyText(String())
               .SetMedia(CtrlImg::Dir(), Size(DPI(16), DPI(16)))
               .SetMediaSide(UiAlign::LEFT)
               .SetMediaReserve(DPI(16));
    title_card_.ShowRule(false).ShowBottomLine(false).EnableHover(false).SetSelectable(false);
    version_badge_.SetText("v0.12");

    source_label_.SetText("SOURCE DIRECTORY");
    source_edit_.SetPlaceholder("D:/projects/source");
    source_edit_.SetData(GetCurrentDirectory());
    source_edit_.WhenChange << [=] {
        UpdateFooterPath();
        RefreshRenamePreview();
    };
    source_browse_.SetText("...");
    source_browse_.WhenAction << [=] { HandleBrowseSource(); };

    source_history_.Add("Location History...", String())
                   .Add("Current Directory", GetCurrentDirectory())
                   .Add("Workspace Root", GetCurrentDirectory())
                   .Add("Temp Directory", GetTempPath())
                   .Select(0);
    source_history_.WhenSelectData << [=](const Value& v) {
        if(IsNull(v) || v.ToString().IsEmpty())
            return;
        source_edit_.SetData(v);
        UpdateFooterPath();
        RefreshRenamePreview();
    };
    auto filter_changed = [=] {
        UpdateFilterIndicator();
        RefreshRenamePreview();
    };

    nav_setup_button_.SetText("FILTER");
    nav_rename_button_.SetText("RENAME");
    nav_transfer_button_.SetText("TRANSFER");
    nav_setup_button_.WhenAction << [=] { SetSidebarPage(0); };
    nav_rename_button_.WhenAction << [=] { SetSidebarPage(1); };
    nav_transfer_button_.WhenAction << [=] { SetSidebarPage(2); };

    AddSidebarPages();

    generate_button_.SetText("GENERATE LIST");
    generate_button_.WhenAction << [=] { HandleGenerate(); };
    abort_button_.SetText("ABORT");
    abort_button_.WhenAction << [=] { HandleAbort(); };
    help_button_.SetText("HELP");
    help_button_.SetIcon(ICON_DESIGN_SETTINGS_48()).SetIconSize(15, 15).SetContentGap(DPI(10));
    help_button_.WhenAction << [=] { HandleHelp(); };

    output_format_.Add("Text Output", (int)OutputFormat::Text)
                  .Add("CSV Output", (int)OutputFormat::Csv)
                  .Add("JSON Output", (int)OutputFormat::Json)
                  .Select(0);
    slash_mode_.Add("Native Slashes", (int)SlashMode::Native)
               .Add("Linux Slashes", (int)SlashMode::Unix)
               .Select(0);
    slash_mode_.WhenSelect << [=](int) { HandleSlashModeChanged(); };

    state_label_.SetText("IDLE");
    exit_button_.SetText("EXIT");
    exit_button_.SetIcon(ICON_NAVIGATION_EXIT_TO_APP_48()).SetIconSize(15, 15).SetContentGap(DPI(10));
    exit_button_.WhenAction << [=] { Close(); };
    output_copy_button_.SetIcon(ICON_CONTENT_CONTENT_COPY_48())
                       .SetIconSize(14, 14)
                       .SetIconRenderMode(UiIconRenderMode::MonoTint)
                       .SetIconColor(Color(156, 163, 175));
    output_copy_button_.WhenAction << [=] { WriteClipboardText(output_edit_.GetData().ToString()); };
    output_copy_label_.SetText("Copy Output");
    output_edit_.EnableRich().SetSelectable().SetAlign(UiAlign::LEFT, UiAlign::TOP);
    output_edit_.SetText(String());
    footer_meta_.SetText(String());
    footer_path_.SetText(source_edit_.GetData().ToString());
}

void MainWindow::AddSidebarPages()
{
    auto filter_changed = [=] {
        UpdateFilterIndicator();
        RefreshRenamePreview();
    };

    ParentCtrl& setup = setup_page_;
    setup.Add(setup_file_pattern_label_);
    setup.Add(setup_filter_hint_);
    setup.Add(file_pattern_mode_label_);
    setup.Add(file_pattern_mode_);
    setup.Add(file_case_sensitive_);
    setup.Add(setup_file_pattern_);
    setup.Add(dir_pattern_mode_label_);
    setup.Add(dir_pattern_mode_);
    setup.Add(dir_case_sensitive_);
    setup.Add(setup_dir_pattern_);
    setup.Add(size_threshold_label_);
    setup.Add(size_filter_toggle_);
    setup.Add(size_min_);
    setup.Add(size_max_);
    setup.Add(size_unit_);
    setup.Add(date_range_label_);
    setup.Add(date_filter_toggle_);
    setup.Add(date_from_);
    setup.Add(date_to_);
    setup.Add(sort_label_);
    setup.Add(sort_primary_);
    setup.Add(sort_secondary_);
    setup.Add(dir_placement_);
    setup.Add(reverse_sort_);
    setup.Add(recursive_scan_);
    setup.Add(depth_label_);
    setup.Add(depth_limit_);
    setup.Add(include_dirs_);
    setup.Add(include_files_);
    setup.Add(show_hidden_);
    setup.Add(display_label_);
    setup.Add(show_path_);
    setup.Add(show_size_);
    setup.Add(show_date_);
    setup.Add(show_extension_);

    setup_file_pattern_label_.SetText("FILTER RULES");
    setup_filter_hint_.SetText("File Pattern only filters files. Directory Pattern only filters directories.");
    file_pattern_mode_label_.SetText("File Mode");
    file_pattern_mode_.Add("Glob", (int)PatternMode::Glob)
                      .Add("Contains", (int)PatternMode::Contains)
                      .Select(0);
    file_pattern_mode_.WhenSelect << [=](int) { filter_changed(); };
    file_case_sensitive_.SetText("Case Sensitive");
    file_case_sensitive_.WhenAction << [=] { filter_changed(); };
    setup_file_pattern_.SetPlaceholder("File Pattern (e.g. *.cpp;*.h)");
    setup_file_pattern_.WhenChange << [=] { filter_changed(); };
    dir_pattern_mode_label_.SetText("Directory Mode");
    dir_pattern_mode_.Add("Glob", (int)PatternMode::Glob)
                     .Add("Contains", (int)PatternMode::Contains)
                     .Select(0);
    dir_pattern_mode_.WhenSelect << [=](int) { filter_changed(); };
    dir_case_sensitive_.SetText("Case Sensitive");
    dir_case_sensitive_.WhenAction << [=] { filter_changed(); };
    setup_dir_pattern_.SetPlaceholder("Directory Pattern (e.g. src;bin)");
    setup_dir_pattern_.WhenChange << [=] { filter_changed(); };

    size_threshold_label_.SetText("SIZE THRESHOLD");
    size_filter_toggle_.SetText("Enable").SetChecked(false);
    size_filter_toggle_.WhenAction << [=] { filter_changed(); };
    StyleEditField(size_min_, "Min");
    StyleEditField(size_max_, "Max");
    size_min_.SetData(0);
    size_max_.SetData(0);
    size_min_.WhenAction << [=] { filter_changed(); };
    size_max_.WhenAction << [=] { filter_changed(); };
    size_unit_.Add("B", (int)SizeUnit::Bytes)
              .Add("KB", (int)SizeUnit::Kilobytes)
              .Add("MB", (int)SizeUnit::Megabytes)
              .Add("GB", (int)SizeUnit::Gigabytes)
              .Select(1);
    size_unit_.WhenSelect << [=](int) { filter_changed(); };

    date_range_label_.SetText("DATE RANGE");
    date_filter_toggle_.SetText("Enable").SetChecked(false);
    date_filter_toggle_.WhenAction << [=] { filter_changed(); };
    date_from_.SetBackground(BodyBg()).SetColor(White()).SetFont(AppSans(10));
    date_to_.SetBackground(BodyBg()).SetColor(White()).SetFont(AppSans(10));
    date_from_.WhenAction << [=] { filter_changed(); };
    date_to_.WhenAction << [=] { filter_changed(); };

    sort_label_.SetText("SORTING & STRUCTURE");
    sort_primary_.Add("Primary: Name", (int)DirSortKey::Name)
                 .Add("Primary: Size", (int)DirSortKey::Size)
                 .Add("Primary: Date", (int)DirSortKey::Modified)
                 .Add("Primary: Type", (int)DirSortKey::Type)
                 .Add("Primary: Unsorted", (int)DirSortKey::Unsorted)
                 .Select(0);
    sort_secondary_.Add("Sub: Ext", (int)DirSortKey::Type)
                   .Add("Sub: Name", (int)DirSortKey::Name)
                   .Add("Sub: Size", (int)DirSortKey::Size)
                   .Add("Sub: Date", (int)DirSortKey::Modified)
                   .Add("Sub: None", (int)DirSortKey::Unsorted)
                   .Select(0);
    dir_placement_.Add("Dirs First", (int)DirPlacement::DirsFirst)
                  .Add("Dirs Last", (int)DirPlacement::DirsLast)
                  .Add("Inline", (int)DirPlacement::Inline)
                  .Select(0);
    sort_primary_.WhenSelect << [=](int) { filter_changed(); };
    sort_secondary_.WhenSelect << [=](int) { filter_changed(); };
    dir_placement_.WhenSelect << [=](int) { filter_changed(); };
    reverse_sort_.SetText("Reverse");
    reverse_sort_.WhenAction << [=] { filter_changed(); };
    recursive_scan_.SetText("Recursive Scanning").SetChecked(true);
    recursive_scan_.WhenAction << [=] { filter_changed(); };
    depth_label_.SetText("Depth");
    StyleEditField(depth_limit_);
    depth_limit_.SetData(2);
    depth_limit_.WhenAction << [=] { filter_changed(); };
    include_dirs_.SetText("Include Dirs").SetChecked(true);
    include_dirs_.WhenAction << [=] { filter_changed(); };
    include_files_.SetText("Include Files").SetChecked(true);
    include_files_.WhenAction << [=] { filter_changed(); };
    show_hidden_.SetText("Show Hidden");
    show_hidden_.WhenAction << [=] { filter_changed(); };

    display_label_.SetText("DISPLAY OPTIONS");
    show_path_.SetText("Path").SetChecked(true);
    show_size_.SetText("Size").SetChecked(true);
    show_date_.SetText("Date").SetChecked(true);
    show_extension_.SetText("Ext");
    scan_filter_badge_.SetText("ON");

    ParentCtrl& rename = rename_page_;
    rename.Add(rename_operator_label_);
    rename.Add(rename_filter_hint_);
    rename.Add(rename_add_type_);
    rename.Add(rename_params_label_);
    rename.Add(rename_mode_);
    rename.Add(rename_param_a_);
    rename.Add(rename_param_b_);
    rename.Add(rename_param_c_);
    rename.Add(rename_save_button_);
    rename.Add(rename_add_button_);
    rename.Add(rename_remove_button_);
    rename.Add(rename_apply_button_);
    rename.Add(rename_steps_label_);
    rename.Add(rename_stack_panel_);
    rename_stack_panel_.Add(rename_stack_);
    rename.Add(rename_preview_input_);
    rename.Add(rename_preview_label_);
    rename.Add(rename_preview_count_);
    rename.Add(rename_preview_panel_);
    rename_preview_panel_.Add(rename_preview_view_);

    rename_operator_label_.SetText("PROCESS");
    rename_filter_hint_.SetText("Active Scan Filter rules limit which entries are previewed and renamed.");
    rename_add_type_.Add("Search & Replace", (int)RenameStepType::FindReplace)
                    .Add("Case Transform", (int)RenameStepType::Case)
                    .Add("Alphanumeric Only", (int)RenameStepType::Alnum)
                    .Add("Numbering", (int)RenameStepType::Numbering)
                    .Add("Prefix", (int)RenameStepType::Prefix)
                    .Add("Extension Replace", (int)RenameStepType::Extension)
                    .Add("Insert Left", (int)RenameStepType::InsertLeft)
                    .Add("Insert Right", (int)RenameStepType::InsertRight)
                    .Select(0);
    rename_add_type_.WhenSelect << [=](int) {
        if(rename_ui_syncing_)
            return;
        rename_edit_step_ = RenameEngine::MakeDefaultStep(SelectedRenameType(rename_add_type_));
        HandleRenameFieldsChanged();
        RefreshRenameUi();
    };
    rename_params_label_.SetText("PARAMETERS");
    rename_mode_.WhenSelect << [=](int) { HandleRenameFieldsChanged(); };
    rename_param_a_.WhenChange << [=] { HandleRenameFieldsChanged(); };
    rename_param_b_.WhenChange << [=] { HandleRenameFieldsChanged(); };
    rename_param_c_.WhenChange << [=] { HandleRenameFieldsChanged(); };
    rename_save_button_.SetText("Save");
    rename_save_button_.WhenAction << [=] {
        if(rename_selected_step_ >= 0 && rename_selected_step_ < rename_steps_.GetCount()) {
            rename_steps_.Set(rename_selected_step_, CopyStep(rename_edit_step_));
            rename_dirty_ = false;
            RefreshRenameUi();
        }
    };
    rename_add_button_.SetText("Add");
    rename_add_button_.WhenAction << [=] { HandleRenameAdd(); };
    rename_remove_button_.SetText("Delete");
    rename_remove_button_.WhenAction << [=] { HandleRenameRemove(); };
    rename_apply_button_.SetText("Apply Rename");
    rename_apply_button_.WhenAction << [=] { HandleApplyRename(); };
    rename_steps_label_.SetText("STACK");
    rename_stack_.SetModel(rename_stack_model_);
    rename_stack_.SetSelectionMode(UILISTSEL_SINGLE);
    rename_stack_.EnableDragReorder(true).ShowDragHandle(true).SetDragSide(UiAlign::RIGHT);
    rename_stack_.WhenSelection << [=] { HandleRenameSelection(); };
    rename_preview_input_.SetPlaceholder("Preview input, e.g. example_file.txt");
    rename_preview_input_.WhenChange << [=] { RefreshRenamePreview(); };
    rename_preview_label_.SetText("PREVIEW");
    StyleEditField(rename_preview_count_);
    rename_preview_count_.SetData(8);
    rename_preview_count_.Min(1).Max(50);
    rename_preview_count_.WhenAction << [=] { RefreshRenamePreview(); };
    rename_preview_view_.SetReadOnly();
    rename_preview_view_.SetFrame(NullFrame());
    StyleDocEdit(rename_preview_view_, Text(), SidebarBg(), MonospaceZ(10));

    ParentCtrl& transfer = transfer_page_;
    transfer.Add(transfer_target_label_);
    transfer.Add(transfer_target_);
    transfer.Add(transfer_browse_);
    transfer.Add(transfer_preserve_tree_);
    transfer.Add(transfer_flatten_);
    transfer.Add(transfer_conflict_);
    transfer.Add(transfer_verify_hash_);
    transfer.Add(transfer_filter_hint_);
    transfer.Add(transfer_apply_button_);

    transfer_target_label_.SetText("TARGET DIRECTORY");
    transfer_target_.SetPlaceholder("D:/backups/current");
    transfer_browse_.SetText("...");
    transfer_browse_.WhenAction << [=] { HandleBrowseTransferTarget(); };
    transfer_preserve_tree_.SetText("Preserve Tree").SetChecked(true);
    transfer_flatten_.SetText("Flatten Files");
    transfer_conflict_.Add("Auto-Increment", 0)
                     .Add("Overwrite Existing", 1)
                     .Add("Skip Existing", 2)
                     .Select(0);
    transfer_verify_hash_.SetText("Verify MD5 Hashes").SetChecked(true);
    transfer_filter_hint_.SetText("Active Scan Filter rules limit which entries are transferred.");
    transfer_apply_button_.SetText("Apply Transfer");
    transfer_apply_button_.WhenAction << [=] { HandleApplyTransfer(); };
}

void MainWindow::ApplyTheme()
{
    // Keep appearance decisions centralized so UI logic stays focused on behavior.
    sidebar_panel_.SetStyle(MakePanelStyle(SidebarBg(), Border(), 0, 14));
    main_panel_.SetStyle(MakePanelStyle(BodyBg(), BodyBg(), 0, 14));
    output_panel_.SetStyle(MakePanelStyle(OutputBg(), Null, 0, 12));
    output_scroll_panel_.SetStyle(MakeScrollPanelStyle());
    output_scroll_panel_.Transparent();
    output_scroll_panel_.Content().Transparent();
    {
        UiScrollBar::Style sb = UiTheme::ResolveScrollBar();
        sb.thin_idle = false;
        sb.fade_idle = false;
        sb.thick_px = DPI(16);
        sb.thin_px = DPI(16);
        sb.thumb_paint_px_idle = DPI(12);
        sb.thumb_paint_px_hot = DPI(12);
        sb.track_paint_px_idle = DPI(16);
        sb.track_paint_px_hot = DPI(16);
        sb.track_palette.face[ST_NORMAL] = UiFill::Solid(Color(24, 28, 36));
        sb.track_palette.face[ST_HOT] = UiFill::Solid(Color(24, 28, 36));
        sb.track_palette.face[ST_PRESSED] = UiFill::Solid(Color(24, 28, 36));
        sb.track_palette.frame[ST_NORMAL] = Color(60, 67, 79);
        sb.track_palette.frame[ST_HOT] = Color(75, 85, 99);
        sb.track_palette.frame[ST_PRESSED] = Color(75, 85, 99);
        sb.track_metrics.face_enabled = true;
        sb.track_metrics.frame_enabled = true;
        sb.track_metrics.frame_width = DPI(1);
        sb.thumb_palette.face[ST_NORMAL] = Color(120, 130, 145);
        sb.thumb_palette.face[ST_HOT] = Color(148, 163, 184);
        sb.thumb_palette.face[ST_PRESSED] = Color(96, 165, 250);
        sb.thumb_metrics.face_enabled = true;
        sb.thumb_metrics.frame_enabled = false;
        sb.thumb_metrics.radius = DPI(6);
        output_scroll_panel_.SetScrollBarStyle(sb);
    }
    nav_panel_.SetStyle(MakePanelStyle(BodyBg(), Null, 8, 2));
    rename_stack_panel_.SetStyle(MakePanelStyle(BodyBg(), Border(), 4, 0));
    rename_preview_panel_.SetStyle(MakePanelStyle(BodyBg(), Border(), 4, 0));
    setup_page_.Transparent();
    rename_page_.Transparent();
    transfer_page_.Transparent();

    UiTitleCard::Style title_style = UiTheme::ResolveTitleCard();
    for(int i = 0; i < 4; i++) {
        title_style.palette.face[i] = UiFill::None();
        title_style.palette.frame[i] = Null;
        title_style.palette.ink[i] = White();
    }
    title_style.transparent = true;
    title_style.metrics.face_enabled = false;
    title_style.metrics.frame_enabled = false;
    title_style.metrics.content_margin = Rect(0, 0, 0, 0);
    title_style.title_font = AppSans(18, true);
    title_style.subtitle_font = AppSans(8, true);
    title_style.copy_font = AppSans(9, false);
    title_style.subtitle_color = BlueText();
    title_style.media_side = UiAlign::LEFT;
    title_style.media_gap = DPI(6);
    title_style.media_reserve = DPI(16);
    title_style.media_tint_mono = true;
    title_style.show_rule = false;
    title_style.show_bottom_line = false;
    title_card_.SetStyle(title_style);
    version_badge_.SetStyle(MakeBadgeStyle(BlueDark(), StatusText()));
    scan_filter_badge_.SetStyle(MakeBadgeStyle(GreenDark(), GreenText()));

    source_label_.SetStyle(MakeLabelStyle(Muted(), UiLabelRole::Caption));
    setup_file_pattern_label_.SetStyle(MakeLabelStyle(BlueText(), UiLabelRole::Caption));
    setup_filter_hint_.SetStyle(MakeLabelStyle(Muted(), UiLabelRole::Footnote));
    file_pattern_mode_label_.SetStyle(MakeLabelStyle(Muted(), UiLabelRole::Caption));
    dir_pattern_mode_label_.SetStyle(MakeLabelStyle(Muted(), UiLabelRole::Caption));
    size_threshold_label_.SetStyle(MakeLabelStyle(BlueText(), UiLabelRole::Caption));
    date_range_label_.SetStyle(MakeLabelStyle(BlueText(), UiLabelRole::Caption));
    sort_label_.SetStyle(MakeLabelStyle(BlueText(), UiLabelRole::Caption));
    depth_label_.SetStyle(MakeLabelStyle(Muted(), UiLabelRole::Caption));
    display_label_.SetStyle(MakeLabelStyle(BlueText(), UiLabelRole::Caption));
    rename_operator_label_.SetStyle(MakeLabelStyle(GreenText(), UiLabelRole::Caption));
    rename_filter_hint_.SetStyle(MakeLabelStyle(Muted(), UiLabelRole::Footnote));
    rename_params_label_.SetStyle(MakeLabelStyle(GreenText(), UiLabelRole::Caption));
    rename_steps_label_.SetStyle(MakeLabelStyle(GreenText(), UiLabelRole::Caption));
    rename_preview_label_.SetStyle(MakeLabelStyle(GreenText(), UiLabelRole::Caption));
    transfer_target_label_.SetStyle(MakeLabelStyle(AmberText(), UiLabelRole::Caption));
    transfer_filter_hint_.SetStyle(MakeLabelStyle(Muted(), UiLabelRole::Footnote));
    footer_meta_.SetStyle(MakeLabelStyle(Muted(), UiLabelRole::Footnote));
    footer_path_.SetStyle(MakeLabelStyle(Color(0x9c, 0xa3, 0xaf), UiLabelRole::Footnote));
    state_label_.SetStyle(MakeLabelStyle(Muted(), UiLabelRole::Caption));
    output_copy_label_.SetStyle(MakeLabelStyle(Muted(), UiLabelRole::Footnote));
    {
        UiLabel::Style s = MakeLabelStyle(OutputText(), UiLabelRole::Body, false);
        for(int i = 0; i < 4; i++) {
            s.palette.face[i] = UiFill::Solid(OutputBg());
            s.palette.frame[i] = Null;
            s.palette.ink[i] = OutputText();
        }
        s.align_h = UiAlign::LEFT;
        s.align_v = UiAlign::TOP;
        s.font = MonospaceZ(11);
        s.metrics.text_font = s.font;
        s.metrics.use_text_font = true;
        s.metrics.face_enabled = true;
        s.metrics.frame_enabled = false;
        s.metrics.content_margin = Rect(0, 0, 0, 0);
        s.transparent = false;
        output_edit_.SetStyle(s);
    }

    source_edit_.SetStyle(MakeEditStyle());
    setup_file_pattern_.SetStyle(MakeEditStyle());
    setup_dir_pattern_.SetStyle(MakeEditStyle());
    rename_param_a_.SetStyle(MakeEditStyle());
    rename_param_b_.SetStyle(MakeEditStyle());
    rename_param_c_.SetStyle(MakeEditStyle());
    rename_preview_input_.SetStyle(MakeEditStyle());
    transfer_target_.SetStyle(MakeEditStyle());
    StyleEditField(rename_preview_count_);

    UiList::Style list_style = UiTheme::ResolveList();
    list_style.palette.face[ST_NORMAL] = UiFill::Solid(BodyBg());
    list_style.palette.face[ST_HOT] = UiFill::Solid(Color(0x37, 0x41, 0x51));
    list_style.palette.face[ST_PRESSED] = UiFill::Solid(BlueDark());
    list_style.palette.frame[ST_NORMAL] = Null;
    list_style.palette.frame[ST_HOT] = Null;
    list_style.palette.frame[ST_PRESSED] = Null;
    list_style.palette.ink[ST_NORMAL] = Text();
    list_style.palette.ink[ST_HOT] = White();
    list_style.palette.ink[ST_PRESSED] = White();
    list_style.metrics.face_enabled = false;
    list_style.metrics.frame_enabled = false;
    list_style.metrics.focus_enabled = false;
    list_style.font = AppSans(10);
    list_style.row_height = DPI(24);
    list_style.show_icons = false;
    list_style.show_checks = false;
    list_style.show_metadata_marker = false;
    list_style.hot_face = Color(0x37, 0x41, 0x51);
    list_style.hot_ink = White();
    list_style.selected_face = BlueDark();
    list_style.selected_ink = White();
    list_style.selected_frame = Null;
    list_style.separator_color = Border();
    rename_stack_.SetStyle(list_style);

    source_history_.SetStyle(MakeDropdownStyle());
    file_pattern_mode_.SetStyle(MakeDropdownStyle());
    dir_pattern_mode_.SetStyle(MakeDropdownStyle());
    output_format_.SetStyle(MakeDropdownStyle());
    slash_mode_.SetStyle(MakeDropdownStyle());
    size_unit_.SetStyle(MakeDropdownStyle());
    sort_primary_.SetStyle(MakeDropdownStyle());
    sort_secondary_.SetStyle(MakeDropdownStyle());
    dir_placement_.SetStyle(MakeDropdownStyle());
    rename_add_type_.SetStyle(MakeDropdownStyle());
    rename_mode_.SetStyle(MakeDropdownStyle());
    transfer_conflict_.SetStyle(MakeDropdownStyle());

    UiCheckBox::Style check_style = MakeCheckStyle();
    size_filter_toggle_.SetVisual(UICHECKVIS_CLASSIC).SetStyle(check_style).SetSizeMin(0, DPI(20));
    file_case_sensitive_.SetVisual(UICHECKVIS_CLASSIC).SetStyle(check_style).SetSizeMin(0, DPI(20));
    dir_case_sensitive_.SetVisual(UICHECKVIS_CLASSIC).SetStyle(check_style).SetSizeMin(0, DPI(20));
    date_filter_toggle_.SetVisual(UICHECKVIS_CLASSIC).SetStyle(check_style).SetSizeMin(0, DPI(20));
    reverse_sort_.SetVisual(UICHECKVIS_CLASSIC).SetStyle(check_style).SetSizeMin(0, DPI(20));
    recursive_scan_.SetVisual(UICHECKVIS_CLASSIC).SetStyle(check_style).SetSizeMin(0, DPI(20));
    include_dirs_.SetVisual(UICHECKVIS_CLASSIC).SetStyle(check_style).SetSizeMin(0, DPI(20));
    include_files_.SetVisual(UICHECKVIS_CLASSIC).SetStyle(check_style).SetSizeMin(0, DPI(20));
    show_hidden_.SetVisual(UICHECKVIS_CLASSIC).SetStyle(check_style).SetSizeMin(0, DPI(20));
    show_path_.SetVisual(UICHECKVIS_CLASSIC).SetStyle(check_style).SetSizeMin(0, DPI(20));
    show_size_.SetVisual(UICHECKVIS_CLASSIC).SetStyle(check_style).SetSizeMin(0, DPI(20));
    show_date_.SetVisual(UICHECKVIS_CLASSIC).SetStyle(check_style).SetSizeMin(0, DPI(20));
    show_extension_.SetVisual(UICHECKVIS_CLASSIC).SetStyle(check_style).SetSizeMin(0, DPI(20));
    transfer_preserve_tree_.SetVisual(UICHECKVIS_CLASSIC).SetStyle(check_style).SetSizeMin(0, DPI(20));
    transfer_flatten_.SetVisual(UICHECKVIS_CLASSIC).SetStyle(check_style).SetSizeMin(0, DPI(20));
    transfer_verify_hash_.SetVisual(UICHECKVIS_CLASSIC).SetStyle(check_style).SetSizeMin(0, DPI(20));

    nav_setup_button_.SetStyle(MakeNavButtonStyle(BlueDark(), BlueText(), true));
    nav_rename_button_.SetStyle(MakeNavButtonStyle(GreenDark(), GreenText(), false));
    nav_transfer_button_.SetStyle(MakeNavButtonStyle(AmberDark(), AmberText(), false));
    source_browse_.SetStyle(MakeSmallButtonStyle());
    rename_add_button_.SetStyle(MakeSmallButtonStyle());
    rename_remove_button_.SetStyle(MakeSmallButtonStyle());
    transfer_browse_.SetStyle(MakeSmallButtonStyle());
    {
        UiToolButton::Style s = MakeSmallButtonStyle();
        for(int i = 0; i < 4; i++) {
            s.palette.face[i] = UiFill::None();
            s.palette.frame[i] = Null;
            s.palette.ink[i] = Color(156, 163, 175);
            s.palette.icon[i] = Color(156, 163, 175);
        }
        s.metrics.face_enabled = false;
        s.metrics.frame_enabled = false;
        s.metrics.focus_enabled = false;
        s.metrics.content_margin = Rect(0, 0, 0, 0);
        output_copy_button_.SetStyle(s);
    }
    generate_button_.SetStyle(MakeActionStyle(true));
    abort_button_.SetStyle(MakeActionStyle(false));
    help_button_.SetStyle(MakeActionStyle(false));
    rename_save_button_.SetStyle(MakeActionStyle(false));
    rename_apply_button_.SetStyle(MakeActionStyle(true));
    transfer_apply_button_.SetStyle(MakeActionStyle(true));
    exit_button_.SetStyle(MakeActionStyle(true));
    {
        UiButton::Style s = exit_button_.GetStyle();
        for(int i = 0; i < 4; i++) {
            s.palette.face[i] = UiFill::Solid(Color(0xdc, 0x26, 0x26));
            s.palette.frame[i] = Color(0xdc, 0x26, 0x26);
            s.palette.ink[i] = White();
            s.palette.icon[i] = White();
        }
        s.palette.face[ST_HOT] = UiFill::Solid(Color(0xb9, 0x1c, 0x1c));
        s.palette.face[ST_PRESSED] = UiFill::Solid(Color(0x99, 0x1b, 0x1b));
        exit_button_.SetStyle(s);
    }

    UpdateFilterIndicator();

}

void MainWindow::Layout()
{
    // Manual layout keeps the single-window tool predictable and dependency-free.
    Rect r = GetSize();
    int m = DPI(8);
    int sidebar_w = DPI(366);

    sidebar_panel_.SetRect(m, m, sidebar_w, r.GetHeight() - 2 * m);
    main_panel_.SetRect(sidebar_panel_.GetRect().right + DPI(1), m, r.GetWidth() - sidebar_w - m * 2 - DPI(1), r.GetHeight() - 2 * m);

    Size ss = sidebar_panel_.GetSize();
    int x = DPI(4);
    int y = DPI(10);
    int full_w = ss.cx - DPI(16);

    title_card_.SetRect(x, y, full_w - DPI(68), DPI(34));
    version_badge_.SetRect(x + full_w - DPI(62), y + DPI(2), DPI(62), DPI(20));
    y += DPI(46);
    source_label_.SetRect(x, y, full_w, DPI(14));
    y += DPI(18);
    source_edit_.SetRect(x, y, full_w - DPI(30), DPI(28));
    source_browse_.SetRect(x + full_w - DPI(24), y, DPI(24), DPI(28));
    y += DPI(34);
    source_history_.SetRect(x, y, full_w, DPI(28));
    y += DPI(40);
    nav_panel_.SetRect(x, y, full_w, DPI(34));
    int nav_w = (full_w - DPI(8)) / 3;
    nav_setup_button_.SetRect(DPI(2), DPI(2), nav_w, DPI(30));
    nav_rename_button_.SetRect(DPI(4) + nav_w, DPI(2), nav_w, DPI(30));
    nav_transfer_button_.SetRect(DPI(6) + nav_w * 2, DPI(2), nav_w, DPI(30));
    scan_filter_badge_.SetRect(DPI(2) + nav_w - DPI(34), DPI(7), DPI(30), DPI(16));
    y += DPI(42);

    int page_h = ss.cy - y - DPI(12);
    setup_page_.SetRect(x, y, full_w, page_h);
    rename_page_.SetRect(x, y, full_w, page_h);
    transfer_page_.SetRect(x, y, full_w, page_h);

    LayoutSetupPage();
    LayoutRenamePage();
    LayoutTransferPage();

    Size ms = main_panel_.GetSize();
    int mx = DPI(14);
    int my = DPI(10);
    int content_w = ms.cx - mx * 2;
    int row1_left_w = DPI(122 + 10 + 82 + 26 + 128 + 8 + 132);
    int row1_right_w = DPI(52 + 10 + 118 + 10 + 118);
    bool wrap_top = content_w < row1_left_w + row1_right_w + DPI(16);

    int row1_y = my;
    int row2_y = wrap_top ? my + DPI(36) : my;

    generate_button_.SetRect(mx, row1_y, DPI(122), DPI(30));
    abort_button_.SetRect(mx + DPI(132), row1_y, DPI(82), DPI(30));
    output_format_.SetRect(mx + DPI(240), row1_y + DPI(1), DPI(128), DPI(28));
    slash_mode_.SetRect(mx + DPI(376), row1_y + DPI(1), DPI(132), DPI(28));

    int right_group_x = max(mx, ms.cx - mx - row1_right_w);
    state_label_.SetRect(right_group_x, row2_y + DPI(4), DPI(52), DPI(20));
    help_button_.SetRect(right_group_x + DPI(62), row2_y, DPI(118), DPI(30));
    exit_button_.SetRect(right_group_x + DPI(190), row2_y, DPI(118), DPI(30));

    int output_top = wrap_top ? my + DPI(78) : my + DPI(44);
    int footer_h = DPI(18);
    output_panel_.SetRect(mx, output_top, ms.cx - mx * 2, ms.cy - output_top - footer_h - DPI(14));
    output_scroll_panel_.SetRect(DPI(10), DPI(10), output_panel_.GetSize().cx - DPI(20), output_panel_.GetSize().cy - DPI(20));
    SyncOutputContentBounds();
    int footer_y = output_panel_.GetRect().bottom + DPI(8);
    output_copy_button_.SetRect(ms.cx - mx - DPI(112), footer_y - DPI(1), DPI(18), DPI(18));
    output_copy_label_.SetRect(ms.cx - mx - DPI(90), footer_y - DPI(1), DPI(74), DPI(18));
    footer_meta_.SetRect(mx, footer_y, DPI(1), footer_h);
    footer_path_.SetRect(ms.cx - mx - DPI(450), footer_y, DPI(320), footer_h);
}

void MainWindow::LayoutSetupPage()
{
    int m = DPI(8);
    int y = m;
    int w = max(DPI(280), setup_page_.GetSize().cx - DPI(28));

    setup_file_pattern_label_.SetRect(m, y, w, DPI(14));
    y += DPI(18);
    setup_filter_hint_.SetRect(m, y, w, DPI(24));
    y += DPI(26);
    int mode_label_w = DPI(84);
    file_pattern_mode_label_.SetRect(m, y + DPI(3), mode_label_w, DPI(14));
    file_pattern_mode_.SetRect(m + mode_label_w + DPI(4), y - DPI(2), DPI(96), DPI(28));
    file_case_sensitive_.SetRect(m + w - DPI(118), y + DPI(2), DPI(118), DPI(18));
    y += DPI(34);
    setup_file_pattern_.SetRect(m, y, w, DPI(28));
    y += DPI(34);
    dir_pattern_mode_label_.SetRect(m, y + DPI(3), mode_label_w, DPI(14));
    dir_pattern_mode_.SetRect(m + mode_label_w + DPI(4), y - DPI(2), DPI(96), DPI(28));
    dir_case_sensitive_.SetRect(m + w - DPI(118), y + DPI(2), DPI(118), DPI(18));
    y += DPI(34);
    setup_dir_pattern_.SetRect(m, y, w, DPI(28));
    y += DPI(38);

    size_threshold_label_.SetRect(m, y, DPI(140), DPI(14));
    size_filter_toggle_.SetRect(m + w - DPI(70), y - DPI(2), DPI(70), DPI(18));
    y += DPI(20);
    int third = (w - DPI(8) * 2) / 3;
    size_min_.SetRect(m, y, third, DPI(24));
    size_max_.SetRect(m + third + DPI(8), y, third, DPI(24));
    size_unit_.SetRect(m + (third + DPI(8)) * 2, y - DPI(2), third, DPI(28));
    y += DPI(34);

    date_range_label_.SetRect(m, y, DPI(120), DPI(14));
    date_filter_toggle_.SetRect(m + w - DPI(70), y - DPI(2), DPI(70), DPI(18));
    y += DPI(20);
    int half = (w - DPI(8)) / 2;
    date_from_.SetRect(m, y, half, DPI(24));
    date_to_.SetRect(m + half + DPI(8), y, half, DPI(24));
    y += DPI(36);

    sort_label_.SetRect(m, y, w, DPI(14));
    y += DPI(18);
    half = (w - DPI(8)) / 2;
    sort_primary_.SetRect(m, y - DPI(2), half, DPI(28));
    sort_secondary_.SetRect(m + half + DPI(8), y - DPI(2), half, DPI(28));
    y += DPI(34);

    int left_col = (w - DPI(8)) / 2;
    int right_col = w - left_col - DPI(8);
    recursive_scan_.SetRect(m, y, DPI(128), DPI(18));
    depth_label_.SetRect(m + DPI(132), y + DPI(1), DPI(34), DPI(14));
    depth_limit_.SetRect(m + DPI(166), y - DPI(2), DPI(42), DPI(24));
    y += DPI(24);
    include_dirs_.SetRect(m, y, left_col, DPI(18));
    include_files_.SetRect(m + left_col + DPI(8), y, right_col, DPI(18));
    y += DPI(24);
    show_hidden_.SetRect(m, y, left_col, DPI(18));
    reverse_sort_.SetRect(m + left_col + DPI(8), y, right_col, DPI(18));
    y += DPI(28);
    dir_placement_.SetRect(m, y - DPI(2), w, DPI(28));
    y += DPI(38);

    display_label_.SetRect(m, y, w, DPI(14));
    y += DPI(18);
    int cell = (w - DPI(12)) / 4;
    show_path_.SetRect(m, y, cell, DPI(18));
    show_size_.SetRect(m + cell + DPI(4), y, cell, DPI(18));
    show_date_.SetRect(m + (cell + DPI(4)) * 2, y, cell, DPI(18));
    show_extension_.SetRect(m + (cell + DPI(4)) * 3, y, cell, DPI(18));
}

void MainWindow::LayoutRenamePage()
{
    int m = DPI(8);
    int y = m;
    int w = max(DPI(280), rename_page_.GetSize().cx - DPI(28));

    rename_operator_label_.SetRect(m, y, w, DPI(14));
    y += DPI(18);
    rename_filter_hint_.SetRect(m, y, w, DPI(28));
    y += DPI(30);
    rename_add_type_.SetRect(m, y - DPI(2), w, DPI(28));
    y += DPI(36);
    rename_params_label_.SetRect(m, y, w, DPI(14));
    y += DPI(18);
    rename_mode_.SetRect(m, y - DPI(2), w, DPI(28));
    y += DPI(34);
    rename_param_a_.SetRect(m, y, w, DPI(28));
    y += DPI(34);
    rename_param_b_.SetRect(m, y, w, DPI(28));
    y += DPI(34);
    rename_param_c_.SetRect(m, y, w, DPI(28));
    y += DPI(34);
    int bw = (w - DPI(24)) / 4;
    rename_save_button_.SetRect(m, y, bw, DPI(28));
    rename_add_button_.SetRect(m + bw + DPI(8), y, bw, DPI(28));
    rename_remove_button_.SetRect(m + 2 * (bw + DPI(8)), y, bw, DPI(28));
    rename_apply_button_.SetRect(m + 3 * (bw + DPI(8)), y, bw, DPI(28));
    y += DPI(40);
    rename_steps_label_.SetRect(m, y, w, DPI(14));
    y += DPI(18);
    rename_stack_panel_.SetRect(m, y, w, DPI(108));
    rename_stack_.SetRect(DPI(1), DPI(1), rename_stack_panel_.GetSize().cx - DPI(2), rename_stack_panel_.GetSize().cy - DPI(2));
    y += DPI(116);
    rename_preview_input_.SetRect(m, y, w, DPI(28));
    y += DPI(36);
    rename_preview_label_.SetRect(m, y, w - DPI(48), DPI(14));
    rename_preview_count_.SetRect(m + w - DPI(42), y - DPI(4), DPI(42), DPI(22));
    y += DPI(18);
    rename_preview_panel_.SetRect(m, y, w, DPI(150));
    rename_preview_view_.SetRect(DPI(1), DPI(1), rename_preview_panel_.GetSize().cx - DPI(2), rename_preview_panel_.GetSize().cy - DPI(2));
}

void MainWindow::LayoutTransferPage()
{
    int m = DPI(8);
    int y = m;
    int w = max(DPI(280), transfer_page_.GetSize().cx - DPI(28));

    transfer_target_label_.SetRect(m, y, w, DPI(14));
    y += DPI(18);
    transfer_target_.SetRect(m, y, w - DPI(30), DPI(28));
    transfer_browse_.SetRect(m + w - DPI(24), y, DPI(24), DPI(28));
    y += DPI(36);
    transfer_filter_hint_.SetRect(m, y, w, DPI(28));
    y += DPI(28);
    transfer_preserve_tree_.SetRect(m, y, w, DPI(18));
    y += DPI(22);
    transfer_flatten_.SetRect(m, y, w, DPI(18));
    y += DPI(30);
    transfer_conflict_.SetRect(m, y - DPI(2), w, DPI(28));
    y += DPI(36);
    transfer_verify_hash_.SetRect(m, y, w, DPI(18));
    y += DPI(26);
    transfer_apply_button_.SetRect(m, y, w, DPI(28));
}

void MainWindow::Paint(Draw& w)
{
    w.DrawRect(GetSize(), BodyBg());
}

void MainWindow::SetSidebarPage(int page)
{
    active_page_ = minmax(page, 0, 2);
    setup_page_.Show(active_page_ == 0);
    rename_page_.Show(active_page_ == 1);
    transfer_page_.Show(active_page_ == 2);
    nav_setup_button_.SetStyle(MakeNavButtonStyle(BlueDark(), BlueText(), active_page_ == 0));
    nav_rename_button_.SetStyle(MakeNavButtonStyle(GreenDark(), GreenText(), active_page_ == 1));
    nav_transfer_button_.SetStyle(MakeNavButtonStyle(AmberDark(), AmberText(), active_page_ == 2));
}

void MainWindow::HandleBrowseSource()
{
    FileSel fs;
    fs.PreSelect(source_edit_.GetData().ToString());
    if(fs.ExecuteSelectDir("Select source directory")) {
        source_edit_.SetData(NormalizePathSlashes(fs.Get(), slash_mode_.GetSelectedData() == (int)SlashMode::Unix));
        UpdateFooterPath();
        RefreshRenamePreview();
    }
}

void MainWindow::HandleBrowseTransferTarget()
{
    FileSel fs;
    fs.PreSelect(transfer_target_.GetData().ToString());
    if(fs.ExecuteSelectDir("Select transfer target"))
        transfer_target_.SetData(NormalizePathSlashes(fs.Get(), slash_mode_.GetSelectedData() == (int)SlashMode::Unix));
}

void MainWindow::HandleSlashModeChanged()
{
    bool linux_slashes = slash_mode_.GetSelectedData() == (int)SlashMode::Unix;
    source_edit_.SetData(NormalizePathSlashes(source_edit_.GetData().ToString(), linux_slashes));
    if(!transfer_target_.GetData().ToString().IsEmpty())
        transfer_target_.SetData(NormalizePathSlashes(transfer_target_.GetData().ToString(), linux_slashes));
    UpdateFooterPath();
}

DirectoryScanSettings MainWindow::ReadSettings() const
{
    DirectoryScanSettings s;
    s.source_directory = source_edit_.GetData().ToString();
    s.file_patterns = setup_file_pattern_.GetData().ToString();
    s.directory_patterns = setup_dir_pattern_.GetData().ToString();
    s.file_case_sensitive = file_case_sensitive_.IsChecked();
    s.dir_case_sensitive = dir_case_sensitive_.IsChecked();
    s.file_pattern_mode = (PatternMode)(int)file_pattern_mode_.GetSelectedData();
    s.dir_pattern_mode = (PatternMode)(int)dir_pattern_mode_.GetSelectedData();
    s.recursive = recursive_scan_.IsChecked();
    s.recursive_depth = max(0, (int)depth_limit_.GetData());
    s.include_directories = include_dirs_.IsChecked();
    s.include_files = include_files_.IsChecked();
    s.show_hidden = show_hidden_.IsChecked();
    s.reverse_sort = reverse_sort_.IsChecked();
    s.enable_size_filter = size_filter_toggle_.IsChecked();
    s.min_size = (int)size_min_.GetData();
    s.max_size = (int)size_max_.GetData();
    s.size_unit = (SizeUnit)(int)size_unit_.GetSelectedData();
    s.enable_date_filter = date_filter_toggle_.IsChecked();
    s.modified_from = date_from_.GetData();
    s.modified_to = date_to_.GetData();
    s.sort_key = (DirSortKey)(int)sort_primary_.GetSelectedData();
    s.secondary_sort_key = (DirSortKey)(int)sort_secondary_.GetSelectedData();
    s.dir_placement = (DirPlacement)(int)dir_placement_.GetSelectedData();
    s.slash_mode = (SlashMode)(int)slash_mode_.GetSelectedData();
    s.output_format = (OutputFormat)(int)output_format_.GetSelectedData();
    s.show_path = show_path_.IsChecked();
    s.show_size = show_size_.IsChecked();
    s.show_date = show_date_.IsChecked();
    s.show_extension = show_extension_.IsChecked();
    return s;
}

void MainWindow::HandleGenerate()
{
    UpdateStatus("RUNNING", true);
    DirectoryScanSettings settings = ReadSettings();
    if(settings.output_format == OutputFormat::Text) {
        Vector<DirectoryOutputLine> lines = DirectoryEngine::GenerateTextLines(settings);
        output_edit_.ClearSpans();
        output_edit_.EnableRich().SetSelectable().SetAlign(UiAlign::LEFT, UiAlign::TOP);
        for(int i = 0; i < lines.GetCount(); i++) {
            output_edit_.AddTextSpan(lines[i].text, lines[i].is_dir ? AmberText() : OutputText());
            if(i + 1 < lines.GetCount())
                output_edit_.AddNewlineSpan();
        }
    }
    else {
        output_edit_.SetAlign(UiAlign::LEFT, UiAlign::TOP);
        output_edit_.SetText(DirectoryEngine::Generate(settings));
    }
    SyncOutputContentBounds();
    RefreshRenamePreview();
    UpdateStatus("READY", false);
}

void MainWindow::HandleAbort()
{
    UpdateStatus("IDLE", false);
    PromptOK("Abort is wired for the next threaded scan phase. The current first pass runs synchronously.");
}

void MainWindow::HandleHelp()
{
    HelpDialog dlg;
    dlg.ApplyTheme();
    dlg.Run();
}

void MainWindow::UpdateStatus(const String& text, bool busy)
{
    state_label_.SetText(text.IsEmpty() ? (busy ? "BUSY" : "IDLE") : text);
}

void MainWindow::UpdateFooterPath()
{
    footer_path_.SetText(source_edit_.GetData().ToString());
}

void MainWindow::SyncOutputContentBounds()
{
    Size viewport = output_scroll_panel_.GetSize();
    Size need = output_edit_.GetContentSize();
    output_edit_.SetRect(0, 0, max(viewport.cx, need.cx), max(viewport.cy, need.cy));
    output_scroll_panel_.RefreshLayout();
    output_scroll_panel_.Layout();
}

void MainWindow::SetOutputReport(const String& text)
{
    output_edit_.SetText(text);
    SyncOutputContentBounds();
}

bool MainWindow::HasActiveScanFilter() const
{
    return !TrimBoth(setup_file_pattern_.GetData().ToString()).IsEmpty()
        || !TrimBoth(setup_dir_pattern_.GetData().ToString()).IsEmpty()
        || file_case_sensitive_.IsChecked()
        || dir_case_sensitive_.IsChecked()
        || (int)file_pattern_mode_.GetSelectedData() != (int)PatternMode::Glob
        || (int)dir_pattern_mode_.GetSelectedData() != (int)PatternMode::Glob
        || size_filter_toggle_.IsChecked()
        || date_filter_toggle_.IsChecked()
        || show_hidden_.IsChecked()
        || !recursive_scan_.IsChecked()
        || (int)depth_limit_.GetData() != 2
        || !include_dirs_.IsChecked()
        || !include_files_.IsChecked();
}

void MainWindow::UpdateFilterIndicator()
{
    scan_filter_badge_.Show(HasActiveScanFilter());
}

void MainWindow::HandleApplyRename()
{
    DirectoryScanSettings settings = ReadSettings();
    String dir = settings.source_directory;
    if(dir.IsEmpty() || !DirectoryExists(dir)) {
        PromptOK("Select a valid source directory first.");
        return;
    }

    Vector<FilePlanItem> items;
    Index<String> existing_names;
    CollectRenameEntries(items,
                         dir,
                         dir,
                         0,
                         settings,
                         SplitPatternsText(settings.file_patterns, settings.file_case_sensitive),
                         SplitPatternsText(settings.directory_patterns, settings.dir_case_sensitive));
    for(const FilePlanItem& item : items)
        existing_names.FindAdd(item.source_name);

    if(items.IsEmpty()) {
        PromptOK("No eligible files or directories found in the source directory.");
        return;
    }

    Vector<String> names;
    for(const FilePlanItem& item : items)
        names.Add(item.source_name);
    Vector<String> preview = RenameEngine::Preview(names, rename_steps_, existing_names);
    int changed = 0;
    for(int i = 0; i < items.GetCount(); i++) {
        items[i].target_name = preview[i];
        items[i].target_path = AppendFileName(GetFileFolder(items[i].source_path), preview[i]);
        if(items[i].source_name != items[i].target_name)
            changed++;
    }

    if(changed == 0) {
        PromptOK("Rename stack does not change any eligible entries.");
        return;
    }
    if(!PromptYesNo(Format("Apply rename to %d entries in the source directory?", changed)))
        return;

    String report;
    report << "Rename apply\n\n";
    Vector<FilePlanItem> changed_items;
    for(const FilePlanItem& item : items)
        if(item.source_name != item.target_name)
            changed_items.Add(item);

    // Two-phase rename avoids collisions on swaps/cycles.
    Vector<String> temp_paths;
    bool ok = true;
    for(int i = 0; i < changed_items.GetCount(); i++) {
        String tmp = AppendFileName(dir, Format(".__dirlister_tmp_%lld_%d", (int64)GetTickCount(), i));
        temp_paths.Add(tmp);
        if(!FileMove(changed_items[i].source_path, tmp)) {
            report << "FAILED TEMP: " << changed_items[i].source_name << "\n";
            ok = false;
            break;
        }
    }
    if(ok) {
        for(int i = 0; i < changed_items.GetCount(); i++) {
            if(!FileMove(temp_paths[i], changed_items[i].target_path)) {
                report << "FAILED APPLY: " << changed_items[i].source_name << " -> " << changed_items[i].target_name << "\n";
                ok = false;
            }
            else
                report << changed_items[i].source_name << " -> " << changed_items[i].target_name << "\n";
        }
    }
    if(!ok)
        report << "\nRename completed with errors.\n";
    else
        report << "\nRename completed successfully.\n";

    SetOutputReport(report);
    RefreshRenamePreview();
    UpdateStatus(ok ? "READY" : "ERROR", false);
}

void MainWindow::HandleApplyTransfer()
{
    DirectoryScanSettings settings = ReadSettings();
    String source = settings.source_directory;
    String target_root = transfer_target_.GetData().ToString();
    if(source.IsEmpty() || !DirectoryExists(source)) {
        PromptOK("Select a valid source directory first.");
        return;
    }
    if(target_root.IsEmpty()) {
        PromptOK("Select a target directory for transfer first.");
        return;
    }
    if(!DirectoryExists(target_root) && !RealizeDirectory(target_root)) {
        PromptOK("Failed to create or access the target directory.");
        return;
    }

    Vector<FilePlanItem> entries;
    CollectTransferEntries(entries,
                           source,
                           source,
                           0,
                           settings,
                           SplitPatternsText(settings.file_patterns, settings.file_case_sensitive),
                           SplitPatternsText(settings.directory_patterns, settings.dir_case_sensitive));
    if(entries.IsEmpty()) {
        PromptOK("No eligible entries found to transfer.");
        return;
    }

    int conflict_mode = (int)transfer_conflict_.GetSelectedData();
    String summary = Format("Transfer %d entries to:\n%s", entries.GetCount(), target_root);
    if(!PromptYesNo(summary))
        return;

    String report;
    report << "Transfer apply\n\n";
    int copied = 0;
    int skipped = 0;
    int failed = 0;

    for(const FilePlanItem& entry : entries) {
        String relative = transfer_preserve_tree_.IsChecked() && !transfer_flatten_.IsChecked()
                        ? entry.relative_path
                        : entry.source_name;
        String dest = AppendFileName(target_root, relative);

        if(entry.is_dir) {
            if(transfer_preserve_tree_.IsChecked()) {
                if(RealizeDirectory(dest))
                    report << "DIR  " << relative << "\n";
                else {
                    report << "FAIL DIR  " << relative << "\n";
                    failed++;
                }
            }
            continue;
        }

        RealizeDirectory(GetFileFolder(dest));
        if(FileExists(dest)) {
            if(conflict_mode == 2) {
                report << "SKIP " << relative << " [exists]\n";
                skipped++;
                continue;
            }
            if(conflict_mode == 0)
                dest = UniqueTargetPath(dest);
            else if(conflict_mode == 1) {
                if(!FileDelete(dest)) {
                    report << "FAIL " << relative << " [failed to remove existing]\n";
                    failed++;
                    continue;
                }
            }
        }

        if(CopyFileVerified(entry.source_path, dest, transfer_verify_hash_.IsChecked())) {
            report << "COPY " << relative << " -> " << MakeRelativePath(target_root, dest) << "\n";
            copied++;
        }
        else {
            report << "FAIL " << relative << "\n";
            failed++;
        }
    }

    report << Format("\nSummary: copied=%d skipped=%d failed=%d\n", copied, skipped, failed);
    SetOutputReport(report);
    UpdateStatus(failed ? "ERROR" : "READY", false);
}

void MainWindow::ResetRenameModel()
{
    rename_steps_.Clear();
    rename_steps_.AddPick(RenameEngine::MakeDefaultStep(RenameStepType::FindReplace));
    rename_edit_step_ = RenameEngine::MakeDefaultStep(RenameStepType::FindReplace);
    rename_selected_step_ = 0;
    rename_dirty_ = false;
    RefreshRenameUi();
}

Vector<String> MainWindow::CollectRenameSamples(Index<String>& existing_names) const
{
    Vector<String> names;
    DirectoryScanSettings settings = ReadSettings();
    String dir = settings.source_directory;
    if(dir.IsEmpty() || !DirectoryExists(dir))
        return names;

    int limit = max(1, (int)rename_preview_count_.GetData());

    Vector<FilePlanItem> items;
    CollectRenameEntries(items,
                         dir,
                         dir,
                         0,
                         settings,
                         SplitPatternsText(settings.file_patterns, settings.file_case_sensitive),
                         SplitPatternsText(settings.directory_patterns, settings.dir_case_sensitive));
    for(int i = 0; i < items.GetCount() && names.GetCount() < limit; i++) {
        existing_names.FindAdd(items[i].source_name);
        names.Add(items[i].source_name);
    }
    return names;
}

void MainWindow::RefreshRenamePreview()
{
    // The preview uses live directory samples so users can validate the rename stack before exporting.
    Index<String> existing_names;
    Vector<String> sample_names = CollectRenameSamples(existing_names);
    if(sample_names.IsEmpty()) {
        rename_preview_view_.SetData("Select a valid source directory to preview rename results.\n");
        return;
    }

    String text;
    const Vector<RenameStep>& stack = rename_steps_;
    if(!rename_preview_input_.GetData().ToString().IsEmpty()) {
        Vector<String> single;
        single.Add(rename_preview_input_.GetData().ToString());
        Index<String> single_names;
        single_names.FindAdd(single[0]);
        Vector<String> out = RenameEngine::Preview(single, stack, single_names);
        if(!out.IsEmpty())
            text << single[0] << "  ->  " << out[0] << "\n\n";
    }

    Vector<String> preview = RenameEngine::Preview(sample_names, stack, existing_names);
    for(int i = 0; i < sample_names.GetCount(); i++)
        text << sample_names[i] << "  ->  " << preview[i] << "\n";
    rename_preview_view_.SetData(text);
}

void MainWindow::RefreshRenameUi()
{
    rename_ui_syncing_ = true;

    rename_stack_model_.Clear();
    for(const RenameStep& step : rename_steps_)
        rename_stack_model_.Add(RenameEngine::GetStepTitle(step.type), step.id);

    RenameStepType type = rename_edit_step_.id.IsEmpty() ? RenameStepType::FindReplace : rename_edit_step_.type;
    rename_add_type_.SelectByData((int)type);

    rename_mode_.Hide();
    rename_param_a_.Hide();
    rename_param_b_.Hide();
    rename_param_c_.Hide();

    switch(type) {
    case RenameStepType::Case:
        rename_mode_.Show();
        rename_mode_.Clear();
        rename_mode_.Add("lower", 0).Add("upper", 1).Add("camel", 2).Add("title", 3);
        if(GetStepData(rename_edit_step_, "mode", "lower") == "upper") rename_mode_.Select(1);
        else if(GetStepData(rename_edit_step_, "mode", "lower") == "camel") rename_mode_.Select(2);
        else if(GetStepData(rename_edit_step_, "mode", "lower") == "title") rename_mode_.Select(3);
        else rename_mode_.Select(0);
        break;
    case RenameStepType::Alnum:
        rename_mode_.Show();
        rename_mode_.Clear();
        rename_mode_.Add("enabled", 0).Add("disabled", 1);
        rename_mode_.Select(GetStepData(rename_edit_step_, "enabled", "true") == "false" ? 1 : 0);
        break;
    case RenameStepType::Numbering:
        rename_param_a_.Show();
        rename_param_b_.Show();
        rename_param_c_.Show();
        rename_param_a_.SetPlaceholder("Find token");
        rename_param_b_.SetPlaceholder("Pattern, e.g. ####");
        rename_param_c_.SetPlaceholder("Start");
        rename_param_a_.SetData(GetStepData(rename_edit_step_, "find"));
        rename_param_b_.SetData(GetStepData(rename_edit_step_, "pattern", "####"));
        rename_param_c_.SetData(GetStepData(rename_edit_step_, "start", "1"));
        break;
    case RenameStepType::Prefix:
        rename_param_a_.Show();
        rename_param_a_.SetPlaceholder("Prefix text");
        rename_param_a_.SetData(GetStepData(rename_edit_step_, "prefix"));
        break;
    case RenameStepType::Extension:
        rename_param_a_.Show();
        rename_param_a_.SetPlaceholder("New extension, e.g. png");
        rename_param_a_.SetData(GetStepData(rename_edit_step_, "ext", "txt"));
        break;
    case RenameStepType::InsertLeft:
    case RenameStepType::InsertRight:
        rename_param_a_.Show();
        rename_param_b_.Show();
        rename_param_a_.SetPlaceholder("Index");
        rename_param_b_.SetPlaceholder("Text to insert");
        rename_param_a_.SetData(GetStepData(rename_edit_step_, "index", "0"));
        rename_param_b_.SetData(GetStepData(rename_edit_step_, "text"));
        break;
    case RenameStepType::FindReplace:
    default:
        rename_param_a_.Show();
        rename_param_b_.Show();
        rename_param_a_.SetPlaceholder("Find");
        rename_param_b_.SetPlaceholder("Replace");
        rename_param_a_.SetData(GetStepData(rename_edit_step_, "find"));
        rename_param_b_.SetData(GetStepData(rename_edit_step_, "replace"));
        break;
    }

    if(rename_selected_step_ >= 0 && rename_selected_step_ < rename_steps_.GetCount())
        rename_stack_.SetCursor(rename_selected_step_).Select(rename_selected_step_);
    rename_ui_syncing_ = false;
    SyncRenameSaveState();
    RefreshRenamePreview();
}

void MainWindow::HandleRenameAdd()
{
    HandleRenameFieldsChanged();
    RenameStep step = CopyStep(rename_edit_step_);
    if(step.id.IsEmpty())
        step = RenameEngine::MakeDefaultStep(step.type);
    rename_steps_.AddPick(pick(step));
    rename_selected_step_ = rename_steps_.GetCount() - 1;
    rename_dirty_ = false;
    RefreshRenameUi();
}

void MainWindow::HandleRenameRemove()
{
    if(rename_selected_step_ < 0 || rename_selected_step_ >= rename_steps_.GetCount())
        return;
    rename_steps_.Remove(rename_selected_step_);
    rename_selected_step_ = min(rename_selected_step_, rename_steps_.GetCount() - 1);
    if(rename_selected_step_ < 0)
        rename_edit_step_ = RenameEngine::MakeDefaultStep(SelectedRenameType(rename_add_type_));
    rename_dirty_ = false;
    RefreshRenameUi();
}

void MainWindow::HandleRenameMove(int delta)
{
    int from = rename_selected_step_;
    int to = from + delta;
    if(from < 0 || from >= rename_steps_.GetCount() || to < 0 || to >= rename_steps_.GetCount())
        return;
    Swap(rename_steps_[from], rename_steps_[to]);
    rename_selected_step_ = to;
    RefreshRenameUi();
}

void MainWindow::HandleRenameSelection()
{
    if(rename_ui_syncing_)
        return;
    rename_selected_step_ = rename_stack_.GetCursor();
    if(rename_selected_step_ >= 0 && rename_selected_step_ < rename_steps_.GetCount()) {
        rename_edit_step_ = CopyStep(rename_steps_[rename_selected_step_]);
        rename_dirty_ = false;
    }
    RefreshRenameUi();
}

void MainWindow::HandleRenameFieldsChanged()
{
    if(rename_ui_syncing_)
        return;

    rename_edit_step_.type = SelectedRenameType(rename_add_type_);
    if(rename_edit_step_.id.IsEmpty())
        rename_edit_step_.id = RenameEngine::MakeDefaultStep(rename_edit_step_.type).id;

    switch(rename_edit_step_.type) {
    case RenameStepType::Case: {
        String mode = "lower";
        int index = rename_mode_.GetSelection();
        if(index == 1) mode = "upper";
        else if(index == 2) mode = "camel";
        else if(index == 3) mode = "title";
        SetStepData(rename_edit_step_, "mode", mode);
        break;
    }
    case RenameStepType::Alnum:
        SetStepData(rename_edit_step_, "enabled", rename_mode_.GetSelection() == 1 ? "false" : "true");
        break;
    case RenameStepType::Numbering:
        SetStepData(rename_edit_step_, "find", rename_param_a_.GetData().ToString());
        SetStepData(rename_edit_step_, "pattern", rename_param_b_.GetData().ToString());
        SetStepData(rename_edit_step_, "start", rename_param_c_.GetData().ToString());
        break;
    case RenameStepType::Prefix:
        SetStepData(rename_edit_step_, "prefix", rename_param_a_.GetData().ToString());
        break;
    case RenameStepType::Extension:
        SetStepData(rename_edit_step_, "ext", rename_param_a_.GetData().ToString());
        break;
    case RenameStepType::InsertLeft:
    case RenameStepType::InsertRight:
        SetStepData(rename_edit_step_, "index", rename_param_a_.GetData().ToString());
        SetStepData(rename_edit_step_, "text", rename_param_b_.GetData().ToString());
        break;
    case RenameStepType::FindReplace:
    default:
        SetStepData(rename_edit_step_, "find", rename_param_a_.GetData().ToString());
        SetStepData(rename_edit_step_, "replace", rename_param_b_.GetData().ToString());
        break;
    }
    rename_dirty_ = true;
    SyncRenameSaveState();
    RefreshRenamePreview();
}

void MainWindow::SyncRenameSaveState()
{
    rename_save_button_.Enable(rename_selected_step_ >= 0 && rename_dirty_);
}

}
