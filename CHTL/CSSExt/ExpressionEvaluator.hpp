#pragma once

#include "Expression.hpp"
#include <string>

namespace CHTL {
namespace CSSExt {

// A simple struct to hold a computed value. Can be extended for different types.
struct Value {
    double number;
    std::string unit;
    std::string string_val;
    bool is_number = false;
};

class ExpressionEvaluator : public ExprVisitor {
public:
    Value evaluate(const ExprPtr& expr);

    void visit(Binary& expr) override;
    void visit(Grouping& expr) override;
    void visit(Literal& expr) override;
    void visit(Unary& expr) override;

private:
    Value m_result;
    void checkNumberOperands(const Token& op, const Value& left, const Value& right);
};

} // namespace CSSExt
} // namespace CHTL
