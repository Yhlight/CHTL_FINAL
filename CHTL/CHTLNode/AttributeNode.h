#ifndef ATTRIBUTE_NODE_H
#define ATTRIBUTE_NODE_H

#include "../Expression/Expr.h"
#include <string>
#include <memory>

namespace CHTL {

// Represents a single attribute of an HTML element (e.g., id="box")
// or a single CSS property (e.g., width: 100px).
struct AttributeNode {
    std::string key;
    std::unique_ptr<Expr> value_expr;

    // This is not a virtual clone, just a helper for deep copying.
    AttributeNode clone() const {
        AttributeNode new_attr;
        new_attr.key = key;
        if (value_expr) {
            new_attr.value_expr = value_expr->clone();
        }
        return new_attr;
    }
};

} // namespace CHTL

#endif // ATTRIBUTE_NODE_H
