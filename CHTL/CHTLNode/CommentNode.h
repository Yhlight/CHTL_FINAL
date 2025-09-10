#ifndef CHTL_COMMENT_NODE_H
#define CHTL_COMMENT_NODE_H

#include "BaseNode.h"
#include <string>

// Represents a generator comment (`--`) that should be output to the final HTML.
class CommentNode : public BaseNode {
public:
    std::string content;

    // Constructor to initialize the comment node with its content.
    explicit CommentNode(const std::string& text) : content(text) {}
};

#endif // CHTL_COMMENT_NODE_H
