#ifndef CHTL_JS_BASE_NODE_H
#define CHTL_JS_BASE_NODE_H

#include <string>
#include <memory>

namespace CHTL_JS {

enum class CHTLJSNodeType {
    RawJS,
    EnhancedSelector,
    Listen,
    EventHandler,
    Delegate
};

class CHTLJSBaseNode {
public:
    virtual ~CHTLJSBaseNode() = default;
    CHTLJSNodeType type;
};

} // namespace CHTL_JS

#endif // CHTL_JS_BASE_NODE_H
