#ifndef CHTL_EXPRESSION_EVALUATOR_H
#define CHTL_EXPRESSION_EVALUATOR_H

#include "../CHTLExpression/ExprNode.h"
#include "../CHTLExpression/LiteralExprNode.h"
#include "../CHTLExpression/BinaryOpExprNode.h"
#include "../CHTLExpression/UnaryOpExprNode.h"
#include "../CHTLExpression/ConditionalExprNode.h"
#include <string>
#include <stdexcept>
#include <variant>
#include <unordered_map>

using EvalResult = std::variant<bool, double, std::string>;
struct CssValue {
    EvalResult value;
    std::string unit;
};
using PropertyMap = std::unordered_map<std::string, CssValue>;

class ExpressionEvaluator {
public:
    // The main entry point
    std::string evaluate(ExprNode* root, const PropertyMap& context);
    CssValue visit(ExprNode* node, const PropertyMap& context);

private:
    CssValue visitLiteral(LiteralExprNode* node, const PropertyMap& context);
    CssValue visitBinaryOp(BinaryOpExprNode* node, const PropertyMap& context);
    CssValue visitUnaryOp(UnaryOpExprNode* node, const PropertyMap& context);
    CssValue visitConditional(ConditionalExprNode* node, const PropertyMap& context);
};

#endif // CHTL_EXPRESSION_EVALUATOR_H
