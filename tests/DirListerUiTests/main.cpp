#include "../../DirLister/MainWindow.h"
#include <plugin/png/png.h>
using namespace Upp;
template <class T> T* Find(Ctrl& parent, const String& text = String()) {
    for(Ctrl* c = parent.GetFirstChild(); c; c = c->GetNext()) {
        if(T* t = dynamic_cast<T*>(c))
            if(text.IsEmpty() || t->GetData().ToString() == text) return t;
        if(T* t = Find<T>(*c, text)) return t;
    }
    return nullptr;
}
UiButton* Generate(Ctrl& parent) {
    for(Ctrl* c = parent.GetFirstChild(); c; c = c->GetNext()) {
        if(auto* b = dynamic_cast<UiButton*>(c)) if(b->GetText() == "GENERATE LIST") return b;
        if(auto* b = Generate(*c)) return b;
    }
    return nullptr;
}
void FilesOff(Ctrl& parent) {
    for(Ctrl* c = parent.GetFirstChild(); c; c = c->GetNext()) {
        if(auto* b = dynamic_cast<UiCheckBox*>(c)) if(b->GetText() == "Files") b->SetChecked(false);
        FilesOff(*c);
    }
}
UiDropdown* Dropdown(Ctrl& parent, const char* first_label) {
    for(Ctrl* c = parent.GetFirstChild(); c; c = c->GetNext()) {
        if(auto* d = dynamic_cast<UiDropdown*>(c))
            if(d->Model().GetCount() && d->Model().Get(0).text == first_label) return d;
        if(auto* d = Dropdown(*c, first_label)) return d;
    }
    return nullptr;
}
UiButton* FindUiButton(Ctrl& parent, const char* text) {
    for(Ctrl* c = parent.GetFirstChild(); c; c = c->GetNext()) {
        if(auto* b = dynamic_cast<UiButton*>(c)) if(b->GetText() == text) return b;
        if(auto* b = FindUiButton(*c, text)) return b;
    }
    return nullptr;
}
void SetCheck(Ctrl& parent, const char* text, bool value) {
    for(Ctrl* c = parent.GetFirstChild(); c; c = c->GetNext()) {
        if(auto* b = dynamic_cast<UiCheckBox*>(c)) if(b->GetText() == text) b->SetChecked(value);
        SetCheck(*c, text, value);
    }
}
void Snapshot(MainWindow& window, const char* path) {
    ImageDraw draw(window.GetSize());
    window.DrawCtrl(draw);
    PNGEncoder().SaveFile(path, (Image)draw);
}

