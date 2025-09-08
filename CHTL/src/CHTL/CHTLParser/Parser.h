#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/DocumentNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/BaseNode.h"

#include <vector>
#include <memory>

namespace CHTL {

// The Parser class is responsible for turning a flat sequence of tokens
// from the Lexer into a hierarchical Abstract Syntax Tree (AST).
class Parser {
public:
    // The constructor takes the vector of tokens to be parsed.
    explicit Parser(const std::vector<Token>& tokens);

    // The main entry point for parsing. It returns the root of the AST.
    std::unique_ptr<DocumentNode> parse();

private:
    // Core recursive-descent parsing functions.
    std::unique_ptr<BaseNode> parseNode();
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<TextNode> parseTextElement();
    void parseAttributes(ElementNode* element);

    // Helper functions for navigating and consuming the token stream.
    const Token& peek() const;
    const Token& advance();
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(TokenType type);
    const Token& consume(TokenType type, const std::string& message);

    const std::vector<Token>& m_tokens;
    size_t m_current = 0;
};

} // namespace CHTL
