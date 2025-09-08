#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/DocumentNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/StyleBlockNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLNode/StyleSelectorNode.h"

#include <vector>
#include <memory>

namespace CHTL {

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens);
    std::unique_ptr<DocumentNode> parse();

private:
    // Main parsing functions
    std::unique_ptr<BaseNode> parseNode();
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<TextNode> parseTextElement();
    void parseAttributes(ElementNode* element);

    // New style-parsing functions
    std::unique_ptr<StyleBlockNode> parseStyleBlock();
    std::unique_ptr<BaseNode> parseStyleContent(); // Dispatcher for property vs selector
    std::unique_ptr<StylePropertyNode> parseStyleProperty();
    std::unique_ptr<StyleSelectorNode> parseStyleSelector();

    // Token manipulation helpers
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
