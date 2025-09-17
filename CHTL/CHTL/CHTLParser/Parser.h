#pragma once

#include "../CHTLLexer/Token.h"
#include "../CHTLNode/RootNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/RuleNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/Expression/ExprNode.h"
#include "../CHTLNode/PropertyNode.h"
#include <vector>
#include <memory>

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    std::shared_ptr<RootNode> parse();

private:
    std::vector<Token> tokens;
    size_t current = 0;

    // Token helpers
    Token advance();
    Token peek() const;
    Token previous() const;
    bool isAtEnd() const;
    bool check(TokenType type) const;
    bool match(const std::vector<TokenType>& types);
    Token consume(TokenType type, const std::string& message);

    // Statement parsers
    std::shared_ptr<BaseNode> parseStatement();
    std::shared_ptr<ElementNode> parseElement();
    void parseBlock(std::shared_ptr<ElementNode> element);
    void parseAttribute(std::shared_ptr<ElementNode> element);
    std::shared_ptr<TextNode> parseTextObject();
    std::shared_ptr<StyleNode> parseStyleBlock();
    std::shared_ptr<RuleNode> parseRule();
    std::shared_ptr<TemplateDefinitionNode> parseTemplateDefinition();
    std::shared_ptr<TemplateUsageNode> parseTemplateUsage();

    // Expression and Property Parsers
    std::shared_ptr<PropertyNode> parseProperty();
    std::shared_ptr<ExprNode> parseExpression();
    std::shared_ptr<ExprNode> parseTerm();
    std::shared_ptr<ExprNode> parseFactor();
    std::shared_ptr<ExprNode> parsePrimary();
};
