#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/Node.h"
#include "../CHTLNode/StatementNodes.h"
#include "../CHTLNode/ExpressionNodes.h"
#include <vector>
#include <string>
#include <memory>

#include "../CHTLContext/Context.h"

class Parser {
public:
    Parser(Lexer& lexer, Context& context);

    std::shared_ptr<Program> parseProgram();
    const std::vector<std::string>& getErrors() const;

private:
    void nextToken();
    bool currentTokenIs(TokenType type) const;
    bool peekTokenIs(TokenType type) const;
    bool expectPeek(TokenType type);
    void peekError(TokenType type);

    std::shared_ptr<Statement> parseStatement();
    std::shared_ptr<ElementNode> parseElementNode();
    std::shared_ptr<TextNode> parseTextNode();
    std::shared_ptr<StyleNode> parseStyleNode();
    std::shared_ptr<AttributeNode> parseAttributeNode();

    // For now, expressions are simple literals. This can be expanded later.
    std::shared_ptr<Expression> parseExpression();
    std::shared_ptr<TextNode> parseTextAttributeAsNode();
    std::string parseSelector();
    std::shared_ptr<Statement> parseTemplateStatement();
    std::shared_ptr<TemplateUsageNode> parseTemplateUsageNode();
    std::shared_ptr<Expression> parseVarUsageExpression();

    Lexer& lexer;
    Context& context;
    Token currentToken;
    Token peekToken;
    std::vector<std::string> errors;
};
