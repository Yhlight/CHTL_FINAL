#ifndef BASE_NODE_H
#define BASE_NODE_H

#include <memory>
#include <vector>

namespace CHTL {

enum class NodeType {
    Element,
    Text,
    Comment,
    Style,
    TemplateUsage,
    Insert,
    Delete
};

class BaseNode {
public:
    virtual ~BaseNode() = default;
    virtual NodeType getType() const = 0;

    // Added clone method for template reusability
    virtual std::unique_ptr<BaseNode> clone() const = 0;

    void addChild(std::unique_ptr<BaseNode> child);
    const std::vector<std::unique_ptr<BaseNode>>& getChildren() const;

protected:
    std::vector<std::unique_ptr<BaseNode>> m_children;
};

} // namespace CHTL

#endif // BASE_NODE_H
