#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/AST.h"
#include "../CHTLContext/Context.h"
#include <vector>
#include <memory>

namespace CHTL {

class Parser {
public:
    explicit Parser(const std::vector<Token>& tokens, CHTLContext& context);

    DocumentNode parse();

private:
    // Parsing methods
    std::unique_ptr<BaseNode> parseStatement();
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<TextNode> parseText();
    std::unique_ptr<StyleBlockNode> parseStyleBlock();
    std::unique_ptr<BaseNode> parseStyleBlockItem();
    std::unique_ptr<StyleTemplateDefinitionNode> parseTemplateDefinition();
    std::unique_ptr<ImportNode> parseImportStatement();
    AttributeNode parseAttribute();
    std::string parseCssValue();

    // Token management helpers
    Token advance();
    Token peek() const;
    Token peekNext() const;
    bool match(TokenType type);
    bool check(TokenType type) const;
    Token consume(TokenType type, const std::string& message);
    bool isAtEnd() const;

    const std::vector<Token>& m_tokens;
    CHTLContext& m_context;
    size_t m_current = 0;
};

} // namespace CHTL
