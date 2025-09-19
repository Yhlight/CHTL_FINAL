#ifndef CHTL_JS_BASE_NODE_H
#define CHTL_JS_BASE_NODE_H

#include <string>
#include <memory>
#include "Visitor.h"

namespace CHTL_JS {

enum class CHTLJSNodeType {
    RawJS,
    EnhancedSelector,
    Listen,
    EventHandler,
    Delegate,
    Animate
};

class CHTLJSBaseNode {
public:
    virtual ~CHTLJSBaseNode() = default;
    virtual void accept(Visitor& visitor) = 0;
    CHTLJSNodeType type;
};

} // namespace CHTL_JS

#endif // CHTL_JS_BASE_NODE_H
