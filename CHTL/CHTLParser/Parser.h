#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/Node.h"
#include "../CHTLNode/ElementNode.h"
#include <vector>
#include <memory>
#include <map>
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLContext.h"

namespace CHTL {

class Parser {
public:
    Parser(Lexer& lexer);

    std::unique_ptr<Node> parseProgram();
    const std::vector<std::string>& getErrors() const { return m_errors; }
    CHTLContext& getContext() { return m_context; }

private:
    void nextToken();
    bool expectPeek(TokenType t);
    void peekError(TokenType t);

    std::unique_ptr<Statement> parseStatement();
    std::unique_ptr<ElementNode> parseElementStatement();
    std::unique_ptr<StyleNode> parseStyleBlock();
    std::unique_ptr<AttributeNode> parseAttribute();
    std::unique_ptr<TemplateNode> parseTemplateStatement();
    std::unique_ptr<Statement> parseTemplateInstantiationStatement();

    Lexer& m_lexer;
    std::vector<std::string> m_errors;

    Token m_currentToken;
    Token m_peekToken;
    CHTLContext m_context;
};

} // namespace CHTL
