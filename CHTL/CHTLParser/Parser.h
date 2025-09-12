#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLExpression/ExpressionParser.h"

#include <vector>
#include <memory>

namespace CHTL
{
    class Parser
    {
    public:
        Parser(std::vector<Token> tokens);
        std::unique_ptr<ProgramNode> parse();

    private:
        std::unique_ptr<BaseNode> parseStatement();
        std::unique_ptr<ElementNode> parseElement();
        std::unique_ptr<TextNode> parseText();
        std::unique_ptr<StyleNode> parseStyle();

        Token& currentToken();
        Token& peekToken();
        void advance();
        bool expect(TokenType type);
        void reportError(const std::string& message);

        std::vector<Token> m_tokens;
        size_t m_position;
        bool m_hasError;
    };
}

#endif // CHTL_PARSER_H
