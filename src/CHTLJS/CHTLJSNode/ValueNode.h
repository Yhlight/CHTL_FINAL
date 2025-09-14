#ifndef CHTL_JS_VALUE_NODE_H
#define CHTL_JS_VALUE_NODE_H

#include "CHTLJSNode.h"
#include <string>
#include <utility>

namespace CHTLJS {

class ValueNode : public CHTLJSNode {
public:
    // Constructor for string literals vs. other JS values (e.g. selectors)
    explicit ValueNode(std::string val, bool is_string_literal)
        : value(std::move(val)), is_string(is_string_literal) {}

    CHTLJSNodeType getType() const override { return CHTLJSNodeType::Value; }

    std::unique_ptr<CHTLJSNode> clone() const override {
        return std::make_unique<ValueNode>(this->value, this->is_string);
    }

    const std::string& getValue() const { return value; }
    bool isStringLiteral() const { return is_string; }

    // Public members for easier access from parser and generator
    std::string value;
    bool is_string;
};

} // namespace CHTLJS

#endif // CHTL_JS_VALUE_NODE_H
