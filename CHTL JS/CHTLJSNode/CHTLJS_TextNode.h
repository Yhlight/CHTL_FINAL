#ifndef CHTLJS_TEXT_NODE_H
#define CHTLJS_TEXT_NODE_H

#include "CHTLJS_BaseNode.h"
#include <string>

// Represents a block of raw, unprocessed text (JavaScript code).
class CHTLJS_TextNode : public CHTLJS_BaseNode {
public:
    std::string content;

    explicit CHTLJS_TextNode(const std::string& text) : content(text) {}
};

#endif // CHTLJS_TEXT_NODE_H
