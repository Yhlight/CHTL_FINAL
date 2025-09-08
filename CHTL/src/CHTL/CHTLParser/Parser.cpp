#include "Parser.h"
#include "../CHTLLoader/CHTLLoader.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/StyleBlockNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLNode/StyleSelectorNode.h"
#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/CustomDefinitionNode.h"
#include "../CHTLNode/CustomUsageNode.h"
#include "../CHTLNode/DeleteRuleNode.h"
#include "../CHTLNode/InsertRuleNode.h"
#include "../ExpressionNode/BinaryExpr.h"
#include "../ExpressionNode/TernaryExpr.h"
#include "../ExpressionNode/LiteralExpr.h"
#include "../ExpressionNode/VariableExpr.h"
#include <stdexcept>
#include <string>
#include <initializer_list>
#include <cctype>

namespace CHTL {

Parser::Parser(const std::vector<Token>& tokens, CHTLContext& context, CHTLLoader& loader, std::string current_path)
    : m_tokens(tokens), m_context(context), m_loader(loader), m_current_path(std::move(current_path)), m_current_namespace(CHTLContext::GLOBAL_NAMESPACE) {}

std::unique_ptr<DocumentNode> Parser::parse() {
    auto document = std::make_unique<DocumentNode>();
    while (!isAtEnd()) {
        if (check(TokenType::LeftBracket)) {
            parseTopLevelDefinition();
        } else {
            document->addChild(parseNode());
        }
    }
    return document;
}

void Parser::parseTopLevelDefinition() {
    consume(TokenType::LeftBracket, "Expected '[' to start definition.");
    const Token& keyword = consume(TokenType::Identifier, "Expected 'Template', 'Custom', 'Import', or 'Namespace'.");

    if (keyword.value == "Template") {
        consume(TokenType::RightBracket, "Expected ']' after 'Template'.");
        parseTemplateDefinition();
    } else if (keyword.value == "Custom") {
        consume(TokenType::RightBracket, "Expected ']' after 'Custom'.");
        parseCustomDefinition();
    } else if (keyword.value == "Import") {
        consume(TokenType::RightBracket, "Expected ']' after 'Import'.");
        parseImportDefinition();
    } else if (keyword.value == "Namespace") {
        consume(TokenType::RightBracket, "Expected ']' after 'Namespace'.");
        parseNamespaceDefinition();
    } else {
        throw std::runtime_error("Unknown definition keyword: " + keyword.value);
    }
}

void Parser::parseNamespaceDefinition() {
    const Token& name = consume(TokenType::Identifier, "Expected namespace name.");
    std::string previous_namespace = m_current_namespace;
    m_current_namespace = name.value;
    if (match({TokenType::OpenBrace})) {
        while(!check(TokenType::CloseBrace) && !isAtEnd()) {
            parseTopLevelDefinition();
        }
        consume(TokenType::CloseBrace, "Expected '}' to close namespace block.");
        m_current_namespace = previous_namespace;
    }
}

void Parser::parseImportDefinition() {
    consume(TokenType::At, "Expected '@' for import type.");
    consume(TokenType::Identifier, "Expected import type (e.g., 'Chtl').");
    consume(TokenType::KeywordFrom, "Expected 'from' keyword.");
    const Token& path = consume(TokenType::StringLiteral, "Expected file path string.");
    if(match({TokenType::Semicolon})) {}
    else {
        consume(TokenType::KeywordAs, "Expected 'as' or ';' after import path.");
        consume(TokenType::Identifier, "Expected alias name.");
        consume(TokenType::Semicolon, "Expected ';' after import statement.");
    }
    m_loader.queueImport(m_current_path, path.value);
}

void Parser::parseTemplateDefinition() {
    consume(TokenType::At, "Expected '@' for template type.");
    const Token& type = consume(TokenType::Identifier, "Expected template type.");
    const Token& name = consume(TokenType::Identifier, "Expected template name.");
    auto def = std::make_unique<TemplateDefinitionNode>(type.value, name.value);
    consume(TokenType::OpenBrace, "Expected '{' for template body.");
    if (type.value == "Style") {
        while (!check(TokenType::CloseBrace) && !isAtEnd()) { def->addChild(parseStyleContent()); }
    } else {
        while (!check(TokenType::CloseBrace) && !isAtEnd()) { def->addChild(parseNode()); }
    }
    consume(TokenType::CloseBrace, "Expected '}' to close template body.");
    m_context.registerTemplate(m_current_namespace, std::move(def));
}

void Parser::parseCustomDefinition() {
    consume(TokenType::At, "Expected '@' for custom type.");
    const Token& type = consume(TokenType::Identifier, "Expected custom type.");
    const Token& name = consume(TokenType::Identifier, "Expected custom name.");
    auto def = std::make_unique<CustomDefinitionNode>(type.value, name.value);
    consume(TokenType::OpenBrace, "Expected '{' for custom body.");
    if (type.value == "Style") {
        while (!check(TokenType::CloseBrace) && !isAtEnd()) { def->addChild(parseStyleContent()); }
    } else {
        while (!check(TokenType::CloseBrace) && !isAtEnd()) { def->addChild(parseNode()); }
    }
    consume(TokenType::CloseBrace, "Expected '}' to close custom body.");
    m_context.registerCustom(m_current_namespace, std::move(def));
}

std::unique_ptr<BaseNode> Parser::parseUsage() {
    consume(TokenType::At, "Expected '@' to start usage.");
    const Token& typeOrKeyword = peek();
    if (typeOrKeyword.value == "Custom") {
        advance(); // consume "Custom"
        return parseCustomUsage();
    }
    advance(); // consume "Element" or "Style"

    std::string ns, name;
    const Token& first_id = consume(TokenType::Identifier, "Expected namespace or template name.");
    if (match({TokenType::ColonColon})) {
        ns = first_id.value;
        name = consume(TokenType::Identifier, "Expected template name after '::'.").value;
    } else {
        name = first_id.value;
    }
    consume(TokenType::Semicolon, "Expected ';' after template usage.");
    return std::make_unique<TemplateUsageNode>(typeOrKeyword.value, name, ns);
}

std::unique_ptr<BaseNode> Parser::parseCustomUsage() {
    consume(TokenType::At, "Expected '@' for custom type.");
    const Token& type = consume(TokenType::Identifier, "Expected custom type.");

    std::string ns, name;
    const Token& first_id = consume(TokenType::Identifier, "Expected namespace or custom name.");
    if (match({TokenType::ColonColon})) {
        ns = first_id.value;
        name = consume(TokenType::Identifier, "Expected custom name after '::'.").value;
    } else {
        name = first_id.value;
    }

    auto usageNode = std::make_unique<CustomUsageNode>(type.value, name, ns);
    consume(TokenType::OpenBrace, "Expected '{' for specialization body.");
    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        usageNode->addSpecialization(parseSpecializationRule());
    }
    consume(TokenType::CloseBrace, "Expected '}' to close specialization body.");
    return usageNode;
}

