#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>
#include <sstream>

namespace CHTL {

// Represents an element in the CHTL document, like `div {}` or `body {}`.
class ElementNode : public BaseNode {
public:
    std::string tagName;
    // We use a vector of unique_ptr to manage the memory of child nodes automatically.
    // Children can be any type of node that derives from BaseNode.
    std::vector<std::unique_ptr<BaseNode>> children;

    ElementNode(const std::string& tag) : tagName(tag) {}

    std::string toString() const override {
        std::stringstream ss;
        ss << "ElementNode(tag: " << tagName << ", children: [";
        for (size_t i = 0; i < children.size(); ++i) {
            ss << children[i]->toString();
            if (i < children.size() - 1) {
                ss << ", ";
            }
        }
        ss << "])";
        return ss.str();
    }
};

} // namespace CHTL
