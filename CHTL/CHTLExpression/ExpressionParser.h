#ifndef CHTL_EXPRESSIONPARSER_H
#define CHTL_EXPRESSIONPARSER_H

#include "../CHTLLexer/Token.h"
#include "ExprNode.h"
#include <vector>
#include <memory>

namespace CHTL
{
    class ExpressionParser
    {
    public:
        ExpressionParser(std::vector<Token> tokens);
        std::unique_ptr<ExprNode> parse();

    private:
        std::unique_ptr<ExprNode> parseAddition();
        std::unique_ptr<ExprNode> parseMultiplication();
        std::unique_ptr<ExprNode> parsePower();
        std::unique_ptr<ExprNode> parsePrimary();

        Token& currentToken();
        void advance();

        std::vector<Token> m_tokens;
        size_t m_position;
    };
}

#endif // CHTL_EXPRESSIONPARSER_H
