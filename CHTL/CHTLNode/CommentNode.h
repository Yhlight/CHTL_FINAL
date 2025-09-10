#ifndef CHTL_COMMENTNODE_H
#define CHTL_COMMENTNODE_H

#include "BaseNode.h"
#include <string>

class CommentNode : public BaseNode {
public:
    std::string content;

    // Constructor
    explicit CommentNode(const Token& token, std::string content)
        : content(std::move(content)) {
        this->token = token;
    }

    std::string debugString(int indent = 0) const override {
        return indentString(indent) + "CommentNode: " + content + "\n";
    }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<CommentNode>(*this);
    }
};

#endif //CHTL_COMMENTNODE_H
