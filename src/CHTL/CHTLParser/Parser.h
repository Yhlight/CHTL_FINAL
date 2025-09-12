#pragma once

#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLNode/AST.h"
#include <memory>
#include <vector>
#include <string>

namespace CHTL {

class Parser {
public:
    Parser(Lexer& lexer);

    std::unique_ptr<ProgramNode> parseProgram();
    const std::vector<std::string>& getErrors() const { return m_errors; }

private:
    void nextToken();
    bool currentTokenIs(TokenType type) const;
    bool peekTokenIs(TokenType type) const;
    bool expectPeek(TokenType type);

    std::shared_ptr<Node> parseStatement();
    std::shared_ptr<ElementNode> parseElement();
    std::shared_ptr<AttributeNode> parseAttribute();
    std::shared_ptr<TextNode> parseTextNode();
    std::shared_ptr<CommentNode> parseCommentNode();

    Lexer& m_lexer;
    Token m_curToken;
    Token m_peekToken;

    std::vector<std::string> m_errors;
};

} // namespace CHTL
