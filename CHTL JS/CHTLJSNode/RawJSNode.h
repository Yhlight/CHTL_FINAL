#ifndef RAW_JS_NODE_H
#define RAW_JS_NODE_H

#include "CHTLJSBaseNode.h"

namespace CHTL_JS {

class RawJSNode : public CHTLJSBaseNode {
public:
    explicit RawJSNode(const std::string& js_content)
        : content(js_content) {
        type = CHTLJSNodeType::RawJS;
    }

    std::string content;
};

} // namespace CHTL_JS

#endif // RAW_JS_NODE_H
