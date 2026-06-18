# Changelog

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
