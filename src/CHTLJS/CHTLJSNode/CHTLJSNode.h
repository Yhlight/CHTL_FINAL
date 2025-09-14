#ifndef CHTL_JS_NODE_H
#define CHTL_JS_NODE_H

#include <memory>

namespace CHTLJS {

enum class CHTLJSNodeType {
    Root,
    EnhancedSelector,
    Listen,
    Delegate,
    Animate,
    Value,
    Placeholder,
    // Future node types will go here
};

class CHTLJSNode {
public:
    CHTLJSNode(CHTLJSNodeType type) : type_(type) {}
    virtual ~CHTLJSNode() = default;
    CHTLJSNodeType getType() const { return type_; }
    virtual std::unique_ptr<CHTLJSNode> clone() const = 0;

protected:
    CHTLJSNodeType type_;
};

} // namespace CHTLJS

#endif // CHTL_JS_NODE_H
