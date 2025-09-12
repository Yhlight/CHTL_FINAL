#ifndef CHTL_CSS_VALUE_EVALUATOR_H
#define CHTL_CSS_VALUE_EVALUATOR_H

#include "../CHTLLexer/Token.h"
#include <string>
#include <vector>
#include <variant>
#include <map>
#include <optional>

namespace CHTL {

class CssValueEvaluator {
public:
    struct ValueUnit {
        double value;
        std::string unit;
    };

    using PropertyRegistry = std::map<std::string, std::map<std::string, ValueUnit>>;
    using LocalContext = std::map<std::string, ValueUnit>;
    using Value = std::variant<ValueUnit, bool, std::string>;

    CssValueEvaluator() = default;
    std::string evaluate(const std::vector<Token>& tokens, const PropertyRegistry& registry, const LocalContext& local_context);

    static ValueUnit parseValueUnit(const std::string& lexeme);

private:
    // Parser state
    const std::vector<Token>* tokens_ = nullptr;
    size_t current_ = 0;
    const PropertyRegistry* registry_ = nullptr;
    const LocalContext* local_context_ = nullptr;

    // Expression parsing methods (Recursive Descent)
    Value expression();
    Value ternary();
    Value logical();
    Value comparison();
    Value term();
    Value factor();
    Value power();
    Value unary();
    Value primary();

    // Helper methods
    bool isAtEnd() const;
    const Token& peek() const;
    const Token& previous() const;
    const Token& advance();
    bool match(const std::vector<TokenType>& types);
    bool check(TokenType type) const;
    std::string stringify(const Value& value);
    bool isTruthy(const Value& value);
};

} // namespace CHTL

#endif // CHTL_CSS_VALUE_EVALUATOR_H
