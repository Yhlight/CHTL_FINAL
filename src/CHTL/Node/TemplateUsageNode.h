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

    std::shared_ptr<BaseNode> clone() const override {
        auto node = std::make_shared<TemplateUsageNode>();
        node->templateType = this->templateType;
        node->name = this->name;
        return node;
    }
};

} // namespace CHTL

#endif // CHTL_TEMPLATE_USAGE_NODE_H
