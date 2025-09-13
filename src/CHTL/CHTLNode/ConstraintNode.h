#ifndef CHTL_CONSTRAINT_NODE_H
#define CHTL_CONSTRAINT_NODE_H

#include <string>
#include <vector>

namespace CHTL {

enum class ConstraintType {
    // e.g., except span;
    TagName,
    // e.g., except @Html;
    HtmlTagType,
    // e.g., except [Template];
    TemplateRootType,
    // e.g., except [Template] @Element;
    TemplateSubType,
};

struct Constraint {
    ConstraintType type;
    // For TagName, this is the tag name.
    // For TemplateSubType, this is the sub-type (e.g., "@Element").
    // For others, this can be empty.
    std::string identifier;
};

} // namespace CHTL

#endif // CHTL_CONSTRAINT_NODE_H
