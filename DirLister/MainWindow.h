// DirLister - main desktop window for scanning, previewing, and exporting listings.
// Change log:
// - 2026-04-22: Added class/function comments and grouped members for readability.

#ifndef _DirLister_MainWindow_h_
#define _DirLister_MainWindow_h_

#include <CtrlLib/CtrlLib.h>
#include <Ui/Ui.h>

#include "DirectoryEngine.h"
#include "DirListerTheme.h"
#include "RenameEngine.h"

namespace Upp {

// Owns the full DirLister UI: scan setup, rename preview, transfer settings, and output view.
class MainWindow : public TopWindow {
public:
    typedef MainWindow CLASSNAME;

    // Builds the window, applies the shared theme, and initializes the default rename stack.
    MainWindow();

    virtual void Layout() override;
    virtual void Paint(Draw& w) override;

private:
    // Constructs controls, binds events, and seeds default widget values.
    void BuildUi();
    // Applies the centralized visual theme to all controls.
    void ApplyTheme();
    // Creates and initializes the three left-sidebar pages.
    void AddSidebarPages();

    // Manual page layout helpers for the three sidebar sections.
    void LayoutSetupPage();
    void LayoutRenamePage();
    void LayoutTransferPage();

    // File-system pickers and generate/abort actions.
    void HandleBrowseSource();
    void HandleBrowseTransferTarget();
    void HandleGenerate();
    void HandleAbort();

    // Small UI synchronization helpers.
    void UpdateStatus(const String& text, bool busy = false);
    void UpdateFooterPath();
    void HandleSlashModeChanged();
    void SetSidebarPage(int page);

    // Reads the current form state into the scan engine contract.
    DirectoryScanSettings ReadSettings() const;

    // Rename-stack model and preview synchronization.
    void ResetRenameModel();
    void RefreshRenameUi();
    void RefreshRenamePreview();
    void HandleRenameAdd();
    void HandleRenameRemove();
    void HandleRenameMove(int delta);
    void HandleRenameSelection();
    void HandleRenameFieldsChanged();
    Vector<String> CollectRenameSamples(Index<String>& existing_names) const;
    void SyncRenameSaveState();

private:
    // Window shell and persistent navigation controls.
    UiPanel sidebar_panel_;
    UiPanel main_panel_;
    UiPanel output_panel_;
    UiTitleCard title_card_;
    UiLabel version_badge_;
    UiLabel source_label_;
    UiLabel source_subtitle_;
    UiLineEdit source_edit_;
    UiToolButton source_browse_;
    UiDropdown source_history_;
    UiPanel nav_panel_;
    UiButton nav_setup_button_;
    UiButton nav_rename_button_;
    UiButton nav_transfer_button_;
    int active_page_ = 0;

    // Page containers hosted inside the left sidebar.
    ParentCtrl setup_page_;
    ParentCtrl rename_page_;
    ParentCtrl transfer_page_;

    // Setup page controls.
    UiLabel setup_file_pattern_label_;
    UiLineEdit setup_file_pattern_;
    UiLineEdit setup_dir_pattern_;
    UiLabel size_threshold_label_;
    UiCheckBox size_filter_toggle_;
    EditInt size_min_;
    EditInt size_max_;
    UiDropdown size_unit_;
    UiLabel date_range_label_;
    UiCheckBox date_filter_toggle_;
    DropDate date_from_;
    DropDate date_to_;
    UiLabel sort_label_;
    UiDropdown sort_primary_;
    UiDropdown sort_secondary_;
    UiDropdown dir_placement_;
    UiCheckBox reverse_sort_;
    UiCheckBox recursive_scan_;
    UiLabel depth_label_;
    EditInt depth_limit_;
    UiCheckBox include_dirs_;
    UiCheckBox include_files_;
    UiCheckBox show_hidden_;
    UiLabel display_label_;
    UiCheckBox show_path_;
    UiCheckBox show_size_;
    UiCheckBox show_date_;
    UiCheckBox show_extension_;
    UiLabel format_label_;
    UiDropdown output_format_;
    UiDropdown slash_mode_;

    // Rename page controls and backing model state.
    UiLabel rename_operator_label_;
    UiDropdown rename_add_type_;
    UiButton rename_save_button_;
    UiToolButton rename_add_button_;
    UiToolButton rename_remove_button_;
    UiLabel rename_params_label_;
    UiDropdown rename_mode_;
    UiLineEdit rename_param_a_;
    UiLineEdit rename_param_b_;
    UiLineEdit rename_param_c_;
    UiLabel rename_steps_label_;
    UiPanel rename_stack_panel_;
    UiList rename_stack_;
    UiListModel rename_stack_model_;
    UiLineEdit rename_preview_input_;
    UiLabel rename_preview_label_;
    EditInt rename_preview_count_;
    UiPanel rename_preview_panel_;
    DocEdit rename_preview_view_;
    Vector<RenameStep> rename_steps_;
    RenameStep rename_edit_step_;
    int rename_selected_step_ = -1;
    bool rename_ui_syncing_ = false;
    bool rename_dirty_ = false;

    // Transfer page controls.
    UiLabel transfer_target_label_;
    UiLineEdit transfer_target_;
    UiToolButton transfer_browse_;
    UiCheckBox transfer_preserve_tree_;
    UiCheckBox transfer_flatten_;
    UiDropdown transfer_conflict_;
    UiCheckBox transfer_verify_hash_;

    // Output panel and footer.
    UiButton generate_button_;
    UiButton abort_button_;
    UiButton exit_button_;
    UiLabel state_label_;
    DocEdit output_edit_;
    UiLabel footer_meta_;
    UiLabel footer_path_;
};

}

#endif
