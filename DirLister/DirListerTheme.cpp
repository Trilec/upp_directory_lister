// DirLister - shared theme helpers for the window and editor widgets.
// Change log:
// - 2026-04-22: Added file header comments for maintainability.

#include "DirListerTheme.h"

namespace Upp {
namespace DirListerTheme {

// Central palette used by the whole window. Keeping this here avoids repeating literal colors in the UI layer.
Color BodyBg()     { return Color(0x0f, 0x11, 0x15); }
Color SidebarBg()  { return Color(0x1a, 0x1d, 0x23); }
Color Border()     { return Color(0x2d, 0x31, 0x39); }
Color Text()       { return Color(0xd1, 0xd5, 0xdb); }
Color Muted()      { return Color(0x6b, 0x72, 0x80); }
Color Blue()       { return Color(0x3b, 0x82, 0xf6); }
Color BlueDark()   { return Color(0x1e, 0x29, 0x3b); }
Color BlueText()   { return Color(0x60, 0xa5, 0xfa); }
Color GreenDark()  { return Color(0x06, 0x4e, 0x3b); }
Color GreenText()  { return Color(0x34, 0xd3, 0x99); }
Color AmberDark()  { return Color(0x45, 0x1a, 0x03); }
Color AmberText()  { return Color(0xfb, 0xbf, 0x24); }
Color OutputBg()   { return Black(); }
Color OutputText() { return Color(0xa5, 0xb4, 0xfc); }
Color StatusFace() { return Color(0x1e, 0x29, 0x3b); }
Color StatusText() { return Color(0x38, 0xbd, 0xf8); }

Font AppSans(int px, bool bold)
{
    Font f = SansSerifZ(px);
    if(Font::FindFaceNameIndex("Inter") >= 0)
        f.FaceName("Inter");
    if(bold)
        f.Bold();
    return f;
}

UiPanel::Style MakePanelStyle(Color face, Color frame, int radius, int margin)
{
    UiPanel::Style s = UiTheme::ResolvePanel(UiPanelRole::Surface);
    for(int i = 0; i < 4; i++) {
        s.palette.face[i] = UiFill::Solid(face);
        s.palette.frame[i] = frame;
        s.palette.ink[i] = Color(224, 231, 255);
    }
    s.metrics.face_enabled = true;
    s.metrics.frame_enabled = true;
    s.metrics.frame_width = DPI(1);
    s.metrics.radius = DPI(radius);
    s.metrics.focus_enabled = false;
    s.metrics.content_margin = Rect(DPI(margin), DPI(margin), DPI(margin), DPI(margin));
    s.metrics.shadow.enabled = false;
    return s;
}

UiScrollPanel::Style MakeScrollPanelStyle()
{
    UiScrollPanel::Style s = UiScrollPanel::StyleDefault();
    for(int i = 0; i < 4; i++) {
        s.palette.face[i] = UiFill::None();
        s.palette.frame[i] = Null;
    }
    s.transparent = true;
    s.metrics.face_enabled = false;
    s.metrics.frame_enabled = false;
    s.metrics.focus_enabled = false;
    s.metrics.radius = 0;
    s.metrics.frame_width = 0;
    s.metrics.content_margin = Rect(0, 0, 0, 0);
    return s;
}

UiLabel::Style MakeLabelStyle(Color ink, UiLabelRole role, bool transparent)
{
    UiLabel::Style s = UiTheme::ResolveLabel(role);
    for(int i = 0; i < 4; i++) {
        s.palette.ink[i] = ink;
        if(transparent) {
            s.palette.face[i] = UiFill::None();
            s.palette.frame[i] = Null;
        }
    }
    s.transparent = transparent;
    s.metrics.face_enabled = !transparent;
    s.metrics.frame_enabled = false;
    if(role == UiLabelRole::Title)
        s.font = AppSans(18, true);
    else if(role == UiLabelRole::Caption)
        s.font = AppSans(8, true);
    else
        s.font = AppSans(10, false);
    s.metrics.text_font = s.font;
    s.metrics.use_text_font = true;
    return s;
}

UiLabel::Style MakeBadgeStyle(Color face, Color ink)
{
    UiLabel::Style s = UiTheme::ResolveLabel(UiLabelRole::Badge);
    for(int i = 0; i < 4; i++) {
        s.palette.face[i] = UiFill::Solid(face);
        s.palette.frame[i] = Border();
        s.palette.ink[i] = ink;
    }
    s.metrics.face_enabled = true;
    s.metrics.frame_enabled = true;
    s.metrics.frame_width = DPI(1);
    s.metrics.radius = DPI(999);
    s.metrics.focus_enabled = false;
    s.metrics.content_margin = Rect(DPI(10), DPI(3), DPI(10), DPI(3));
    s.font = AppSans(8, true);
    return s;
}

UiBaseEdit::Style MakeEditStyle()
{
    UiBaseEdit::Style s = UiTheme::ResolveEdit(UiEditRole::Field);
    for(int i = 0; i < 4; i++) {
        s.palette.face[i] = UiFill::Solid(BodyBg());
        s.palette.frame[i] = Border();
        s.palette.ink[i] = White();
    }
    s.metrics.face_enabled = true;
    s.metrics.frame_enabled = true;
    s.metrics.frame_width = DPI(1);
    s.metrics.radius = DPI(4);
    s.metrics.focus_enabled = false;
    s.metrics.content_margin = Rect(DPI(8), DPI(5), DPI(8), DPI(5));
    s.font = AppSans(10);
    s.metrics.text_font = s.font;
    s.metrics.use_text_font = true;
    s.placeholder_ink = Muted();
    s.selection_color = Color(0x25, 0x63, 0xeb);
    s.selection_ink = White();
    return s;
}

UiDropdown::Style MakeDropdownStyle()
{
    UiDropdown::Style s = UiTheme::ResolveDropdown();
    for(int i = 0; i < 4; i++) {
        s.palette.face[i] = UiFill::Solid(BodyBg());
        s.palette.frame[i] = Border();
        s.palette.ink[i] = White();
    }
    s.metrics.face_enabled = true;
    s.metrics.frame_enabled = true;
    s.metrics.frame_width = DPI(1);
    s.metrics.radius = DPI(4);
    s.metrics.focus_enabled = false;
    s.metrics.content_margin = Rect(DPI(8), DPI(5), DPI(8), DPI(5));
    s.font = AppSans(10);
    s.transparent = false;
    for(int i = 0; i < 4; i++) {
        s.popup_item_style.palette.face[i] = UiFill::Solid(BodyBg());
        s.popup_item_style.palette.frame[i] = Null;
        s.popup_item_style.palette.ink[i] = Text();
    }
    s.popup_item_style.palette.face[ST_HOT] = UiFill::Solid(Color(0x37, 0x41, 0x51));
    s.popup_item_style.palette.ink[ST_HOT] = White();
    s.popup_item_style.palette.face[ST_PRESSED] = UiFill::Solid(Blue());
    s.popup_item_style.palette.ink[ST_PRESSED] = White();
    s.popup_item_style.palette.ink[ST_DISABLED] = Muted();
    s.popup_item_style.font = AppSans(10);
    s.popup_frame_color = Border();
    s.popup_background_color = BodyBg();
    s.popup_frame_width = DPI(1);
    s.popup_radius = DPI(4);
    return s;
}

UiCheckBox::Style MakeCheckStyle()
{
    UiCheckBox::Style s = UiTheme::ResolveCheckBox(UICHECKVIS_CLASSIC);
    for(int i = 0; i < 4; i++) {
        s.palette.face[i] = UiFill::None();
        s.palette.frame[i] = Null;
        s.palette.ink[i] = Text();
        s.indicator_palette.face[i] = UiFill::Solid(BodyBg());
        s.indicator_palette.frame[i] = Muted();
        s.indicator_palette.ink[i] = White();
    }
    s.indicator_palette.face[ST_HOT] = UiFill::Solid(Color(0x13, 0x17, 0x1d));
    s.indicator_palette.frame[ST_HOT] = Color(0x4b, 0x55, 0x63);
    s.indicator_palette.face[ST_PRESSED] = UiFill::Solid(Blue());
    s.indicator_palette.frame[ST_PRESSED] = Blue();
    s.indicator_palette.ink[ST_PRESSED] = White();
    s.font = AppSans(9);
    s.metrics.face_enabled = false;
    s.metrics.frame_enabled = false;
    s.metrics.focus_enabled = false;
    s.metrics.content_margin = Rect(0, 0, 0, 0);
    s.indicator_metrics.face_enabled = true;
    s.indicator_metrics.frame_enabled = true;
    s.indicator_metrics.focus_enabled = false;
    s.indicator_metrics.frame_width = DPI(1);
    s.indicator_metrics.radius = DPI(4);
    s.indicator_size = DPI(16);
    s.indicator_gap = DPI(8);
    s.mark_thickness = DPI(2);
    s.checked_icon = ICON_DESIGN_CHECK_SMALL_48();
    s.marker_render_mode = UiIconRenderMode::MonoTint;
    return s;
}

UiButton::Style MakeActionStyle(bool primary)
{
    UiButton::Style s = UiTheme::ResolveButton(primary ? UiButtonRole::Accent : UiButtonRole::Subtle);
    for(int i = 0; i < 4; i++) {
        s.palette.face[i] = UiFill::Solid(primary ? Blue() : BodyBg());
        s.palette.frame[i] = primary ? Blue() : Color(0x4b, 0x55, 0x63);
        s.palette.ink[i] = primary ? White() : Color(0x9c, 0xa3, 0xaf);
    }
    s.palette.face[ST_HOT] = UiFill::Solid(primary ? Color(0x25, 0x63, 0xeb) : BodyBg());
    s.palette.ink[ST_HOT] = primary ? White() : Color(0xe5, 0xe7, 0xeb);
    s.font = AppSans(9, true);
    s.metrics.radius = DPI(999);
    s.metrics.frame_width = DPI(1);
    s.metrics.content_margin = Rect(DPI(18), DPI(6), DPI(18), DPI(6));
    s.metrics.focus_enabled = false;
    return s;
}

UiButton::Style MakeNavButtonStyle(Color face, Color ink, bool active)
{
    UiButton::Style s = UiTheme::ResolveButton(UiButtonRole::Subtle);
    for(int i = 0; i < 4; i++) {
        s.palette.face[i] = UiFill::Solid(active ? face : BodyBg());
        s.palette.frame[i] = Null;
        s.palette.ink[i] = active ? ink : Color(0x4b, 0x55, 0x63);
    }
    s.palette.face[ST_HOT] = UiFill::Solid(active ? face : Color(0x13, 0x17, 0x1d));
    s.palette.ink[ST_HOT] = active ? ink : Text();
    s.font = AppSans(8, true);
    s.metrics.radius = DPI(6);
    s.metrics.frame_width = 0;
    s.metrics.focus_enabled = false;
    s.metrics.content_margin = Rect(DPI(10), DPI(7), DPI(10), DPI(7));
    return s;
}

UiToolButton::Style MakeSmallButtonStyle()
{
    UiToolButton::Style s = UiTheme::ResolveToolButton();
    for(int i = 0; i < 4; i++) {
        s.palette.face[i] = UiFill::Solid(Color(0x37, 0x41, 0x51));
        s.palette.frame[i] = Border();
        s.palette.ink[i] = White();
    }
    s.font = AppSans(9, true);
    s.metrics.radius = DPI(4);
    s.metrics.frame_width = DPI(1);
    s.metrics.focus_enabled = false;
    s.metrics.content_margin = Rect(DPI(7), DPI(5), DPI(7), DPI(5));
    return s;
}

UiTab::Style MakeTabStyle(int active)
{
    UiTab::Style s = UiTheme::ResolveTab(UITAB_SEGMENTED);
    s.visual = UITAB_SEGMENTED;
    s.fill_tabs = true;
    s.metrics.radius = DPI(8);
    s.tab_metrics.radius = DPI(6);
    s.metrics.frame_width = 0;
    s.metrics.face_enabled = true;
    s.metrics.frame_enabled = false;
    s.tab_metrics.frame_width = 0;
    s.tab_metrics.face_enabled = true;
    s.tab_metrics.focus_enabled = false;
    s.metrics.content_margin = Rect(DPI(2), DPI(2), DPI(2), DPI(2));
    s.tab_padding = Rect(DPI(10), DPI(7), DPI(10), DPI(7));
    s.tab_font = AppSans(8, true);
    for(int i = 0; i < 4; i++) {
        s.palette.face[i] = UiFill::Solid(BodyBg());
        s.palette.frame[i] = Null;
        s.palette.ink[i] = Color(0x4b, 0x55, 0x63);
        s.tab_palette.face[i] = UiFill::Solid(BodyBg());
        s.tab_palette.frame[i] = Null;
        s.tab_palette.ink[i] = Color(0x4b, 0x55, 0x63);
    }
    Color active_face = BlueDark();
    Color active_ink = BlueText();
    if(active == 1) {
        active_face = GreenDark();
        active_ink = GreenText();
    }
    else if(active == 2) {
        active_face = AmberDark();
        active_ink = AmberText();
    }
    s.tab_palette.face[ST_PRESSED] = UiFill::Solid(active_face);
    s.tab_palette.ink[ST_PRESSED] = active_ink;
    s.tab_palette.face[ST_HOT] = UiFill::Solid(BodyBg());
    s.tab_palette.ink[ST_HOT] = Text();
    return s;
}

void StyleDocEdit(DocEdit& edit, Color ink, Color paper, Font font)
{
    edit.NoBackground();
    edit.SetFont(font);
    edit.SetColor(TextCtrl::INK_NORMAL, ink);
    edit.SetColor(TextCtrl::INK_DISABLED, Muted());
    edit.SetColor(TextCtrl::INK_SELECTED, White());
    edit.SetColor(TextCtrl::PAPER_NORMAL, paper);
    edit.SetColor(TextCtrl::PAPER_READONLY, paper);
    edit.SetColor(TextCtrl::PAPER_SELECTED, Blue());
}

void StyleEditField(EditField& edit, const char* null_text)
{
    edit.SetBackground(BodyBg()).SetColor(White()).SetFont(AppSans(10));
    if(null_text)
        edit.NullText(null_text, Muted());
}

}
}
