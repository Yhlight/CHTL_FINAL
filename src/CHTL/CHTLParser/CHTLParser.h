#pragma once

#include "Token.h"
#include "BaseNode.h"
#include "ElementNode.h"
#include "AttributeNode.h"
#include "TextNode.h"

#include <vector>
#include <memory>
#include <stdexcept>

namespace CHTL {

class CHTLParser {
public:
    explicit CHTLParser(const std::vector<Token>& tokens);

    std::shared_ptr<ElementNode> parse();

    // Error handling
    class ParseError : public std::runtime_error {
    public:
        using std::runtime_error::runtime_error;
    };

private:
    const std::vector<Token>& tokens;
    int current = 0;

    // Parsing methods for different grammar rules
    std::shared_ptr<BaseNode> declaration(std::shared_ptr<ElementNode> rootNode);
    std::shared_ptr<ElementNode> element(std::shared_ptr<ElementNode> rootNode);
    std::shared_ptr<TextNode> textElement();
    void parseStyleBlock(std::shared_ptr<ElementNode> currentNode, std::shared_ptr<ElementNode> rootNode);

    // Token-handling utility methods
    bool isAtEnd();
    Token peek();
    Token previous();
    Token advance();
    bool check(TokenType type);
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);

    ParseError error(const Token& token, const std::string& message);
};

} // namespace CHTL
