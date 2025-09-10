#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/CommentNode.h"
#include <vector>
#include <string>
#include <memory>

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

    // --- Helper Methods ---
    void nextToken();
    bool currentTokenIs(TokenType t) const;
    bool peekTokenIs(TokenType t) const;
    bool expectPeek(TokenType t);

    // --- Parsing Methods ---
    // Each method will parse a part of the language and return an AST node.
    std::unique_ptr<BaseNode> parseStatement();
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<AttributeNode> parseAttribute();
    std::unique_ptr<TextNode> parseTextElement();
    std::unique_ptr<CommentNode> parseComment();

    // Helper to record an error
    void peekError(TokenType t);
};

#endif //CHTL_PARSER_H
