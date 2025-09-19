#ifndef TEMPLATE_USAGE_NODE_H
#define TEMPLATE_USAGE_NODE_H

#include "BaseNode.h"
#include "TemplateDefinitionNode.h" // For TemplateType
#include <string>

namespace CHTL {

class TemplateUsageNode : public BaseNode {
public:
    TemplateUsageNode(TemplateType type, const std::string& name)
        : type(type), name(name) {}

    void accept(Visitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<TemplateUsageNode>(*this);
    }

    TemplateType type;
    std::string name;
};

} // namespace CHTL

#endif // TEMPLATE_USAGE_NODE_H
