#ifndef CHTL_CUSTOM_NODE_H
#define CHTL_CUSTOM_NODE_H

#include "BaseNode.h"
#include "TemplateNode.h" // For TemplateType enum
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// Represents a [Custom] definition block.
// It's structurally similar to a TemplateNode.
class CustomNode : public BaseNode {
public:
    TemplateType customType; // Re-using TemplateType for Style, Element, Var
    std::string name;
    std::vector<std::shared_ptr<BaseNode>> body;

    std::shared_ptr<BaseNode> clone() const override {
        auto node = std::make_shared<CustomNode>();
        node->customType = this->customType;
        node->name = this->name;
        for (const auto& child : this->body) {
            node->body.push_back(child->clone());
        }
        return node;
    }
};

} // namespace CHTL

#endif // CHTL_CUSTOM_NODE_H
