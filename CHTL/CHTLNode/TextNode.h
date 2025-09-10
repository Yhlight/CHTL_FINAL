#ifndef CHTL_TEXTNODE_H
#define CHTL_TEXTNODE_H

#include "BaseNode.h"
#include <string>
#include <iostream>

namespace CHTL {

class TextNode : public BaseNode {
public:
    std::string content;

    TextNode(std::string text) : content(std::move(text)) {}

    void print(int indent = 0) const override {
        std::cout << std::string(indent, ' ') << "TextNode: \"" << content << "\"" << std::endl;
    }
};

} // namespace CHTL

#endif // CHTL_TEXTNODE_H
