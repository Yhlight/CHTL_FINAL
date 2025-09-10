#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/StyleNode.h"
#include <vector>
#include <memory>

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::unique_ptr<ElementNode> parse();

private:
    const std::vector<Token>& tokens;
    int current = 0;

    // --- Parser Helper Methods ---
    bool isAtEnd();
    Token peek();
    Token peekNext();
    Token previous();
    Token advance();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);

    // --- Grammar Parsing Methods ---
    std::unique_ptr<ElementNode> parseElement();
    void parseChildren(ElementNode* parent);
    void parseAttribute(ElementNode* element);
    std::string parseValue();
    std::unique_ptr<TextNode> parseTextNode();
    std::unique_ptr<CommentNode> parseCommentNode();
    std::unique_ptr<StyleNode> parseStyleNode(ElementNode* parent);
};

#endif // CHTL_PARSER_H
