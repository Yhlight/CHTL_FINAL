#pragma once

#include "CHTLLexer/Token.h"
#include "CHTLNode/Node.h"
#include "CHTLNode/ExprNode.h"
#include <vector>
#include <stdexcept>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    NodeList parse();

    // The exception class should be public so it can be caught externally.
    class ParseError : public std::runtime_error {
    public:
        ParseError(const std::string& message) : std::runtime_error(message) {}
    };

private:
    NodePtr parseDeclaration();
    NodePtr parseElement();
    NodePtr parseProperty();
    NodePtr parseTextNode();
    NodePtr parseStyleNode();
    NodePtr parseScriptNode();

    // Expression parsing methods
    ExprNodePtr parseExpression();
    ExprNodePtr parseTernary();
    ExprNodePtr parseLogicalOr();
    ExprNodePtr parseLogicalAnd();
    ExprNodePtr parseComparison();
    ExprNodePtr parseTerm();
    ExprNodePtr parseFactor();
    ExprNodePtr parsePower();
    ExprNodePtr parseUnary();
    ExprNodePtr parsePrimary();

    // Template and other parsing methods
    NodePtr parseTemplateDefinition();
    NodePtr parseStyleTemplateDefinition();
    NodePtr parseCustomDefinition();
    NodePtr parseElementTemplateDefinition();
    NodePtr parseDelete();
    NodePtr parseInsert();
    NodePtr parseImport();
    NodePtr parseConfiguration();
    NodePtr parseNamespace();
    NodePtr parseUseStatement();
    NodePtr parseConstraint();
    NodePtr parseVarTemplateDefinition();
    NodePtr parseTemplateUsage();
    NodePtr parseComment(const Token& token);

    // Helper methods
    const Token& peek() const;
    const Token& previous() const;
    const Token& advance();
    bool isAtEnd() const;

    bool check(TokenType type) const;
    bool match(const std::vector<TokenType>& types);
    const Token& consume(TokenType type, const std::string& message);
    void skipComments();

    ParseError error(const Token& token, const std::string& message);

    const std::vector<Token>& tokens;
    size_t current = 0;
};
