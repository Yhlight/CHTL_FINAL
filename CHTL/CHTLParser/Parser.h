#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/CustomNode.h"
#include "../CHTLNode/ExprNode.h"
#include <vector>
#include <memory>

class Parser {
public:
    Parser(Lexer& lexer);
    NodeList parse();

private:
    Lexer& lexer;
    Token previousToken;
    Token currentToken;
    Token peekToken;

    // Core
    void advance();
    Token peek();
    bool match(TokenType type);
    bool check(TokenType type);
    void consume(TokenType type, const std::string& message);

    // Main Parser
    AstNodePtr parseDeclaration();
    AstNodePtr parseElement();
    AstNodePtr parseTextNode();
    AstNodePtr parseCommentNode();
    AstNodePtr parseOriginNode();
    AstNodePtr parseNamespaceNode();
    AstNodePtr parseImportNode();
    AstNodePtr parseTemplateNode();
    AstNodePtr parseTemplateUsageNode();
    AstNodePtr parseCustomNode();
    void parseProperties(std::vector<std::pair<std::string, std::unique_ptr<ExprNode>>>& properties);

    // Expression Parsing (Recursive Descent)
    std::unique_ptr<ExprNode> parseExpression();
    std::unique_ptr<ExprNode> parseTernary();
    std::unique_ptr<ExprNode> parseLogical();
    std::unique_ptr<ExprNode> parseEquality();
    std::unique_ptr<ExprNode> parseComparison();
    std::unique_ptr<ExprNode> parseTerm();
    std::unique_ptr<ExprNode> parseFactor();
    std::unique_ptr<ExprNode> parsePower();
    std::unique_ptr<ExprNode> parseUnary();
    std::unique_ptr<ExprNode> parseMemberAccess();
    std::unique_ptr<ExprNode> parsePrimary();
};
