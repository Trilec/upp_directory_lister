# DirLister

DirLister is a desktop tool for exploring a directory, filtering what you see, generating clean listings, previewing batch rename rules, and copying matching files to another location.

It is built with U++ and the newer `upp_Ui` controls.

![DirLister application snapshot](./DirLister_snapshot.jpg)

## What DirLister Is Useful For

- creating a clean file or folder inventory for a project or archive
- listing only the files you care about, using patterns and filters
- previewing rename rules before applying them
- copying matching files into a backup, export, or staging directory
- producing output that is easy to read or paste into notes, tickets, or documentation

## Main Features

- source directory selection
- file and directory pattern filtering
- pattern modes: `Glob` and `Contains`
- recursive scanning with depth control
- size and date filtering
- flexible sorting and directory placement
- text, CSV, and JSON output
- rename process stack with live preview
- transfer/copy workflow with conflict handling
- built-in Help dialog with examples and usage notes

## Quick Start

1. Choose a `Source Directory`.
2. Open `Scan Filter` and set any file patterns, directory patterns, or filters you want.
3. Click `Generate List` to preview the current result.
4. If needed, switch to `Rename` or `Transfer` to work on the filtered set.

## Scan Filter

The `Scan Filter` page controls what the app looks at.

You can use it to:

- limit which files are included with file patterns such as `*.cpp;*.h;*.md`
- limit which folders are included with directory patterns such as `src*;docs*`
- choose whether matching is `Glob` based or simple `Contains` text matching
- optionally enable `Case Sensitive` matching
- restrict results by size or modification date
- control recursive scanning depth
- change sort order and how directories are grouped
- choose which fields appear in the output

The main point of this page is that it defines the working set for the rest of the tool.

## Generate a Listing

Use `Generate List` when you want a readable or exportable view of the current scan result.

Available output modes:

- `Text Output`: easy to read and easy to paste elsewhere
- `CSV Output`: useful for spreadsheets or post-processing
- `JSON Output`: useful for tooling or structured export

If you enable path, size, date, or extension display, those values are added to each line of the generated result.

## Rename

The `Rename` page lets you build a stack of rename processes and preview the result before applying it.

Supported process types include:

- Search & Replace
- Case Transform
- Alphanumeric Only
- Numbering
- Prefix
- Extension Replace
- Insert Left
- Insert Right

Typical workflow:

1. Choose a process.
2. Fill in its parameters.
3. Click `Add` to place it into the stack.
4. Reorder the stack by dragging rows.
5. Review the preview.
6. Click `Apply Rename` when you are satisfied.

Notes:

- the rename preview uses entries from the active source directory
- rename applies to eligible entries in that source directory
- it is best to preview carefully before applying extension or numbering rules

## Transfer

The `Transfer` page copies matching files and folders into a target directory.

Options include:

- preserve the original folder tree
- flatten files into a single destination level
- verify copied files after transfer
- choose how file conflicts are handled

Conflict handling modes:

- `Auto-Increment`: creates a new target name when a file already exists
- `Overwrite Existing`: replaces the existing target file
- `Skip Existing`: leaves existing target files untouched

Typical workflow:

1. Choose the target directory.
2. Decide whether to preserve the tree or flatten the files.
3. Choose the conflict policy.
4. Click `Apply Transfer` and confirm.

## Help

Use the top-bar `Help` button for an in-app guide covering:

- listing generation
- rename workflow
- transfer workflow
- preview and apply flow
- practical examples

## Notes

- `Linux Slashes` changes the visible path style for easier copy/paste into tools and docs.
- `Generate List` is safe and preview-oriented.
- `Apply Rename` and `Apply Transfer` perform real filesystem operations and ask for confirmation first.

## Build

Package file:

- `DirLister/DirLister.upp`

Required U++ packages:

- `Core`
- `Draw`
- `CtrlCore`
- `CtrlLib`
- `Ui`
