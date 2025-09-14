#ifndef CHTL_JS_VALUE_NODE_H
#define CHTL_JS_VALUE_NODE_H

#include "CHTLJSNode.h"
#include <string>

namespace CHTL {
namespace CHTLJS {

// A simple node to hold a literal value, used as the result of a `vir` lookup.
class ValueNode : public CHTLJSNode {
public:
    explicit ValueNode(std::string value) : value_(std::move(value)) {}

    CHTLJSNodeType getType() const override { return CHTLJSNodeType::Value; }

    std::unique_ptr<CHTLJSNode> clone() const override {
        return std::make_unique<ValueNode>(value_);
    }

    const std::string& getValue() const { return value_; }

private:
    std::string value_;
};

} // namespace CHTLJS
} // namespace CHTL

#endif // CHTL_JS_VALUE_NODE_H
