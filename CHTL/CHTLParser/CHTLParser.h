#pragma once

#include "CHTLLexer/Token.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/ExpressionNode.h"
#include <vector>
#include <memory>
#include <map>

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

    // Token helpers
    Token Peek(size_t offset = 0);
    Token Consume();
    Token Expect(TokenType type);
    void SkipComments();

    // Node Parsers
    NodePtr ParseNode();
    NodePtr ParseElement();
    NodePtr ParseStyleBlock(bool isGlobal);
    NodePtr ParseTextBlock();
    NodePtr ParseTemplateDefinition();
    NodePtr ParseElementTemplateUsage();

    // Expression (Pratt) Parser
    Precedence GetPrecedence();
    ExpressionNodePtr ParseExpression(int precedence = LOWEST);
    ExpressionNodePtr ParsePrefixExpression();
    ExpressionNodePtr ParseInfixExpression(ExpressionNodePtr left);
    ExpressionNodePtr ParseTernaryExpression(ExpressionNodePtr condition);
    ExpressionNodePtr ParseVariableUsage();

    // Element Specialization Parsers
    NodePtr ParseElementModification();
    NodePtr ParseElementDeletion();
    NodePtr ParseElementInsertion();
};

} // namespace CHTL
