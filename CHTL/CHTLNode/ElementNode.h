#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include "ImportNode.h" // Using enums from here

struct Constraint {
    ImportCategory category = ImportCategory::NONE; // e.g., CUSTOM, TEMPLATE
    ImportType type = ImportType::NONE;             // e.g., ELEMENT, STYLE
    std::string name;                               // e.g., "span", "Box"
};

class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::map<std::string, std::string> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;
    std::vector<Constraint> constraints;

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<BaseNode> clone() const override {
        auto node = std::make_unique<ElementNode>();
        node->tagName = this->tagName;
        node->attributes = this->attributes;
        node->constraints = this->constraints; // shallow copy is fine for constraints
        for (const auto& child : this->children) {
            auto clonedChild = child->clone();
            clonedChild->parent = node.get();
            node->children.push_back(std::move(clonedChild));
        }
        return node;
    }
};
