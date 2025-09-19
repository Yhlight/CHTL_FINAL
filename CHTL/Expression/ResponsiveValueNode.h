#ifndef CHTL_RESPONSIVE_VALUE_NODE_H
#define CHTL_RESPONSIVE_VALUE_NODE_H

#include "Expr.h"
#include <string>

namespace CHTL {

class ResponsiveValueNode : public Expr {
public:
    explicit ResponsiveValueNode(const std::string& var_name) : variable_name(var_name) {}

    void accept(ExprVisitor& visitor) override {
        visitor.visit(*this);
    }

    std::unique_ptr<Expr> clone() const override {
        return std::make_unique<ResponsiveValueNode>(variable_name);
    }

    std::string variable_name;
};

} // namespace CHTL

#endif // CHTL_RESPONSIVE_VALUE_NODE_H
