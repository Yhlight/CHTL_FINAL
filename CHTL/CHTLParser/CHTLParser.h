#pragma once

#include "CHTLLexer/Token.h"
#include "CHTLNode/BaseNode.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/ExpressionNode.h"
#include <vector>
#include <memory>

namespace CHTL {

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

    // Expression Parsing
    ExpressionNodePtr ParseExpression();
    ExpressionNodePtr ParsePrimaryExpression();

    // Style Block Parsing Helper
    NodePtr ParseStyleBlockContent();
};

} // namespace CHTL
