#ifndef CHTL_CSS_VALUE_EVALUATOR_H
#define CHTL_CSS_VALUE_EVALUATOR_H

#include "../CHTLLexer/Token.h"
#include <string>
#include <vector>
#include <variant>

namespace CHTL {

class CssValueEvaluator {
public:
    CssValueEvaluator() = default;
    std::string evaluate(const std::vector<Token>& tokens);

private:
    struct ValueUnit {
        double value;
        std::string unit;
    };

    using RpnToken = std::variant<ValueUnit, TokenType>;

    ValueUnit parseValueUnit(const std::string& lexeme);
    int getPrecedence(TokenType type);
    std::vector<RpnToken> toRPN(const std::vector<Token>& tokens);
    std::string evaluateRPN(std::vector<RpnToken>& rpn);
};

} // namespace CHTL

#endif // CHTL_CSS_VALUE_EVALUATOR_H
