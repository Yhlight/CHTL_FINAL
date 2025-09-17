#pragma once

#include "BaseNode.h"
#include "AttributeNode.h"
#include <string>
#include <vector>
#include <memory>

class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& tagName) : tagName(tagName) {}

    void accept(AstVisitor& visitor) override {
        visitor.visit(*this);
    }

    void addChild(std::shared_ptr<BaseNode> child) {
        children.push_back(child);
    }

    void addAttribute(std::shared_ptr<AttributeNode> attr) {
        attributes.push_back(attr);
    }

    std::string tagName;
    std::vector<std::shared_ptr<AttributeNode>> attributes;
    std::vector<std::shared_ptr<BaseNode>> children;
};
