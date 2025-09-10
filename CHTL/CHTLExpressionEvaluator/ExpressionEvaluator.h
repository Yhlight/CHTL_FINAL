#ifndef CHTL_EXPRESSION_EVALUATOR_H
#define CHTL_EXPRESSION_EVALUATOR_H

#include "../CHTLExpression/ExprNode.h"
#include "../CHTLExpression/LiteralExprNode.h"
#include "../CHTLExpression/BinaryOpExprNode.h"
#include "../CHTLExpression/UnaryOpExprNode.h"
#include <string>
#include <stdexcept>

// A simple struct to hold a numeric value and its unit.
struct CssValue {
    double value = 0.0;
    std::string unit = "";
};

class ExpressionEvaluator {
public:
    std::string evaluate(ExprNode* root);

private:
    CssValue visit(ExprNode* node);
    CssValue visitLiteral(LiteralExprNode* node);
    CssValue visitBinaryOp(BinaryOpExprNode* node);
    CssValue visitUnaryOp(UnaryOpExprNode* node);
};

#endif // CHTL_EXPRESSION_EVALUATOR_H
