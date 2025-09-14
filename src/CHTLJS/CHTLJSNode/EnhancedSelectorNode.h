#ifndef CHTL_JS_ENHANCED_SELECTOR_NODE_H
#define CHTL_JS_ENHANCED_SELECTOR_NODE_H

#include "CHTLJSNode.h"
#include <string>

namespace CHTLJS {

class EnhancedSelectorNode : public CHTLJSNode {
public:
    explicit EnhancedSelectorNode(std::string selector) : selector_(std::move(selector)) {}

    CHTLJSNodeType getType() const override { return CHTLJSNodeType::EnhancedSelector; }

    std::unique_ptr<CHTLJSNode> clone() const override {
        return std::make_unique<EnhancedSelectorNode>(selector_);
    }

    const std::string& getSelector() const { return selector_; }
    const std::string& getChainedCode() const { return chained_code_; }
    void setChainedCode(std::string code) { chained_code_ = std::move(code); }

private:
    std::string selector_;
    std::string chained_code_;
};

} // namespace CHTLJS

#endif // CHTL_JS_ENHANCED_SELECTOR_NODE_H
