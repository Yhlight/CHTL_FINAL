#ifndef CHTL_TEMPLATE_NODE_H
#define CHTL_TEMPLATE_NODE_H

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

enum class TemplateType {
    ELEMENT,
    STYLE,
    VAR
};

// Represents a [Template] @Type Name { ... } block
class TemplateNode : public BaseNode {
public:
    TemplateType type;
    std::string name;
    std::vector<std::unique_ptr<BaseNode>> body;

    TemplateNode(TemplateType t, const std::string& n) : type(t), name(n) {}

    void addChild(std::unique_ptr<BaseNode> child) {
        body.push_back(std::move(child));
    }
};

#endif // CHTL_TEMPLATE_NODE_H
