#ifndef CHTL_JS_NODE_H
#define CHTL_JS_NODE_H

#include <memory>

namespace CHTLJS {

enum class CHTLJSNodeType {
    Root,
    EnhancedSelector,
    Listen,
    Delegate,
    // Future node types will go here
};

class CHTLJSNode {
public:
    virtual ~CHTLJSNode() = default;
    virtual CHTLJSNodeType getType() const = 0;
    virtual std::unique_ptr<CHTLJSNode> clone() const = 0;
};

} // namespace CHTLJS

#endif // CHTL_JS_NODE_H
