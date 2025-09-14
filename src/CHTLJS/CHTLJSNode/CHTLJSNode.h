#ifndef CHTL_JS_NODE_H
#define CHTL_JS_NODE_H

#include <memory>
#include "RouterNode.h"
#include "ScriptLoaderNode.h"

namespace CHTLJS {

enum class CHTLJSNodeType {
    Root,
    EnhancedSelector,
    Listen,
    Delegate,
    Animate,
    Value,
    Router,
    Scriptloader,
    // Future node types will go here
};

class CHTLJSNode {
protected:
    CHTLJSNodeType type_;

public:
    explicit CHTLJSNode(CHTLJSNodeType type) : type_(type) {}
    virtual ~CHTLJSNode() = default;
    CHTLJSNodeType getType() const { return type_; }
    // Making clone pure virtual might be too restrictive if some nodes are not clonable.
    // For now, let's provide a default implementation that throws.
    virtual std::unique_ptr<CHTLJSNode> clone() const {
        throw std::runtime_error("Clone not implemented for this node type.");
    }
};

} // namespace CHTLJS

#endif // CHTL_JS_NODE_H
