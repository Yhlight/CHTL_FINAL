#pragma once

#include "CHTLLexer/Token.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/ExpressionNode.h"
#include <vector>
#include <memory>

namespace CHTL {

// Precedence levels for operators
enum Precedence {
    LOWEST,
    TERNARY,      // ? :
    LOGICAL_OR,   // ||
    LOGICAL_AND,  // &&
    EQUALS,       // == !=
    LESS_GREATER, // > < >= <=
    SUM,          // + -
    PRODUCT,      // * / %
    POWER,        // **
    PREFIX        // -X or !X
};

class CHTLParser {
public:
    explicit CHTLParser(std::vector<Token> tokens);
    NodeList Parse();

private:
    std::vector<Token> m_tokens;
    size_t m_cursor = 0;

    Token Peek(size_t offset = 0);
    Token Consume();
    Token Expect(TokenType type);

    NodePtr ParseNode();
    NodePtr ParseElement();
    NodePtr ParseStyleBlock();
    NodePtr ParseTextBlock();

    // Pratt Parser for expressions
    Precedence GetPrecedence();
    ExpressionNodePtr ParseExpression(int precedence = 0);
    ExpressionNodePtr ParsePrefixExpression();
    ExpressionNodePtr ParseInfixExpression(ExpressionNodePtr left);
    ExpressionNodePtr ParseTernaryExpression(ExpressionNodePtr condition);

    // Style Block Parsing Helper
    NodePtr ParseStyleBlockContent();

    // Template Parsing
    NodePtr ParseTemplateDefinition();
    NodePtr ParseTemplateUsage();
};

} // namespace CHTL
