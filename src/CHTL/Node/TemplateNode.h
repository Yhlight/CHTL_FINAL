#ifndef CHTL_TEMPLATE_NODE_H
#define CHTL_TEMPLATE_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

enum class TemplateType {
    STYLE,
    ELEMENT,
    VAR
};

class TemplateNode : public BaseNode {
public:
    TemplateType templateType;
    std::string name;
    // The body of a template can contain other nodes.
    // For a style template, the body will be a StyleNode.
    // For an element template, it will be a list of other nodes.
    // We can use a generic container for now.
    std::vector<std::shared_ptr<BaseNode>> body;
};

} // namespace CHTL

#endif // CHTL_TEMPLATE_NODE_H
