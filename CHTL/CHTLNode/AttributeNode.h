#ifndef ATTRIBUTE_NODE_H
#define ATTRIBUTE_NODE_H

#include "../Expression/Expr.h"
#include <string>
#include <memory>
#include <utility>

namespace CHTL {

struct AttributeNode {
    std::string key;
    std::unique_ptr<Expr> value_expr;

    // Default constructor
    AttributeNode() = default;

    // Constructor
    AttributeNode(std::string key, std::unique_ptr<Expr> value_expr)
        : key(std::move(key)), value_expr(std::move(value_expr)) {}

    // Copy constructor
    AttributeNode(const AttributeNode& other)
        : key(other.key), value_expr(other.value_expr ? other.value_expr->clone() : nullptr) {}

    // Copy assignment
    AttributeNode& operator=(const AttributeNode& other) {
        if (this == &other) {
            return *this;
        }
        key = other.key;
        value_expr = other.value_expr ? other.value_expr->clone() : nullptr;
        return *this;
    }

    // Move constructor
    AttributeNode(AttributeNode&& other) noexcept = default;

    // Move assignment
    AttributeNode& operator=(AttributeNode&& other) noexcept = default;


    AttributeNode clone() const {
        return {key, value_expr ? value_expr->clone() : nullptr};
    }
};

} // namespace CHTL

#endif // ATTRIBUTE_NODE_H
