# DirLister

DirLister is a U++ desktop tool for scanning a directory, filtering and sorting the result, previewing rename rules, and exporting the final listing as text, CSV, or JSON.

![DirLister application snapshot](./DirLister_snapshot.jpg)

## Project Layout

- `DirLister/main.cpp`: GUI entry point.
- `DirLister/MainWindow.*`: window composition, layout, and event wiring.
- `DirLister/DirectoryEngine.*`: directory scan, filtering, sorting, and output rendering.
- `DirLister/RenameEngine.*`: rename-step defaults and preview pipeline.
- `DirLister/DirListerTheme.*`: shared visual styling helpers.

## Build Notes

The package file is `DirLister/DirLister.upp`.

Required U++ packages:

- `Core`
- `Draw`
- `CtrlCore`
- `CtrlLib`
- `Ui`

## Maintenance Notes

- Keep behavior in `MainWindow` and the engines, and keep appearance in `DirListerTheme`.
- Prefer small fixes over new helper layers unless logic is clearly reused.
- When adding exports, validate escaping and formatting for untrusted file names and paths.
