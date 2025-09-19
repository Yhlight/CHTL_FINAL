#ifndef ENHANCED_SELECTOR_NODE_H
#define ENHANCED_SELECTOR_NODE_H

#include "CHTLJSBaseNode.h"

namespace CHTL_JS {

class EnhancedSelectorNode : public CHTLJSBaseNode {
public:
    explicit EnhancedSelectorNode(const std::string& selector_content)
        : content(selector_content) {
        type = CHTLJSNodeType::EnhancedSelector;
    }

    std::string content;
};

} // namespace CHTL_JS

#endif // ENHANCED_SELECTOR_NODE_H