std::unique_ptr<BaseNode> Parser::parseSpecializationRule() {
    if (match({TokenType::KeywordDelete})) {
        std::string target = parseIdentifierSequence();
        consume(TokenType::Semicolon, "Expected ';' after delete rule.");
        return std::make_unique<DeleteRuleNode>(target);
    }
    if (match({TokenType::KeywordInsert})) {
        return parseInsertRule();
    }
    throw std::runtime_error("Unknown or unimplemented specialization rule.");
}

std::unique_ptr<BaseNode> Parser::parseInsertRule() {
    std::string mode;
    const Token& first_word = advance();
    if (first_word.value == "at") {
        const Token& second_word = consume(TokenType::Identifier, "Expected 'top' or 'bottom' after 'at'.");
        mode = first_word.value + " " + second_word.value;
        if (mode != "at top" && mode != "at bottom") { throw std::runtime_error("Unknown insert mode after 'at': " + second_word.value); }
    } else {
        if (first_word.type != TokenType::KeywordAfter && first_word.type != TokenType::KeywordBefore && first_word.type != TokenType::KeywordReplace) { throw std::runtime_error("Expected insert mode keyword (e.g., 'after', 'before', 'replace')."); }
        mode = first_word.value;
    }
    std::string target;
    if (mode != "at top" && mode != "at bottom") {
        while(!check(TokenType::OpenBrace) && !isAtEnd()) { target += advance().value; }
        size_t end = target.find_last_not_of(" \t\n\r");
        if (std::string::npos != end) { target = target.substr(0, end + 1); }
    }
    consume(TokenType::OpenBrace, "Expected '{' for insert rule body.");
    std::vector<std::unique_ptr<BaseNode>> content;
    while(!check(TokenType::CloseBrace) && !isAtEnd()) { content.push_back(parseNode()); }
    consume(TokenType::CloseBrace, "Expected '}' to close insert rule body.");
    return std::make_unique<InsertRuleNode>(mode, target, std::move(content));
}

