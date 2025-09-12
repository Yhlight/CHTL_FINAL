#ifndef CHTL_COMMENTNODE_H
#define CHTL_COMMENTNODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL {

class CommentNode : public BaseNode {
public:
    explicit CommentNode(std::string comment) : comment(std::move(comment)) {}

    void accept(NodeVisitor& visitor) override;

    const std::string& getComment() const { return comment; }

private:
    std::string comment;
};

} // namespace CHTL

#endif // CHTL_COMMENTNODE_H
