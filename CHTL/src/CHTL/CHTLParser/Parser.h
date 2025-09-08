#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLContext/CHTLContext.h" // Include the new context
#include "../CHTLNode/DocumentNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/StyleBlockNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLNode/StyleSelectorNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../ExpressionNode/Expr.h"

#include <vector>
#include <memory>
#include <string>
#include <initializer_list>

namespace CHTL {

class Parser {
public:
    // Constructor now takes a context to store template definitions
    explicit Parser(const std::vector<Token>& tokens, CHTLContext& context);

    std::unique_ptr<DocumentNode> parse();

private:
    // Main parsing functions
    std::unique_ptr<BaseNode> parseNode();
    std::unique_ptr<ElementNode> parseElement();
    std::unique_ptr<TextNode> parseTextElement();
    void parseAttributes(ElementNode* element);
    std::string parseIdentifierSequence();

    // New template parsing functions
    void parseTemplateDefinition();
    std::unique_ptr<TemplateUsageNode> parseTemplateUsage();

    // Style-parsing functions
    std::unique_ptr<StyleBlockNode> parseStyleBlock();
    std::unique_ptr<BaseNode> parseStyleContent();
    std::unique_ptr<StylePropertyNode> parseStyleProperty();
    std::unique_ptr<StyleSelectorNode> parseStyleSelector();

    // Expression-parsing functions
    std::unique_ptr<Expr> parseExpression();
    std::unique_ptr<Expr> parseTernary();
    std::unique_ptr<Expr> parseLogicalOr();
    std::unique_ptr<Expr> parseLogicalAnd();
    std::unique_ptr<Expr> parseComparison();
    std::unique_ptr<Expr> parsePrimary();

    // Token manipulation helpers
    const Token& peek() const;
    const Token& advance();
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(std::initializer_list<TokenType> types);
    const Token& consume(TokenType type, const std::string& message);

    const std::vector<Token>& m_tokens;
    CHTLContext& m_context; // Reference to the shared context
    size_t m_current = 0;
};

} // namespace CHTL