std::unique_ptr<BaseNode> Parser::parseNode() {
    if (peek().type == TokenType::Identifier) {
        if (peek().value == "text") return parseTextElement();
        return parseElement();
    }
    if (peek().type == TokenType::At) {
        return parseUsage();
    }
    throw std::runtime_error("Unexpected token while parsing a node: " + peek().value);
}

std::unique_ptr<ElementNode> Parser::parseElement() {
    std::string tagName = parseIdentifierSequence();
    auto element = std::make_unique<ElementNode>(tagName);
    consume(TokenType::OpenBrace, "Expected '{' after element tag name.");
    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        if (check(TokenType::Identifier) && peek().value == "style") {
            element->setStyleBlock(std::unique_ptr<StyleBlockNode>(static_cast<StyleBlockNode*>(parseStyleBlock().release())));
        } else if (check(TokenType::At)) {
             element->addChild(parseUsage());
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

std::unique_ptr<BaseNode> Parser::parseStyleBlock() {
    consume(TokenType::Identifier, "Expected 'style' keyword.");
    consume(TokenType::OpenBrace, "Expected '{' after 'style'.");
    auto styleBlock = std::make_unique<StyleBlockNode>();
    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        styleBlock->addChild(parseStyleContent());
    }
    consume(TokenType::CloseBrace, "Expected '}' to close style block.");
    return styleBlock;
}

std::unique_ptr<BaseNode> Parser::parseStyleContent() {
    if (check(TokenType::At)) return parseUsage();
    if (check(TokenType::Dot) || check(TokenType::Hash) || check(TokenType::Ampersand)) return parseStyleSelector();
    if (check(TokenType::Identifier)) {
        size_t lookahead_pos = m_current;
        while(lookahead_pos < m_tokens.size() && (m_tokens[lookahead_pos].type == TokenType::Identifier || m_tokens[lookahead_pos].type == TokenType::Minus)) { lookahead_pos++; }
        if (lookahead_pos < m_tokens.size() && m_tokens[lookahead_pos].type == TokenType::OpenBrace) return parseStyleSelector();
        return parseStyleProperty();
    }
    throw std::runtime_error("Unexpected token in style block: " + peek().value);
}

std::unique_ptr<BaseNode> Parser::parseTextElement() {
    consume(TokenType::Identifier, "Expected 'text' keyword.");
    consume(TokenType::OpenBrace, "Expected '{' after 'text' keyword.");
    const Token& content = advance();
    if (content.type != TokenType::StringLiteral && content.type != TokenType::Identifier) {
        throw std::runtime_error("Expected string or unquoted literal inside text block.");
    }
    auto textNode = std::make_unique<TextNode>(content.value);
    consume(TokenType::CloseBrace, "Expected '}' to close text block.");
    return textNode;
}

void Parser::parseAttributes(ElementNode* element) {
    std::string key = parseIdentifierSequence();
    if (!match({TokenType::Colon, TokenType::Equals})) {
        throw std::runtime_error("Expected ':' or '=' after attribute key.");
    }
    std::string value;
    if(peek().type == TokenType::StringLiteral) {
        value = advance().value;
    } else {
        value = parseIdentifierSequence();
    }
    consume(TokenType::Semicolon, "Expected ';' after attribute value.");
    element->addAttribute(std::make_unique<AttributeNode>(key, value));
}

std::unique_ptr<BaseNode> Parser::parseStyleProperty() {
    std::string key = parseIdentifierSequence();
    consume(TokenType::Colon, "Expected ':' after style property name.");
    auto value = parseExpression();
    consume(TokenType::Semicolon, "Expected ';' after style property value.");
    return std::make_unique<StylePropertyNode>(key, std::move(value));
}

std::unique_ptr<BaseNode> Parser::parseStyleSelector() {
    std::string selector_str;
    while (!check(TokenType::OpenBrace) && !isAtEnd()) {
        selector_str += advance().value;
    }
    auto selectorNode = std::make_unique<StyleSelectorNode>(selector_str);
    consume(TokenType::OpenBrace, "Expected '{' after selector.");
    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        selectorNode->addProperty(std::unique_ptr<StylePropertyNode>(static_cast<StylePropertyNode*>(parseStyleProperty().release())));
    }
    consume(TokenType::CloseBrace, "Expected '}' to close selector block.");
    return selectorNode;
}

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
