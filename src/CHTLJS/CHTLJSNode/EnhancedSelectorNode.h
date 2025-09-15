#ifndef CHTL_JS_ENHANCED_SELECTOR_NODE_H
#define CHTL_JS_ENHANCED_SELECTOR_NODE_H

#include "CHTLJSNode.h"
#include <string>
#include <memory>

namespace CHTLJS {

class EnhancedSelectorNode : public CHTLJSNode {
public:
    explicit EnhancedSelectorNode(std::string selector) : selector_(std::move(selector)) {}

    CHTLJSNodeType getType() const override { return CHTLJSNodeType::EnhancedSelector; }

    std::unique_ptr<CHTLJSNode> clone() const override {
        auto node = std::make_unique<EnhancedSelectorNode>(selector_);
        node->trailing_js_placeholder = this->trailing_js_placeholder;
        return node;
    }

    const std::string& getSelector() const { return selector_; }

    std::string trailing_js_placeholder;

private:
    std::string selector_;
};

} // namespace CHTLJS

#endif // CHTL_JS_ENHANCED_SELECTOR_NODE_H
