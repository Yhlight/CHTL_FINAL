#ifndef CHTL_ELEMENTNODE_H
#define CHTL_ELEMENTNODE_H

#include "BaseNode.h"
#include "AttributeNode.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

class ElementNode : public BaseNode {
public:
    std::string tagName;
    std::vector<std::unique_ptr<AttributeNode>> attributes;
    std::vector<std::unique_ptr<BaseNode>> children;

    // Constructor
    explicit ElementNode(const Token& token, std::string tagName)
        : tagName(std::move(tagName)) {
        this->token = token;
    }

    void addAttribute(std::unique_ptr<AttributeNode> attribute) {
        attributes.push_back(std::move(attribute));
    }

    void addChild(std::unique_ptr<BaseNode> child) {
        children.push_back(std::move(child));
    }

    std::string debugString(int indent = 0) const override {
        std::stringstream ss;
        ss << indentString(indent) << "ElementNode: <" << tagName << ">\n";

        // Print attributes
        for (const auto& attr : attributes) {
            ss << attr->debugString(indent + 1);
        }

        // Print children
        for (const auto& child : children) {
            ss << child->debugString(indent + 1);
        }

        return ss.str();
    }
};

#endif //CHTL_ELEMENTNODE_H
