#ifndef CHTL_EXPRESSION_PARSER_H
#define CHTL_EXPRESSION_PARSER_H

#include "../CHTLLexer/Token.h"
#include "../CHTLExpression/ExprNode.h"
#include "../CHTLExpression/LiteralExprNode.h"
#include "../CHTLExpression/BinaryOpExprNode.h"
#include "../CHTLExpression/UnaryOpExprNode.h"
#include "../CHTLExpression/ConditionalExprNode.h"
#include <vector>
#include <memory>
#include <unordered_map>

class ExpressionParser {
public:
    explicit ExpressionParser(const std::vector<Token>& tokens);
    std::unique_ptr<ExprNode> parse();

private:
    const std::vector<Token>& tokens;
    int current = 0;

    std::unique_ptr<ExprNode> parseExpression(int precedence = 0);
    std::unique_ptr<ExprNode> parsePrefix();

    int getPrecedence(TokenType type);

    Token peek();
    Token peekNext();
    Token advance();
    bool isAtEnd();
};

#endif // CHTL_EXPRESSION_PARSER_H
