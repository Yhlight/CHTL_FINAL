#pragma once

#include "../CHTLLexer/Token.hpp"
#include "../CHTLNode/BaseNode.hpp"
#include "../CHTLNode/ElementNode.hpp"
#include "../CHTLNode/AttributeNode.hpp"
#include "../CHTLNode/TextNode.hpp"
#include "../CHTLNode/CommentNode.hpp"
#include <vector>
#include <memory>

namespace CHTL {

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::vector<NodePtr> parse();

private:
    NodePtr declaration();
    NodePtr element();
    NodePtr textBlock();
    std::shared_ptr<AttributeNode> attribute();
    NodePtr styleBlock();
    NodePtr templateDefinition();
    NodePtr customDefinition();
    NodePtr importStatement();
    NodePtr namespaceStatement();
    NodePtr deleteStatement();
    NodePtr insertStatement();

    Token advance();
    Token peek();
    Token previous();
    bool isAtEnd();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);


    const std::vector<Token>& m_tokens;
    int m_current = 0;
};

} // namespace CHTL
