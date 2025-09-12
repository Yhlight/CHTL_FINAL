#pragma once

#include "Node.h"
#include "ExpressionNode.h"
#include <string>
#include <utility>
#include <vector>
#include <sstream>

namespace CHTL {

class CSSPropertyNode : public Node {
public:
    CSSPropertyNode(std::string key, ExpressionPtr value)
        : key(std::move(key)), value(std::move(value)) {}

    std::string ToString(int indent = 0) const override {
        std::stringstream ss;
        ss << std::string(indent, ' ') << "CSSProperty( " << key << " )";
        if (value) {
            ss << " [\n";
            ss << value->ToString(indent + 2) << "\n";
            ss << std::string(indent, ' ') << "]";
        }
        return ss.str();
    }

    NodePtr clone() const override {
        auto value_clone = value ? std::dynamic_pointer_cast<ExpressionNode>(value->clone()) : nullptr;
        return std::make_shared<CSSPropertyNode>(key, value_clone);
    }

    std::string key;
    ExpressionPtr value;
};

using CSSPropertyPtr = std::shared_ptr<CSSPropertyNode>;
using CSSPropertyList = std::vector<CSSPropertyPtr>;

} // namespace CHTL
