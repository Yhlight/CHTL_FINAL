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
    std::vector<std::shared_ptr<BaseNode>> body;

    std::shared_ptr<BaseNode> clone() const override {
        auto node = std::make_shared<TemplateNode>();
        node->templateType = this->templateType;
        node->name = this->name;
        for (const auto& child : this->body) {
            node->body.push_back(child->clone());
        }
        return node;
    }
};

} // namespace CHTL

#endif // CHTL_TEMPLATE_NODE_H
