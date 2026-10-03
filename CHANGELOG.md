# Changelog

## v1.0.0 - 2026-10-04

- Added an inline filter process stack with Add, Save, Delete and drag reorder, matching the Rename workflow.
- Unified name match/exclude, contains/doesn't contain, first-N-per-parent, size and date ranges in that stack.
- Added tree text output with explicit ancestor context and sibling sorting.
- Disambiguated flat text with relative paths and honored the sorting toggle in text previews.
- Replaced the large output label with a read-only text editor that manages scrolling.
- Shared scan rules across listings, rename previews, and transfers; made scan settings scrollable.
- Added focused engine and native stack checks under `tests`. Generated test data and caches stay outside the repository.
- Reorganized source, documentation, design assets, tests and the latest Windows x64 binary.
- Added a scrolling Help guide with filter, rename and combined workflow recipes.
- Fixed Rename drag reordering to update the executed process order.
- Updated the README screenshot to `SnapShot_DirectoryLister.jpg`.

## v0.2.0 - 2026-06-14

- Updated app call sites for the current `upp_Ui` V1 custom style API.
- Reworked View, Sorting, and Filtering setup controls for clearer behavior.
- Added a grid-aligned View section for `Path`, `Ext`, `Date`, `Size`, `Dirs`, `Files`, and `Hidden`.
- Fixed extension output so it no longer adds a duplicate bracketed extension field.
- Updated README build instructions and verified local `umk` build output.

## 2026-04-22

- Added root project documentation for the current U++ DirLister rewrite.
- Added file headers and comments across the main source files to improve readability.
- Hardened CSV export by escaping quoted values correctly.
- Normalized scan settings before execution to clamp invalid sizes and swapped date ranges.
- Fixed the main window status label to display the requested state text.
