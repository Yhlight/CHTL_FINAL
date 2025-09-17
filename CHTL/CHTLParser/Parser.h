#ifndef PARSER_H
#define PARSER_H

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/Node.h"
#include <memory>
#include <vector>
#include <string>

namespace CHTL {

class Parser {
public:
    Parser(Lexer& l);

    // The main entry point for parsing. Returns the root of the AST.
    std::unique_ptr<RootNode> parseProgram();

    const std::vector<std::string>& getErrors() const;

private:
    // Advances the token stream.
    void nextToken();

    // The core parsing functions that build the AST.
    // These methods correspond to different rules in the CHTL grammar.
    void parseStatementList(Node* parent);
    std::unique_ptr<Node> parseStatement();
    std::unique_ptr<ElementNode> parseElementStatement();
    void parseAttributes(ElementNode* element);
    std::unique_ptr<TextNode> parseTextStatement();

    // Helper to check the current token type.
    bool currentTokenIs(TokenType t) const;
    // Helper to check the next token type.
    bool peekTokenIs(TokenType t) const;
    // Helper to advance if the next token is as expected.
    bool expectPeek(TokenType t);

    void peekError(TokenType t);

    Lexer& m_lexer;
    Token m_currentToken;
    Token m_peekToken;

    std::vector<std::string> m_errors;
};

} // namespace CHTL

#endif // PARSER_H
