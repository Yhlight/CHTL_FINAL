#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/ExpressionNode.h"
#include "../CHTLNode/CSSPropertyNode.h"
#include "../CHTLNode/CSSRuleNode.h"
#include <vector>
#include <string>
#include <memory>
#include <map>
#include <functional>

// Forward declare to be used in function pointers
class Parser;

// Define function pointer types for our Pratt parser
using PrefixParseFn = std::function<std::unique_ptr<ExpressionNode>(Parser*)>;
using InfixParseFn = std::function<std::unique_ptr<ExpressionNode>(Parser*, std::unique_ptr<ExpressionNode>)>;

enum Precedence {
    LOWEST,
    EQUALS,      // ==
    LESSGREATER, // > or <
    SUM,         // +
    PRODUCT,     // *
    PREFIX,      // -X or !X
    CALL,        // myFunction(X)
    INDEX        // array[index]
};

class Parser {
public:
    // Constructor takes the output of the lexer
    explicit Parser(std::vector<Token> tokens);

    // Main entry point for parsing. Returns the root of the AST.
    std::vector<std::unique_ptr<BaseNode>> parseProgram();

    // Returns any errors that occurred during parsing.
    const std::vector<std::string>& getErrors() const;

private:
    std::vector<Token> m_tokens;
    size_t m_position;
    Token m_currentToken;
    Token m_peekToken;
    std::vector<std::string> m_errors;

    // --- Pratt Parser Members ---
    std::map<TokenType, PrefixParseFn> m_prefixParseFns;
    std::map<TokenType, InfixParseFn> m_infixParseFns;
    std::map<TokenType, Precedence> m_precedences;

    // --- Helper Methods ---
    void nextToken();
    bool currentTokenIs(TokenType t) const;
    bool peekTokenIs(TokenType t) const;
    bool expectPeek(TokenType t);
    void peekError(TokenType t);
    Precedence currentPrecedence();
    Precedence peekPrecedence();

    // --- Pratt Parser Registration ---
    void registerPrefix(TokenType tokenType, PrefixParseFn fn);
    void registerInfix(TokenType tokenType, InfixParseFn fn);

    // --- Parsing Methods ---
    std::unique_ptr<BaseNode> parseStatement();
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<AttributeNode> parseAttribute();
    std::unique_ptr<TextNode> parseTextElement();
    std::unique_ptr<CommentNode> parseComment();
    std::unique_ptr<BaseNode> parseStyleBlock();
    std::unique_ptr<CSSPropertyNode> parseCSSProperty();
    std::unique_ptr<CSSRuleNode> parseCSSRule();

    // --- Expression Parsing Methods ---
    std::unique_ptr<ExpressionNode> parseExpression(Precedence precedence);
    // Prefix parsing functions
    std::unique_ptr<ExpressionNode> parseIdentifierExpression();
    std::unique_ptr<ExpressionNode> parseNumberLiteralExpression();
    // Infix parsing function
    std::unique_ptr<ExpressionNode> parseInfixExpression(std::unique_ptr<ExpressionNode> left);
};

#endif //CHTL_PARSER_H
