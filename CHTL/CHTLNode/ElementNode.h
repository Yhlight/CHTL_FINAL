#ifndef CHTL_ELEMENTNODE_H
#define CHTL_ELEMENTNODE_H

#include "BaseNode.h"
#include <string>
#include <iostream>

namespace CHTL {

class ElementNode : public BaseNode {
public:
    std::string tagName;
    // Attributes will be added here later
    NodeList children;

    ElementNode(std::string name) : tagName(std::move(name)) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "ElementNode: <" << tagName << ">" << std::endl;
        for (const auto& child : children) {
            child->print(indent + 2);
        }
    }
};

} // namespace CHTL

#endif // CHTL_ELEMENTNODE_H
