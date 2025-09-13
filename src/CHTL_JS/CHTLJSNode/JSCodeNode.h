#ifndef CHTL_JS_JS_CODE_NODE_H
#define CHTL_JS_JS_CODE_NODE_H

#include "CHTLJSBaseNode.h"
#include <string>

namespace CHTL {
namespace CHTL_JS {

class JSCodeNode : public CHTLJSNode {
public:
    explicit JSCodeNode(const std::string& placeholder) : placeholder_(placeholder) {}

    std::string placeholder_;
};

} // namespace CHTL_JS
} // namespace CHTL

#endif // CHTL_JS_JS_CODE_NODE_H