GUI_APP_MAIN {
    MainWindow window;
    window.Layout();
    auto* source = Find<UiLineEdit>(window, GetCurrentDirectory());
    auto* output = Find<LineEdit>(window);
    auto* generate = Generate(window);
    String report;
    bool ok = source && output && generate;
    if(ok) {
        String root = "I:/archive/fbb/BB_job/prod/work";
        if(!DirectoryExists(root)) root = AppendFileName(GetTempPath(), "DirLister-tests/scan-fixture");
        source->SetData(root);
        auto* depth_label = Find<UiLabel>(window, "Depth");
        auto* depth = depth_label ? dynamic_cast<EditInt*>(depth_label->GetNext()) : nullptr;
        if(depth) depth->SetData(2);
        FilesOff(window);
        generate->WhenAction();
        output->Layout();
        DirectoryScanSettings s; s.source_directory = root; s.include_files = false;
        int expected = DirectoryEngine::Scan(s).GetCount();
        ok = expected > 200 && output->GetLineCount() == expected + 1;
        output->MoveTextEnd();
        output->ScrollIntoCursor();
        Point scroll = output->GetScrollPos();
        Size page = output->GetPageSize();
        ok = ok && scroll.y > 200 && scroll.y + page.cy >= expected;
        report << "Rows: " << output->GetLineCount() << ", bottom scroll: " << scroll.y
               << ", visible rows: " << page.cy << "\n";
        window.SetRect(0, 0, DPI(900), DPI(600)); window.Layout(); output->Layout();
        output->MoveTextEnd(); output->ScrollIntoCursor();
        ok = ok && output->GetSize().cx > 0 && output->GetSize().cy > 0
                && output->GetScrollPos().y + output->GetPageSize().cy >= expected;
    }
    if(ok) {
        source->SetData(AppendFileName(GetTempPath(), "DirLister-tests/metadata-fixture"));
        SetCheck(window, "Files", true); SetCheck(window, "Recursive Scanning", false);
        auto* type = Dropdown(window, "Name matches glob");
        auto* target = Dropdown(window, "Files");
        auto* units = Dropdown(window, "B");
        auto* stack = Find<UiList>(window);
        auto* pattern = Find<UiLineEdit>(*type->GetParent());
        auto* add = FindUiButton(*type->GetParent(), "Add");
        auto* save = FindUiButton(*type->GetParent(), "Save");
        auto* remove = FindUiButton(*type->GetParent(), "Delete");
        auto* minimum = Find<EditDouble>(window);
        EditDouble* maximum = nullptr;
        for(Ctrl* c = minimum->GetNext(); c; c = c->GetNext())
            if((maximum = dynamic_cast<EditDouble*>(c))) break;
        type->SelectByData(5); minimum->SetData(4); maximum->SetData(10); units->SelectByData(0);
        add->WhenAction();
        ok = ok && stack->Model().GetCount() == 1;
        type->SelectByData(2); target->SelectByData(0); pattern->SetData("a"); add->WhenAction();
        generate->WhenAction();
        String result = output->GetData();
        ok = ok && stack->Model().GetCount() == 2 && result.Find("a4.txt") >= 0 && result.Find("b10.txt") < 0;
        type->SelectByData(3); pattern->SetData("a4"); pattern->WhenChange();
        generate->WhenAction();
        ok = ok && output->GetData().ToString().Find("a4.txt") >= 0 && save->IsEnabled();
        save->WhenAction(); generate->WhenAction();
        result = output->GetData();
        ok = ok && result.Find("a4.txt") < 0 && result.Find("b10.txt") >= 0;
        type->SelectByData(4); target->SelectByData(0); pattern->SetData("*.txt");
        Dropdown(window, "Glob")->SelectByData(0);
        auto* limit = Find<EditInt>(*type->GetParent(), "3"); limit->SetData(1); add->WhenAction();
        generate->WhenAction();
        ok = ok && output->GetData().ToString().Find("b10.txt") >= 0;
        UiReorderRequest request; request.from = 2; request.before = 0;
        stack->WhenReorderRequest(request);
        generate->WhenAction();
        result = output->GetData();
        ok = ok && request.handled && stack->Model().Get(0).text.Find("First N") >= 0 && result.Find("b10.txt") < 0;
        remove->WhenAction(); generate->WhenAction();
        ok = ok && stack->Model().GetCount() == 2 && output->GetData().ToString().Find("b10.txt") >= 0;
        type->SelectByData(7);
        auto* from_date = Find<DropDate>(window);
        auto* to_date = dynamic_cast<DropDate*>(from_date->GetNext());
        from_date->SetData(Date(2026, 1, 1)); to_date->SetData(Date(2026, 1, 1)); target->SelectByData(0);
        ok = ok && from_date->IsShown() && !minimum->IsShown() && !pattern->IsShown();
        add->WhenAction(); generate->WhenAction();
        ok = ok && stack->Model().GetCount() == 3 && output->GetData().ToString().Find("b10.txt") >= 0;
        window.SetRect(0, 0, DPI(1280), DPI(760)); window.Layout();
        auto* scroll_panel = Find<UiScrollPanel>(window);
        scroll_panel->SetScrollPos(Point(0, 0));
        Snapshot(window, AppendFileName(GetTempPath(), "DirLister-tests/filter-date-ui.png"));
        type->SelectByData(4); pattern->SetData("BB_*"); target->SelectByData(1); limit->SetData(3);
        ok = ok && pattern->IsShown() && !from_date->IsShown();
        Snapshot(window, AppendFileName(GetTempPath(), "DirLister-tests/filter-stack-ui.png"));
        report << (ok ? "PASS: inline mixed stack Add/Save/Delete/reorder and dynamic parameters\n"
                      : "FAIL: inline filter stack interactions\n");
    }
    if(ok) {
        auto* type = Dropdown(window, "Search & Replace");
        auto* panel = type->GetParent();
        auto* parameter = Find<UiLineEdit>(*panel);
        auto* stack = Find<UiList>(*panel);
        auto* add = FindUiButton(*panel, "Add");
        auto* preview = Find<DocEdit>(*panel);
        UiLineEdit* sample = nullptr;
        for(Ctrl* c = panel->GetFirstChild(); c; c = c->GetNext())
            if(auto* edit = dynamic_cast<UiLineEdit*>(c)) sample = edit;
        SetCheck(window, "Enable", false);
        type->SelectByData((int)RenameStepType::Prefix);
        parameter->SetData("Project_"); parameter->WhenChange(); add->WhenAction();
        type->SelectByData((int)RenameStepType::Case); add->WhenAction();
        sample->SetData("MyFile.txt"); sample->WhenChange();
        ok = ok && stack->Model().GetCount() == 3
                && preview->GetData().ToString().Find("project_myfile.txt") >= 0;
        UiReorderRequest request; request.from = 2; request.before = 1;
        stack->WhenReorderRequest(request);
        ok = ok && request.handled && stack->Model().Get(1).text == "Case"
                && preview->GetData().ToString().Find("Project_myfile.txt") >= 0;
        report << (ok ? "PASS: rename drag reorder changes executed preview order\n"
                      : "FAIL: rename drag reorder execution\n");
    }
    report << (ok ? "PASS: native output loads all rows and scrolls to end at two window sizes\n"
                  : "FAIL: native output row/scroll checks\n");
    SaveFile(AppendFileName(GetTempPath(), "DirLister-tests/native-ui-check.txt"), report);
    SetExitCode(ok ? 0 : 1);
}
