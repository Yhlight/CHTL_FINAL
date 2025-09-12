#ifndef CHTL_TEXTNODE_H
#define CHTL_TEXTNODE_H

#include "BaseNode.h"
#include <string>

namespace CHTL
{
    class TextNode : public BaseNode
    {
    public:
        std::string content;

        TextNode(std::string content) : content(std::move(content)) {}

        NodeType getType() const override { return NodeType::TEXT_NODE; }
        std::string toString(int indent = 0) const override;
    };
}

#endif // CHTL_TEXTNODE_H
