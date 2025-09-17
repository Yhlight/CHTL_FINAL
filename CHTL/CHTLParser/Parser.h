#pragma once

#include "Lexer.h"
#include "Node.h"
#include "ElementNode.h"
#include "TextNode.h"
#include "StyleNode.h"
#include "TemplateDefinitionNode.h"
#include "TemplateUsageNode.h"

#include <memory>
#include <string>
#include <vector>

namespace CHTL {

class Parser {
public:
    Parser(Lexer& lexer);

    std::unique_ptr<Program> ParseProgram();
    const std::vector<std::string>& Errors() const { return m_errors; }

private:
    void nextToken();
    bool currentTokenIs(TokenType t);
    bool peekTokenIs(TokenType t);
    bool expectPeek(TokenType t);

    std::unique_ptr<Node> parseStatement();
    std::unique_ptr<ElementNode> parseElementNode();
    std::unique_ptr<TextNode> parseTextNode();
    std::unique_ptr<StyleNode> parseStyleNode();
    std::unique_ptr<TemplateDefinitionNode> parseTemplateDefinition();
    std::unique_ptr<TemplateUsageNode> parseTemplateUsage();
    void parseAttribute(ElementNode* node);

    void peekError(TokenType t);

    Lexer& m_lexer;
    Token m_currentToken;
    Token m_peekToken;
    std::vector<std::string> m_errors;
};

} // namespace CHTL
