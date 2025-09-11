#pragma once

#include "Node.h"
#include "AttributeNode.h"
#include <string>
#include <utility>
#include <vector>
#include <sstream>

namespace CHTL {

class ElementNode : public Node {
public:
    ElementNode(std::string tagName) : tagName(std::move(tagName)) {}

    std::string ToString(int indent = 0) const override {
        std::stringstream ss;
        ss << std::string(indent, ' ') << "Element( " << tagName << " ) [\n";

        if (!attributes.empty()) {
            ss << std::string(indent + 2, ' ') << "Attributes:\n";
            for (const auto& attr : attributes) {
                ss << attr->ToString(indent + 4) << "\n";
            }
        }

        if (!children.empty()) {
            ss << std::string(indent + 2, ' ') << "Children:\n";
            for (const auto& child : children) {
                ss << child->ToString(indent + 4) << "\n";
            }
        }

        ss << std::string(indent, ' ') << "]";
        return ss.str();
    }

    NodePtr clone() const override {
        auto new_element_node = std::make_shared<ElementNode>(tagName);
        for (const auto& attr : attributes) {
            new_element_node->attributes.push_back(std::dynamic_pointer_cast<AttributeNode>(attr->clone()));
        }
        for (const auto& child : children) {
            new_element_node->children.push_back(child->clone());
        }
        return new_element_node;
    }

    std::string tagName;
    AttributeList attributes;
    NodeList children;
};

} // namespace CHTL
