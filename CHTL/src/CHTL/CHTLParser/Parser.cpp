#include "Parser.h"
#include "../ExpressionNode/BinaryExpr.h"
#include "../ExpressionNode/TernaryExpr.h"
#include "../ExpressionNode/LiteralExpr.h"
#include "../ExpressionNode/VariableExpr.h"
#include <stdexcept>
#include <string>
#include <initializer_list>
#include <cctype>

namespace CHTL {

Parser::Parser(const std::vector<Token>& tokens, CHTLContext& context)
    : m_tokens(tokens), m_context(context) {}

// --- Main Parser Logic ---

std::unique_ptr<DocumentNode> Parser::parse() {
    auto document = std::make_unique<DocumentNode>();
    while (!isAtEnd()) {
        // At the top level, we can have template definitions or regular nodes.
        if (match({TokenType::LeftBracket})) {
            parseTemplateDefinition();
        } else {
            document->addChild(parseNode());
        }
    }
    return document;
}

void Parser::parseTemplateDefinition() {
    consume(TokenType::Identifier, "Expected 'Template' keyword."); // Assume it's "Template" for now
    consume(TokenType::RightBracket, "Expected ']' after 'Template'.");
    consume(TokenType::At, "Expected '@' for template type.");

    const Token& typeToken = consume(TokenType::Identifier, "Expected template type (Style, Element, Var).");
    const Token& nameToken = consume(TokenType::Identifier, "Expected template name.");

    auto templateNode = std::make_unique<TemplateDefinitionNode>(typeToken.value, nameToken.value);

    consume(TokenType::OpenBrace, "Expected '{' to start template body.");
    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        // The content of the template is just a regular CHTL node
        templateNode->addChild(parseNode());
    }
    consume(TokenType::CloseBrace, "Expected '}' to close template body.");

    m_context.registerTemplate(std::move(templateNode));
}


std::unique_ptr<BaseNode> Parser::parseNode() {
    if (peek().type == TokenType::Identifier) {
        if (peek().value == "text") return parseTextElement();
        return parseElement();
    }
    // A template usage can also be a node
    if (peek().type == TokenType::At) {
        return parseTemplateUsage();
    }
    throw std::runtime_error("Unexpected token while parsing a node: " + peek().value);
}

std::unique_ptr<TemplateUsageNode> Parser::parseTemplateUsage() {
    consume(TokenType::At, "Expected '@' to start template usage.");
    const Token& typeToken = consume(TokenType::Identifier, "Expected template type.");
    const Token& nameToken = consume(TokenType::Identifier, "Expected template name.");
    consume(TokenType::Semicolon, "Expected ';' after template usage.");

    return std::make_unique<TemplateUsageNode>(typeToken.value, nameToken.value);
}

std::unique_ptr<BaseNode> Parser::parseStyleContent() {
    if (match({TokenType::At})) {
        // Backtrack one token so parseTemplateUsage can consume the '@'
        m_current--;
        return parseTemplateUsage();
    }
    if (check(TokenType::Dot) || check(TokenType::Hash) || check(TokenType::Ampersand)) return parseStyleSelector();
    if (check(TokenType::Identifier)) {
        size_t lookahead_pos = m_current;
        while(lookahead_pos < m_tokens.size() && (m_tokens[lookahead_pos].type == TokenType::Identifier || m_tokens[lookahead_pos].type == TokenType::Minus)) { lookahead_pos++; }
        if (lookahead_pos < m_tokens.size() && m_tokens[lookahead_pos].type == TokenType::OpenBrace) return parseStyleSelector();
        return parseStyleProperty();
    }
    throw std::runtime_error("Unexpected token in style block: " + peek().value);
}


// ... (rest of the file is the same, adding placeholders to keep it short)

