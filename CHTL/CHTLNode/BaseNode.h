#ifndef CHTL_BASENODE_H
#define CHTL_BASENODE_H

#include <string>
#include <memory>
#include <vector>

namespace CHTL
{
    // Forward declarations
    class ElementNode;
    class TextNode;
    class CommentNode;
    class StyleNode;

    enum class NodeType
    {
        PROGRAM,
        ELEMENT_NODE,
        TEXT_NODE,
        COMMENT_NODE,
        STYLE_NODE
    };

    class BaseNode
    {
    public:
        virtual ~BaseNode() = default;
        virtual NodeType getType() const = 0;
        virtual std::string toString(int indent = 0) const = 0;
    };

    // The root of the AST
    class ProgramNode : public BaseNode
    {
    public:
        std::vector<std::unique_ptr<BaseNode>> children;

        NodeType getType() const override { return NodeType::PROGRAM; }
        std::string toString(int indent = 0) const override;
    };
}

#endif // CHTL_BASENODE_H
