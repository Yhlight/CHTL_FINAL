#ifndef CHTL_JS_PLACEHOLDER_NODE_H
#define CHTL_JS_PLACEHOLDER_NODE_H

#include "CHTLJSNode.h"
#include <string>

namespace CHTLJS {

class PlaceholderNode : public CHTLJSNode {
public:
    explicit PlaceholderNode(const std::string& placeholder_text)
        : CHTLJSNode(CHTLJSNodeType::Placeholder), placeholder_text_(placeholder_text) {}

    const std::string& getPlaceholderText() const { return placeholder_text_; }

    std::unique_ptr<CHTLJSNode> clone() const override {
        return std::make_unique<PlaceholderNode>(placeholder_text_);
    }

private:
    std::string placeholder_text_;
};

} // namespace CHTLJS

#endif // CHTL_JS_PLACEHOLDER_NODE_H
