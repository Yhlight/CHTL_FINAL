#ifndef CHTL_CSS_EXPRESSION_EVALUATOR_H
#define CHTL_CSS_EXPRESSION_EVALUATOR_H

#include <vector>
#include <string>
#include "../CHTLLexer/Token.h"

namespace CHTL {

class CssExpressionEvaluator {
public:
    std::string evaluate(const std::vector<Token>& tokens);

private:
    int getPrecedence(TokenType type);
};

} // namespace CHTL

#endif // CHTL_CSS_EXPRESSION_EVALUATOR_H
