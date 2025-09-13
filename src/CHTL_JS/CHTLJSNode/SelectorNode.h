#ifndef CHTL_JS_SELECTOR_NODE_H
#define CHTL_JS_SELECTOR_NODE_H

#include "CHTLJSBaseNode.h"
#include <string>

namespace CHTL {
namespace CHTL_JS {

class SelectorNode : public CHTLJSNode {
public:
    explicit SelectorNode(const std::string& selector_text) : selector_text_(selector_text) {}

    std::string selector_text_;
};

} // namespace CHTL_JS
} // namespace CHTL

#endif // CHTL_JS_SELECTOR_NODE_H
