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

// Forward declaration
class CHTLContext;

using EvalResult = std::variant<bool, double, std::string>;
struct CssValue {
    EvalResult value;
    std::string unit;
};
using PropertyMap = std::unordered_map<std::string, CssValue>;

class ExpressionEvaluator {
public:
    std::string evaluate(ExprNode* root, const PropertyMap& localContext);
    CssValue visit(ExprNode* node, const PropertyMap& localContext);
private:
    CssValue visitLiteral(LiteralExprNode* node, const PropertyMap& localContext);
    CssValue visitBinaryOp(BinaryOpExprNode* node, const PropertyMap& localContext);
    CssValue visitUnaryOp(UnaryOpExprNode* node, const PropertyMap& localContext);
    CssValue visitConditional(ConditionalExprNode* node, const PropertyMap& localContext);
};

#endif // CHTL_EXPRESSION_EVALUATOR_H
