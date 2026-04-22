// DirLister - shared theme helpers for the window and editor widgets.
// Change log:
// - 2026-04-22: Added file documentation and function intent comments.

#ifndef _DirLister_DirListerTheme_h_
#define _DirLister_DirListerTheme_h_

#include <CtrlLib/CtrlLib.h>
#include <Ui/Ui.h>

namespace Upp {
namespace DirListerTheme {

// Shared palette accessors used across the window.
Color BodyBg();
Color SidebarBg();
Color Border();
Color Text();
Color Muted();
Color Blue();
Color BlueDark();
Color BlueText();
Color GreenDark();
Color GreenText();
Color AmberDark();
Color AmberText();
Color OutputBg();
Color OutputText();
Color StatusFace();
Color StatusText();

// Returns the preferred UI font while gracefully falling back if Inter is unavailable.
Font AppSans(int px, bool bold = false);

// Shared style builders keep appearance decisions centralized instead of scattered in the window.
UiPanel::Style MakePanelStyle(Color face, Color frame, int radius, int margin);
UiScrollPanel::Style MakeScrollPanelStyle();
UiLabel::Style MakeLabelStyle(Color ink, UiLabelRole role = UiLabelRole::Body, bool transparent = true);
UiLabel::Style MakeBadgeStyle(Color face, Color ink);
UiBaseEdit::Style MakeEditStyle();
UiDropdown::Style MakeDropdownStyle();
UiCheckBox::Style MakeCheckStyle();
UiButton::Style MakeActionStyle(bool primary);
UiButton::Style MakeNavButtonStyle(Color face, Color ink, bool active);
UiToolButton::Style MakeSmallButtonStyle();
UiTab::Style MakeTabStyle(int active);

// Styling helpers for controls that still use CtrlLib text/editor APIs directly.
void StyleDocEdit(DocEdit& edit, Color ink, Color paper, Font font);
void StyleEditField(EditField& edit, const char* null_text = nullptr);

}
}

#endif
