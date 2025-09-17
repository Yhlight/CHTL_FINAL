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

    std::shared_ptr<Node> parseProgram();
    const std::vector<std::string>& getErrors() const { return m_errors; }
    CHTLContext& getContext() { return m_context; }

private:
    void nextToken();
    bool expectPeek(TokenType t);
    void peekError(TokenType t);

    std::shared_ptr<Statement> parseStatement();
    std::shared_ptr<ElementNode> parseElementStatement();
    std::shared_ptr<StyleNode> parseStyleBlock();
    std::shared_ptr<AttributeNode> parseAttribute();
    std::shared_ptr<TemplateNode> parseTemplateStatement();
    std::shared_ptr<Statement> parseTemplateInstantiationStatement();

    Lexer& m_lexer;
    std::vector<std::string> m_errors;

    Token m_currentToken;
    Token m_peekToken;
    CHTLContext m_context;
};

} // namespace CHTL
