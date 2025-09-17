#ifndef CHTL_TEMPLATE_USAGE_NODE_H
#define CHTL_TEMPLATE_USAGE_NODE_H

#include "BaseNode.h"
#include "TemplateNode.h" // For TemplateType enum
#include <string>

namespace CHTL {

// This node will be placed in the AST where a template is used.
class TemplateUsageNode : public BaseNode {
public:
    TemplateType templateType;
    std::string name;
};

} // namespace CHTL

#endif // CHTL_TEMPLATE_USAGE_NODE_H
