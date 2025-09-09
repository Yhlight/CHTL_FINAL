#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/Node.h"
#include <vector>
#include <memory>

namespace CHTL {

class Parser {
public:
    // Takes ownership of the tokens vector.
    Parser(std::vector<Token> tokens);

    // Parses all tokens and returns a vector of root-level AST nodes.
    std::vector<std::unique_ptr<Node>> parse();

private:
    std::vector<Token> m_tokens;
    size_t m_position;

    // Helper methods for token manipulation.
    const Token& peek(size_t offset = 0) const;
    Token consume();
    bool match(TokenType type);
    Token expect(TokenType type);
    void skipComments();

    // Main parsing methods.
    std::unique_ptr<Node> parseStatement();
    std::unique_ptr<ElementNode> parseElement(Token identifier);
    void parseElementBody(ElementNode* element);
    std::unique_ptr<StyleBlockNode> parseStyleBlock();
    std::unique_ptr<CssRuleNode> parseCssRule();
};

} // namespace CHTL
