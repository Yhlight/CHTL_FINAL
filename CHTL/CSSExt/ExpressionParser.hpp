#pragma once

#include "Expression.hpp"
#include "../CHTLLexer/Token.hpp"
#include <vector>

namespace CHTL {
namespace CSSExt {

class ExpressionParser {
public:
    ExpressionParser(const std::vector<Token>& tokens);
    ExprPtr parse();

private:
    ExprPtr expression();
    ExprPtr addition();
    ExprPtr multiplication();
    ExprPtr power();
    ExprPtr unary();
    ExprPtr primary();

    Token advance();
    Token peek();
    Token previous();
    bool isAtEnd();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);

    const std::vector<Token>& m_tokens;
    int m_current = 0;
};

} // namespace CSSExt
} // namespace CHTL
