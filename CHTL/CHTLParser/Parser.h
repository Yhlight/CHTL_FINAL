#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/Node.h"
#include "../CHTLNode/StatementNodes.h"
#include "../CHTLNode/ExpressionNodes.h"
#include <vector>
#include <string>
#include <memory>

class Parser {
public:
    explicit Parser(Lexer& lexer);

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

    Lexer& lexer;
    Token currentToken;
    Token peekToken;
    std::vector<std::string> errors;
};
