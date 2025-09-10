#ifndef CHTL_TEXTNODE_H
#define CHTL_TEXTNODE_H

#include "BaseNode.h"
#include <string>

class TextNode : public BaseNode {
public:
    std::string content;

    // Constructor
    explicit TextNode(const Token& token, std::string content)
        : content(std::move(content)) {
        this->token = token;
    }

    std::string debugString(int indent = 0) const override {
        return indentString(indent) + "TextNode: \"" + content + "\"\n";
    }
};

#endif //CHTL_TEXTNODE_H
