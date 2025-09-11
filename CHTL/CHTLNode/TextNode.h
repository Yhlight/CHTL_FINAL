#ifndef CHTL_TEXT_NODE_H
#define CHTL_TEXT_NODE_H

#include "BaseNode.h"
#include <string>

class TextNode : public BaseNode {
public:
    std::string content;
    explicit TextNode(const std::string& text) : content(text) {}
};

#endif // CHTL_TEXT_NODE_H
