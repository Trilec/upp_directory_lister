// DirLister - rename pipeline helpers used by the preview stack.
// Change log:
// - 2026-04-22: Added file documentation and comments for the rename contract.

#ifndef _DirLister_RenameEngine_h_
#define _DirLister_RenameEngine_h_

#include <Core/Core.h>

namespace Upp {

// Supported rename operators that can be chained in the preview stack.
enum class RenameStepType : byte {
    FindReplace,
    Case,
    Alnum,
    Numbering,
    Prefix,
    Extension,
    InsertLeft,
    InsertRight,
};

// Serialized description of one rename operation and its typed parameters.
struct RenameStep : Moveable<RenameStep> {
    String id;
    RenameStepType type = RenameStepType::FindReplace;
    VectorMap<String, String> data;
};

// Stateless rename processor used by the UI to build defaults and previews.
class RenameEngine {
public:
    // Creates a new step with sensible defaults for the selected operator.
    static RenameStep MakeDefaultStep(RenameStepType type);
    // Returns a compact title suitable for the visible step stack.
    static String GetStepTitle(RenameStepType type);
    // Applies the step stack to one name and de-duplicates the result if needed.
    static String ApplySteps(const String& original_name,
                             const Vector<RenameStep>& steps,
                             Index<String>& existing_names,
                             int selection_index);
    // Applies the step stack to a batch of samples for preview rendering.
    static Vector<String> Preview(const Vector<String>& names,
                                  const Vector<RenameStep>& steps,
                                  const Index<String>& existing_names);
};

}

#endif
