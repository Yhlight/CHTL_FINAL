#pragma once

#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLNode/BaseNode.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/CommentNode.h"
#include <vector>
#include <memory>

class Parser {
public:
    Parser(Lexer& lexer);
    NodeList parse();
    const std::vector<std::string>& getErrors() const;

private:
    void nextToken();
    bool currentTokenIs(TokenType type);
    bool peekTokenIs(TokenType type);
    bool expectPeek(TokenType type);
    void peekError(TokenType type);

    NodePtr parseStatement();
    NodePtr parseElement();
    NodePtr parseText();
    NodePtr parseGeneratorComment();

    // This was declared but not defined, which is fine for now as it's not used.
    // void parseAttributes(ElementNode* element);

    Lexer& lexer;
    Token currentToken;
    Token peekToken;

    std::vector<std::string> errors;
};
