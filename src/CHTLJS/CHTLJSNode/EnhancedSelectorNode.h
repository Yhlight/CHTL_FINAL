#ifndef CHTL_JS_ENHANCED_SELECTOR_NODE_H
#define CHTL_JS_ENHANCED_SELECTOR_NODE_H

#include "CHTLJSNode.h"
#include <string>

namespace CHTLJS {

class EnhancedSelectorNode : public CHTLJSNode {
public:
    explicit EnhancedSelectorNode(std::string selector)
        : CHTLJSNode(CHTLJSNodeType::EnhancedSelector), selector_(std::move(selector)) {}

    const std::string& getSelector() const { return selector_; }

private:
    std::string selector_;
};

} // namespace CHTLJS

#endif // CHTL_JS_ENHANCED_SELECTOR_NODE_H
