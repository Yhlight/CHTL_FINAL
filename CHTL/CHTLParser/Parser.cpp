#include "Parser.h"
namespace CHTL {
Parser::Parser(Lexer& lexer) : m_lexer(lexer) { next_token(); }
void Parser::next_token() { m_token = m_lexer.next_token(); }
std::unique_ptr<RootNode> Parser::parse_root() {
    auto root = std::make_unique<RootNode>();
    m_root_node = root.get();
    while (m_token.type != TokenType::END_OF_FILE) {
        auto stmt = parse_statement();
        if (stmt) { root->children.push_back(std::move(stmt)); }
    }
    return root;
}
std::unique_ptr<Node> Parser::parse_statement() {
    if (m_token.type != TokenType::IDENTIFIER) { next_token(); return nullptr; }
    if (m_token.literal == "text") return parse_text_statement();
    if (m_token.literal == "style") return parse_style_statement();
    return parse_element_statement();
}
std::unique_ptr<ValueNode> Parser::parse_value() {
    if (m_token.type == TokenType::IDENTIFIER) return std::make_unique<IdentifierNode>(m_token);
    if (m_token.type == TokenType::STRING_LITERAL) return std::make_unique<StringLiteralNode>(m_token);
    return nullptr;
}
std::unique_ptr<AttributeNode> Parser::parse_attribute_statement() {
    auto attr = std::make_unique<AttributeNode>();
    attr->name = m_token.literal;
    next_token();
    if (m_token.type != TokenType::COLON && m_token.type != TokenType::EQUALS) return nullptr;
    next_token();
    attr->value = parse_value();
    next_token();
    if (m_token.type == TokenType::SEMICOLON) next_token();
    return attr;
}
std::unique_ptr<TextNode> Parser::parse_text_statement() {
    next_token();
    if (m_token.type != TokenType::LEFT_BRACE) return nullptr;
    next_token();
    auto text_node = std::make_unique<TextNode>();
    text_node->textValue = parse_value();
    next_token();
    if (m_token.type != TokenType::RIGHT_BRACE) return nullptr;
    next_token();
    return text_node;
}
std::unique_ptr<CssRuleNode> Parser::parse_css_rule_statement() {
    auto rule = std::make_unique<CssRuleNode>();
    std::string selector;
    while(m_token.type == TokenType::DOT || m_token.type == TokenType::HASH || m_token.type == TokenType::AMPERSAND || m_token.type == TokenType::IDENTIFIER || m_token.type == TokenType::COLON) {
        selector += m_token.literal;
        next_token();
    }
    rule->selector = selector;
    if (m_token.type != TokenType::LEFT_BRACE) return nullptr;
    next_token();
    while (m_token.type != TokenType::RIGHT_BRACE && m_token.type != TokenType::END_OF_FILE) {
        rule->properties.push_back(parse_attribute_statement());
    }
    if (m_token.type != TokenType::RIGHT_BRACE) return nullptr;
    next_token();
    return rule;
}
std::unique_ptr<StyleBlockNode> Parser::parse_style_statement() {
    next_token();
    if (m_token.type != TokenType::LEFT_BRACE) return nullptr;
    next_token();
    auto style_node = std::make_unique<StyleBlockNode>();
    while (m_token.type != TokenType::RIGHT_BRACE && m_token.type != TokenType::END_OF_FILE) {
        if (m_token.type == TokenType::DOT || m_token.type == TokenType::HASH || m_token.type == TokenType::AMPERSAND) {
            m_root_node->global_css_rules.push_back(parse_css_rule_statement());
        } else {
            style_node->inline_properties.push_back(parse_attribute_statement());
        }
    }
    if (m_token.type != TokenType::RIGHT_BRACE) return nullptr;
    next_token();
    return style_node;
}
std::unique_ptr<ElementNode> Parser::parse_element_statement() {
    auto el = std::make_unique<ElementNode>();
    el->tagName = m_token.literal;
    next_token();
    if (m_token.type != TokenType::LEFT_BRACE) return nullptr;
    next_token();
    while (m_token.type != TokenType::RIGHT_BRACE && m_token.type != TokenType::END_OF_FILE) {
        if (m_token.literal == "style") { el->styleBlock = parse_style_statement(); }
        else if (m_token.literal == "text") { el->children.push_back(parse_text_statement()); }
        else {
            Lexer temp_lexer = m_lexer; Token peek_token = temp_lexer.next_token();
            if (peek_token.type == TokenType::COLON || peek_token.type == TokenType::EQUALS) {
                el->attributes.push_back(parse_attribute_statement());
            } else { el->children.push_back(parse_element_statement()); }
        }
    }
    if (m_token.type != TokenType::RIGHT_BRACE) return nullptr;
    next_token();
    return el;
}
}
