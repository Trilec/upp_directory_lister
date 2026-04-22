// DirLister - rename pipeline helpers used by the preview stack.
// Change log:
// - 2026-04-22: Added file header comments for maintainability.

#include "RenameEngine.h"

namespace Upp {

namespace {

// Generates a lightweight unique id for step list rows.
String NextStepId()
{
    return AsString(GetSysTime().Get()) + AsString(Random());
}

String GetData(const RenameStep& step, const String& key, const String& def = String())
{
    int i = step.data.Find(key);
    return i >= 0 ? step.data[i] : def;
}

String ReplaceAllText(const String& input, const String& find, const String& replace)
{
    if(find.IsEmpty())
        return input;
    String out;
    int pos = 0;
    while(true) {
        int at = input.Find(find, pos);
        if(at < 0) {
            out << input.Mid(pos);
            break;
        }
        out << input.Mid(pos, at - pos) << replace;
        pos = at + find.GetCount();
    }
    return out;
}

String SplitBase(const String& name)
{
    int dot = name.ReverseFind('.');
    if(dot <= 0 || dot == name.GetCount() - 1)
        return name;
    return name.Left(dot);
}

String SplitExt(const String& name)
{
    int dot = name.ReverseFind('.');
    if(dot <= 0 || dot == name.GetCount() - 1)
        return String();
    return name.Mid(dot);
}

String ToCamelCase(const String& input)
{
    Vector<String> parts;
    String current;
    for(int i = 0; i < input.GetCount(); i++) {
        int c = input[i];
        if(c == '_' || c == '-' || c == ' ' || c == '.') {
            if(!current.IsEmpty()) {
                parts.Add(current);
                current.Clear();
            }
        }
        else
            current.Cat(c);
    }
    if(!current.IsEmpty())
        parts.Add(current);
    if(parts.IsEmpty())
        return input;

    String out = ToLower(parts[0]);
    for(int i = 1; i < parts.GetCount(); i++) {
        String lower = ToLower(parts[i]);
        if(lower.IsEmpty())
            continue;
        out << ToUpper(lower.Left(1)) << lower.Mid(1);
    }
    return out;
}

String ToTitleCase(const String& input)
{
    Vector<String> parts;
    String current;
    for(int i = 0; i < input.GetCount(); i++) {
        int c = input[i];
        if(c == '_' || c == '-' || c == ' ' || c == '.') {
            if(!current.IsEmpty()) {
                parts.Add(current);
                current.Clear();
            }
        }
        else
            current.Cat(c);
    }
    if(!current.IsEmpty())
        parts.Add(current);
    if(parts.IsEmpty())
        return input;
    String out;
    for(int i = 0; i < parts.GetCount(); i++) {
        String lower = ToLower(parts[i]);
        if(lower.IsEmpty())
            continue;
        if(!out.IsEmpty())
            out << ' ';
        out << ToUpper(lower.Left(1)) << lower.Mid(1);
    }
    return out.IsEmpty() ? input : out;
}

String ApplyCase(const String& mode, const String& base)
{
    if(mode == "upper")
        return ToUpper(base);
    if(mode == "camel")
        return ToCamelCase(base);
    if(mode == "title")
        return ToTitleCase(base);
    return ToLower(base);
}

String RenderNumberPattern(const String& pattern, int n)
{
    int start = -1;
    int end = -1;
    for(int i = 0; i < pattern.GetCount(); i++) {
        if(pattern[i] == '#') {
            if(start < 0)
                start = i;
            end = i + 1;
        }
        else if(start >= 0)
            break;
    }
    if(start < 0)
        return AsString(n);
    int width = end - start;
    String padded = Format("%0*d", width, n);
    return pattern.Left(start) + padded + pattern.Mid(end);
}

String ReplaceExtension(const String& name, const String& next_ext)
{
    String cleaned = next_ext;
    if(cleaned.StartsWith("."))
        cleaned = cleaned.Mid(1);
    if(cleaned.IsEmpty())
        return name;
    int dot = name.ReverseFind('.');
    if(dot <= 0 || dot == name.GetCount() - 1)
        return name + "." + cleaned;
    return name.Left(dot + 1) + cleaned;
}

String DedupeName(const String& candidate, Index<String>& existing_names)
{
    if(existing_names.Find(candidate) < 0) {
        existing_names.Add(candidate);
        return candidate;
    }

    String base = SplitBase(candidate);
    String ext = SplitExt(candidate);
    for(int i = 1;; i++) {
        String next = base + "-" + AsString(i) + ext;
        if(existing_names.Find(next) < 0) {
            existing_names.Add(next);
            return next;
        }
    }
}

}

RenameStep RenameEngine::MakeDefaultStep(RenameStepType type)
{
    RenameStep step;
    step.id = NextStepId();
    step.type = type;
    switch(type) {
    case RenameStepType::Case:
        step.data.Add("mode", "lower");
        break;
    case RenameStepType::Alnum:
        step.data.Add("enabled", "true");
        break;
    case RenameStepType::Numbering:
        step.data.Add("find", String());
        step.data.Add("pattern", "####");
        step.data.Add("start", "1");
        break;
    case RenameStepType::Prefix:
        step.data.Add("prefix", String());
        break;
    case RenameStepType::Extension:
        step.data.Add("ext", "txt");
        break;
    case RenameStepType::InsertLeft:
    case RenameStepType::InsertRight:
        step.data.Add("index", "0");
        step.data.Add("text", String());
        break;
    case RenameStepType::FindReplace:
    default:
        step.data.Add("find", String());
        step.data.Add("replace", String());
        break;
    }
    return step;
}

String RenameEngine::GetStepTitle(RenameStepType type)
{
    switch(type) {
    case RenameStepType::FindReplace: return "Find/Replace";
    case RenameStepType::Case: return "Case";
    case RenameStepType::Alnum: return "Alnum";
    case RenameStepType::Numbering: return "Number";
    case RenameStepType::Prefix: return "Prefix";
    case RenameStepType::Extension: return "Ext";
    case RenameStepType::InsertLeft: return "Ins L";
    case RenameStepType::InsertRight: return "Ins R";
    }
    return "Step";
}

String RenameEngine::ApplySteps(const String& original_name,
                                const Vector<RenameStep>& steps,
                                Index<String>& existing_names,
                                int selection_index)
{
    String name = original_name;
    String forced_ext;

    for(const RenameStep& step : steps) {
        switch(step.type) {
        case RenameStepType::FindReplace: {
            String find = GetData(step, "find");
            if(!find.IsEmpty())
                name = ReplaceAllText(name, find, GetData(step, "replace"));
            break;
        }
        case RenameStepType::Case:
            name = ApplyCase(GetData(step, "mode", "lower"), name);
            break;
        case RenameStepType::Alnum:
            if(GetData(step, "enabled", "true") != "false") {
                String next;
                for(int i = 0; i < name.GetCount(); i++) {
                    int c = name[i];
                    if(IsAlNum(c) || c == '.')
                        next.Cat(c);
                }
                name = next;
            }
            break;
        case RenameStepType::Numbering: {
            String find = GetData(step, "find");
            if(find.IsEmpty())
                break;
            int start = ScanInt(GetData(step, "start", "1"));
            String pattern = GetData(step, "pattern", "####");
            name = ReplaceAllText(name, find, RenderNumberPattern(pattern, start + selection_index));
            break;
        }
        case RenameStepType::Prefix: {
            String prefix = GetData(step, "prefix");
            if(!prefix.IsEmpty())
                name = prefix + name;
            break;
        }
        case RenameStepType::Extension: {
            String ext = TrimBoth(GetData(step, "ext"));
            if(!ext.IsEmpty())
                forced_ext = ext;
            break;
        }
        case RenameStepType::InsertLeft: {
            int index = minmax(ScanInt(GetData(step, "index", "0")), 0, name.GetCount());
            String text = GetData(step, "text");
            name = name.Left(index) + text + name.Mid(index);
            break;
        }
        case RenameStepType::InsertRight: {
            int raw = ScanInt(GetData(step, "index", "0"));
            int index = minmax(name.GetCount() - raw, 0, name.GetCount());
            String text = GetData(step, "text");
            name = name.Left(index) + text + name.Mid(index);
            break;
        }
        }
    }

    if(!forced_ext.IsEmpty())
        name = ReplaceExtension(name, forced_ext);

    return DedupeName(name, existing_names);
}

Vector<String> RenameEngine::Preview(const Vector<String>& names,
                                     const Vector<RenameStep>& steps,
                                     const Index<String>& existing_names)
{
    Vector<String> out;
    Index<String> working;
    for(int i = 0; i < existing_names.GetCount(); i++)
        working.FindAdd(existing_names[i]);

    for(int i = 0; i < names.GetCount(); i++) {
        int fi = working.Find(names[i]);
        if(fi >= 0)
            working.Unlink(fi);
        out.Add(ApplySteps(names[i], steps, working, i));
    }
    return out;
}

}
