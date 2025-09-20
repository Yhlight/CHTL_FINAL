#ifndef REACTIVE_VALUE_NODE_H
#define REACTIVE_VALUE_NODE_H

#include "../Expression/Expr.h" // Reactive values are a type of expression
#include <string>

namespace CHTL {

class ReactiveValueNode : public Expr {
public:
    std::string variable_name;

    explicit ReactiveValueNode(std::string name)
        : variable_name(std::move(name)) {}

    void accept(ExprVisitor& visitor) override;
    std::unique_ptr<Expr> clone() const override;
};

} // namespace CHTL

#endif // REACTIVE_VALUE_NODE_H
