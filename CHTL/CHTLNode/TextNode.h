#ifndef CHTL_TEXT_NODE_H
#define CHTL_TEXT_NODE_H

#include "BaseNode.h"
#include <string>

// Represents a text literal in the CHTL source.
class TextNode : public BaseNode {
public:
    std::string content;

    // Constructor to initialize the text node with its content.
    explicit TextNode(const std::string& text) : content(text) {}
};

#endif // CHTL_TEXT_NODE_H
