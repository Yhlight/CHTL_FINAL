#ifndef CHTL_PARSER_H
#define CHTL_PARSER_H

#include "../Lexer/Token.h"
#include "../Node/BaseNode.h"
#include "../Node/RootNode.h"
#include "../Node/ElementNode.h"
#include "../Node/AttributeNode.h"
#include "../Node/TextNode.h"
#include "../Node/StyleNode.h"

#include <vector>
#include <memory>

namespace CHTL {

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::shared_ptr<RootNode> parse();

private:
    std::shared_ptr<BaseNode> parseNode();
    std::shared_ptr<ElementNode> parseElement();
    std::shared_ptr<AttributeNode> parseAttribute();
    std::shared_ptr<TextNode> parseTextBlock();
    std::shared_ptr<StyleNode> parseStyleBlock();

    const Token& currentToken() const;
    const Token& peekToken() const;
    void advance();
    bool match(TokenType type);
    const Token& consume(TokenType type, const std::string& errorMessage);

    std::vector<Token> m_tokens;
    size_t m_current = 0;
};

} // namespace CHTL

#endif // CHTL_PARSER_H
