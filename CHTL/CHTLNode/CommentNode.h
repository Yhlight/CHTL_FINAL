#ifndef CHTL_COMMENTNODE_H
#define CHTL_COMMENTNODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL
{
    enum class ChtlCommentType {
        Regular,      // //
        MultiLine,    // /* */
        Generator     // --
    };

    class CommentNode : public BaseNode
    {
    public:
        std::string content;
        ChtlCommentType commentType;

        CommentNode(std::string content, ChtlCommentType type)
            : content(std::move(content)), commentType(type) {}

        NodeType getType() const override { return NodeType::COMMENT_NODE; }
        std::string toString(int indent = 0) const override;
    };
}

#endif // CHTL_COMMENTNODE_H
