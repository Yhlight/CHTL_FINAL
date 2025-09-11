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

        // Print Attributes
        if (!attributes.empty()) {
            ss << std::string(indent + 2, ' ') << "Attributes:\n";
            for (const auto& attr : attributes) {
                ss << attr->ToString(indent + 4) << "\n";
            }
        }

        // Print Children
        if (!children.empty()) {
            ss << std::string(indent + 2, ' ') << "Children:\n";
            for (const auto& child : children) {
                ss << child->ToString(indent + 4) << "\n";
            }
        }

        ss << std::string(indent, ' ') << "]";
        return ss.str();
    }

    std::string tagName;
    AttributeList attributes;
    NodeList children;
};

} // namespace CHTL
