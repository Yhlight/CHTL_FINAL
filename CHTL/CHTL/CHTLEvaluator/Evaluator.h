#pragma once

#include "../CHTLNode/Expression/ExprNode.h"
#include "../CHTLNode/Expression/BinaryExprNode.h"
#include "../CHTLNode/Expression/LiteralExprNode.h"
#include "Value.h"
#include <memory>

// Evaluates style property expressions.
class Evaluator : public ExprVisitor {
public:
    Value evaluate(std::shared_ptr<ExprNode> expr) {
        if (expr) expr->accept(*this);
        return lastValue;
    }

    void visit(BinaryExprNode& expr) override {
        Value left = evaluate(expr.left);
        Value right = evaluate(expr.right);

        if (left.isString || right.isString) {
            // NOTE: This is a simplification for values like '1px solid black'.
            // A more advanced implementation would require a proper CSS value parser.
            std::string op_str = " " + expr.op.value + " ";
            lastValue = Value(left.toString() + op_str + right.toString());
            return;
        }

        // Handle mixed-unit arithmetic with percentages by wrapping in calc().
        // This addresses the user's updated spec.
        if (left.unit != right.unit && !left.unit.empty() && !right.unit.empty()) {
            if (left.unit == "%" || right.unit == "%") {
                std::string calc_expr = "calc(" + left.toString() + " " + expr.op.value + " " + right.toString() + ")";
                lastValue = Value(calc_expr);
                return;
            }
            // As per spec, operations between other different units are an error.
            throw std::runtime_error("Mismatched units in expression: " + left.unit + " and " + right.unit);
        }

        std::string resultUnit = !left.unit.empty() ? left.unit : right.unit;
        double resultNumber = 0.0;

        switch (expr.op.type) {
            case TokenType::PLUS: resultNumber = left.number + right.number; break;
            case TokenType::MINUS: resultNumber = left.number - right.number; break;
            case TokenType::STAR:
                if (!left.unit.empty() && !right.unit.empty()) throw std::runtime_error("Cannot multiply two values with units.");
                resultNumber = left.number * right.number;
                break;
            case TokenType::SLASH:
                if (!right.unit.empty()) throw std::runtime_error("Cannot divide by a value with a unit.");
                if (right.number == 0) throw std::runtime_error("Division by zero.");
                resultNumber = left.number / right.number;
                break;
            default: throw std::runtime_error("Unknown binary operator.");
        }
        lastValue = Value(resultNumber, resultUnit);
    }

    void visit(LiteralExprNode& expr) override {
        // NOTE: The advanced rules for 'Left Association' and 'String Decoupling'
        // from the spec are not yet implemented, as they require a more complex
        // context-aware value parser.
        if (expr.value.type == TokenType::NUMBER_LITERAL) {
            lastValue = Value::fromString(expr.value.value);
        } else {
            lastValue = Value(expr.value.value);
        }
    }

private:
    Value lastValue;
};
