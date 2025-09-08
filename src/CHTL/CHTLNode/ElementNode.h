#pragma once

#include "BaseNode.h"
#include "AttributeNode.h"
#include <string>

class ElementNode : public BaseNode {
public:
    ElementNode(const std::string& tagName) : tagName(tagName) {}

    NodeType getType() const override {
        return NodeType::Element;
    }

    std::string tagName;
    AttributeList attributes;
    NodeList children;
};
