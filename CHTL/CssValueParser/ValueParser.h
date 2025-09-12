#ifndef CHTL_VALUE_PARSER_H
#define CHTL_VALUE_PARSER_H

#include "ValueToken.h"
#include "ValueAst.h"
#include <vector>
#include <memory>

class ValueParser {
public:
    ValueParser(const std::vector<ValueToken>& tokens);
    std::shared_ptr<ExprNode> parse();

private:
    // Pratt Parser implementation for handling operator precedence
    std::shared_ptr<ExprNode> parseExpression(int precedence = 0);
    std::shared_ptr<ExprNode> parsePrefix();
    std::shared_ptr<ExprNode> parseInfix(std::shared_ptr<ExprNode> left, ValueToken token);

    int getPrecedence(ValueTokenType type);

    const std::vector<ValueToken>& tokens;
    size_t current = 0;

    ValueToken advance();
    ValueToken peek();
    bool isAtEnd();
};

#endif // CHTL_VALUE_PARSER_H