std::unique_ptr<ElementNode> Parser::parseElement() {
    std::string tagName = parseIdentifierSequence();
    auto element = std::make_unique<ElementNode>(tagName);
    consume(TokenType::OpenBrace, "Expected '{' after element tag name.");
    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        if (check(TokenType::Identifier) && peek().value == "style") {
            element->setStyleBlock(parseStyleBlock());
        } else if (check(TokenType::At)) {
             element->addChild(parseTemplateUsage());
        } else {
            size_t lookahead_pos = m_current;
            while(lookahead_pos < m_tokens.size() && (m_tokens[lookahead_pos].type == TokenType::Identifier || m_tokens[lookahead_pos].type == TokenType::Minus)) { lookahead_pos++; }
            if (lookahead_pos < m_tokens.size() && (m_tokens[lookahead_pos].type == TokenType::Colon || m_tokens[lookahead_pos].type == TokenType::Equals)) {
                parseAttributes(element.get());
            } else {
                element->addChild(parseNode());
            }
        }
    }
    consume(TokenType::CloseBrace, "Expected '}' to close element.");
    return element;
}
std::unique_ptr<TextNode> Parser::parseTextElement() { consume(TokenType::Identifier, "Expected 'text' keyword."); consume(TokenType::OpenBrace, "Expected '{' after 'text' keyword."); const Token& content = advance(); if (content.type != TokenType::StringLiteral && content.type != TokenType::Identifier) { throw std::runtime_error("Expected string or unquoted literal inside text block."); } auto textNode = std::make_unique<TextNode>(content.value); consume(TokenType::CloseBrace, "Expected '}' to close text block."); return textNode; }
void Parser::parseAttributes(ElementNode* element) { std::string key = parseIdentifierSequence(); if (!match({TokenType::Colon, TokenType::Equals})) { throw std::runtime_error("Expected ':' or '=' after attribute key."); } std::string value; if(peek().type == TokenType::StringLiteral) { value = advance().value; } else { value = parseIdentifierSequence(); } consume(TokenType::Semicolon, "Expected ';' after attribute value."); element->addAttribute(std::make_unique<AttributeNode>(key, value)); }
std::unique_ptr<StyleBlockNode> Parser::parseStyleBlock() { consume(TokenType::Identifier, "Expected 'style' keyword."); consume(TokenType::OpenBrace, "Expected '{' after 'style'."); auto styleBlock = std::make_unique<StyleBlockNode>(); while (!check(TokenType::CloseBrace) && !isAtEnd()) { styleBlock->addChild(parseStyleContent()); } consume(TokenType::CloseBrace, "Expected '}' to close style block."); return styleBlock; }
std::unique_ptr<StylePropertyNode> Parser::parseStyleProperty() { std::string key = parseIdentifierSequence(); consume(TokenType::Colon, "Expected ':' after style property name."); auto value = parseExpression(); consume(TokenType::Semicolon, "Expected ';' after style property value."); return std::make_unique<StylePropertyNode>(key, std::move(value)); }
std::unique_ptr<StyleSelectorNode> Parser::parseStyleSelector() { std::string selector_str; while (!check(TokenType::OpenBrace) && !isAtEnd()) { selector_str += advance().value; } auto selectorNode = std::make_unique<StyleSelectorNode>(selector_str); consume(TokenType::OpenBrace, "Expected '{' after selector."); while (!check(TokenType::CloseBrace) && !isAtEnd()) { selectorNode->addProperty(parseStyleProperty()); } consume(TokenType::CloseBrace, "Expected '}' to close selector block."); return selectorNode; }
std::unique_ptr<Expr> Parser::parseExpression() { return parseTernary(); }
std::unique_ptr<Expr> Parser::parseTernary() { auto expr = parseLogicalOr(); if (match({TokenType::QuestionMark})) { auto thenBranch = parseTernary(); consume(TokenType::Colon, "Expected ':' in ternary expression."); auto elseBranch = parseTernary(); expr = std::make_unique<TernaryExpr>(std::move(expr), std::move(thenBranch), std::move(elseBranch)); } return expr; }
std::unique_ptr<Expr> Parser::parseLogicalOr() { auto expr = parseLogicalAnd(); while (match({TokenType::LogicalOr})) { Token op = m_tokens[m_current - 1]; auto right = parseLogicalAnd(); expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right)); } return expr; }
std::unique_ptr<Expr> Parser::parseLogicalAnd() { auto expr = parseComparison(); while (match({TokenType::LogicalAnd})) { Token op = m_tokens[m_current - 1]; auto right = parseComparison(); expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right)); } return expr; }
std::unique_ptr<Expr> Parser::parseComparison() { auto expr = parsePrimary(); while (match({TokenType::GreaterThan, TokenType::LessThan})) { Token op = m_tokens[m_current - 1]; auto right = parsePrimary(); expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right)); } return expr; }
std::unique_ptr<Expr> Parser::parsePrimary() { if (peek().type == TokenType::StringLiteral) { return std::make_unique<LiteralExpr>(advance()); } if (peek().type == TokenType::Identifier) { const Token& token = peek(); if (!token.value.empty() && std::isdigit(token.value[0])) { return std::make_unique<LiteralExpr>(advance()); } return std::make_unique<VariableExpr>(advance()); } throw std::runtime_error("Expected primary expression."); }
std::string Parser::parseIdentifierSequence() { std::string result = consume(TokenType::Identifier, "Expected an identifier.").value; while (match({TokenType::Minus})) { result += "-" + consume(TokenType::Identifier, "Expected identifier after '-'.").value; } return result; }
const Token& Parser::peek() const { return m_tokens[m_current]; }
const Token& Parser::advance() { if (!isAtEnd()) m_current++; return m_tokens[m_current - 1]; }
bool Parser::isAtEnd() const { return peek().type == TokenType::EndOfFile; }
bool Parser::check(TokenType type) const { if (isAtEnd()) return false; return peek().type == type; }
bool Parser::match(std::initializer_list<TokenType> types) { for (TokenType type : types) { if (check(type)) { advance(); return true; } } return false; }
const Token& Parser::consume(TokenType type, const std::string& message) { if (check(type)) return advance(); throw std::runtime_error(message + " Found token '" + peek().value + "' instead."); }

} // namespace CHTL
