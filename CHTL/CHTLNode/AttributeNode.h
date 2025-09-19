#ifndef ATTRIBUTE_NODE_H
#define ATTRIBUTE_NODE_H

#include "../Expression/Expr.h"
#include <string>
#include <memory>

namespace CHTL {

struct AttributeNode {
    std::string key;
    std::unique_ptr<Expr> value_expr;

    AttributeNode clone() const {
        return {key, value_expr ? value_expr->clone() : nullptr};
    }
};

} // namespace CHTL

#endif // ATTRIBUTE_NODE_H
