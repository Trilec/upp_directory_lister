#include "AppInfo.h"

namespace Upp {

String GetDirListerHelpText()
{
    return String("DirLister ") + DIRLISTER_VERSION + R"HELP( - User Guide

QUICK START
Choose a Source Directory. The Filter stack chooses which entries are included;
the Rename stack changes the names of those selected entries. Generate List
shows the current selection without changing files. Apply Rename and Apply
Transfer ask for confirmation before changing the filesystem.

BOTH STACKS: ADD, EDIT, SAVE AND REORDER
Choose a process from the dropdown. Its parameter fields appear below it.
Fill the parameters and click Add to create a step. To edit an existing step,
select its stack row, change its parameters and click Save. Delete removes the
selected step. Drag the handle at the right of a row to change its position.
The stacks run from top to bottom, so order can change the result.
Changing fields alone does not replace a saved step. Save edits before applying.

FILTER STACK
Each step selects Files, Directories, or Files + directories. The Apply at level field
controls only where that step applies, not how far scanning goes.
It limits that step to a directory level: 0 means any level, 1 means source children,
2 means their children, and so on. Add enables filtering. The Enable checkbox
bypasses the whole Filter stack when switched off.

Name matches glob: keep matching names. * matches any sequence, ? one character.
Name excludes glob: remove matching names and skip matching folder branches.
Name contains: keep names containing the supplied text.
Name doesn't contain: remove matching names and skip matching folder branches.
For name steps, separate alternatives with semicolons, for example *.jpg;*.png.
Several positive steps intersect: each applicable step must pass. Matching is
case insensitive unless Case is checked. Case means case-sensitive name matching.
Empty name patterns match all.

First N matches / parent: keep N matching siblings under each parent. Choose
Glob or Contains and set Keep. Excess matching folder branches are skipped;
other names remain eligible. Keep = 0 skips all matching entries. Enable sorting
below the stack to choose a predictable first N; reverse sorting reverses that
choice. Put exclusions or ranges before First N to sample eligible entries.

Size range / Outside size range: keep file sizes inside / outside the bounds.
Choose B, KB, MB or GB. Each zero bound is unlimited. Folder sizes are not measured.
Modified date range / Outside modified date range: keep modification dates inside
/ outside the inclusive From and To bounds. Blank dates are unlimited. Reversed
size/date bounds are normalized. Entries with unknown dates are unaffected.
Size/date steps do not prune folder branches: children have independent metadata.

VIEW, DEPTH AND OUTPUT
Scan depth limit and Recursive are below Location History, above the tabs.
Scroll below the Filter stack for View and Sorting settings. Select Dirs,
Files, or both; Hidden includes hidden entries. Depth counts extra levels below
source children: depth 0 lists immediate children, depth 1 lists two levels,
and depth 2 lists three levels. With recursion off, only source children appear.

Tree Output keeps parents and children together with ASCII branches. Missing
filtered ancestors are shown as [context]; those ancestors are display context,
not selected entries for rename or transfer. Flat Text uses relative paths to
separate equal names under different parents. CSV and JSON are export formats.
Path, Ext, Date and Size change display fields; Linux Slashes changes separators.
Copy Output copies the complete report, including entries beyond the visible area.

FILTER EXAMPLE: SHOW A FEW SHOTS FROM A LARGE ARCHIVE
Choose I:/archive/fbb/BB_job/prod/work. Under View select Dirs and clear Files.
Enable recursion, set Scan depth limit 2, and choose name sorting and Tree Output.
Add First N matches / parent: Directories, Glob BB_*, Apply at level 0, Keep 3.
Generate List. You see three BB_* folders per parent and their selected children,
plus folders whose names do not match BB_*. Set Keep 1 for a single sample.
To sample numeric shot folders within each selected BB_* folder, add another
First N step: Directories, Glob *, Apply at level 3, Keep 1. It limits that level only.

FILTER EXAMPLE: RECENT IMAGES, EXCLUDING TEMPORARY NAMES
Add these steps in order, all targeting Files:
1. Name matches glob: *.jpg;*.png
2. Name doesn't contain: temp;backup
3. Size range: Min 1, Max 0, unit MB
4. Modified date range: From 2026-01-01, To blank
Generate List. Only images of at least 1 MB, modified on or after that date,
and without temp/backup in their names remain. Each step narrows the selection.
To take three eligible images per parent, add First N last: Files, Glob *, Keep 3.

RENAME STACK
The Rename preview uses the active filtered entries. Sample input lets you test
a name separately; the preview count controls how many examples are shown.
Sample preview does not itself choose which filesystem entries will be renamed.
Generate List first to inspect the full working set, then review the rename preview.

Search & Replace: replace all occurrences of Find with Replace (an empty Find
leaves the name unchanged). Case Transform: lower, upper, camel or title case.
Alphanumeric Only: remove non-alphanumeric characters, preserving dots.
Numbering: replace a Find token using a pattern such as #### and a Start value.
Prefix: insert text at the start of the name.
Extension Replace: replace the extension; this renames the file, not its format.
Insert Left / Insert Right: insert text at the chosen offset from that end.

RENAME EXAMPLE: CLEAN IMAGE NAMES AND ADD A PROJECT PREFIX
Filter Files with Name matches glob *.jpg;*.png. Generate List to check the set.
Remove any unwanted existing Rename steps, then Add these processes in order:
1. Search & Replace: Find a space, Replace _
2. Case Transform: lower
3. Prefix: project_
Preview input My Image.JPG becomes project_my_image.jpg. Save edits to any
selected step, review several samples, then Apply Rename and confirm.

RENAME EXAMPLE: NUMBER A TOKEN
Filter Files with Name contains TOKEN. Add Numbering: Find TOKEN, Pattern ####,
Start 1. With name sorting enabled, shot_TOKEN.exr becomes shot_0001.exr for the
first selected entry, with subsequent entries using 0002, 0003, and so on.
Numbering only replaces the token; names without it stay unchanged.

WHY RENAME ORDER MATTERS
Prefix Project_ followed by lower case produces project_myfile.txt.
Lower case followed by Prefix Project_ produces Project_myfile.txt.
Drag steps to choose the result, then check the preview before applying.
Existing-name collisions can receive an automatic numeric suffix.

COMBINING THE STACKS
For an archive containing final and temporary shots, first Filter Files by
Name matches glob *.exr and Name doesn't contain temp. Generate List to verify
that selection. Then Rename with Prefix final_. The Filter stack chooses the
entries; the Rename stack transforms their names. The same Filter stack also
limits Transfer. Do not confuse the small rename sample with the full selection.

TRANSFER
Choose a target, Preserve Tree or Flatten Files, and a conflict policy:
Auto-Increment keeps existing files and chooses a new destination name;
Overwrite Existing replaces them; Skip Existing leaves them untouched.
Verification compares copied file content. Apply Transfer shows the planned
entry count and target for confirmation, then reports the result in the output.

PRACTICAL TIPS
Start with a shallow depth and Generate List before changing files.
Use name exclusions to avoid scanning unwanted folder branches.
Save edited steps and inspect the full selection, not just the rename samples.
Test a rename recipe on a small copy of a directory before using it broadly.
The current scan is synchronous; Abort does not interrupt a running scan yet.
)HELP";
}

}
