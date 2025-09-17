#include "Parser.h"
#include "../CHTLNode/Expression/BinaryExprNode.h"
#include "../CHTLNode/Expression/LiteralExprNode.h"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::shared_ptr<RootNode> Parser::parse() {
    auto root = std::make_shared<RootNode>();
    while (!isAtEnd()) {
        root->addChild(parseStatement());
    }
    return root;
}

Token Parser::advance() { if (!isAtEnd()) current++; return previous(); }
Token Parser::peek() const { return tokens[current]; }
Token Parser::previous() const { return tokens[current - 1]; }
bool Parser::isAtEnd() const { return peek().type == TokenType::END_OF_FILE; }
bool Parser::check(TokenType type) const { if (isAtEnd()) return false; return peek().type == type; }
bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) { advance(); return true; }
    }
    return false;
}
Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message + " at line " + std::to_string(peek().line) + " but got " + tokenTypeToString(peek().type));
}

std::shared_ptr<BaseNode> Parser::parseStatement() {
    if (check(TokenType::L_BRACKET) && tokens[current + 1].type == TokenType::IDENTIFIER && tokens[current + 1].value == "Template" && tokens[current + 2].type == TokenType::R_BRACKET) {
        return parseTemplateDefinition();
    }
    if (check(TokenType::IDENTIFIER) && tokens[current + 1].type == TokenType::LBRACE) {
        return parseElement();
    }
    if (check(TokenType::KEYWORD_TEXT) && tokens[current + 1].type == TokenType::LBRACE) {
        return parseTextObject();
    }
    throw std::runtime_error("Unexpected token at line " + std::to_string(peek().line) + ". Expected element, text block, or template definition.");
}

std::shared_ptr<TemplateDefinitionNode> Parser::parseTemplateDefinition() {
    consume(TokenType::L_BRACKET, "Expected '[' for template definition.");
    consume(TokenType::IDENTIFIER, "Expected 'Template' keyword.");
    consume(TokenType::R_BRACKET, "Expected ']' for template definition.");
    consume(TokenType::AT_SIGN, "Expected '@' for template type.");

    Token typeToken = consume(TokenType::IDENTIFIER, "Expected template type (Style, Element, Var).");
    TemplateType type;
    if (typeToken.value == "Style") type = TemplateType::STYLE;
    else if (typeToken.value == "Element") type = TemplateType::ELEMENT;
    else if (typeToken.value == "Var") type = TemplateType::VAR;
    else throw std::runtime_error("Unknown template type: " + typeToken.value);

    Token nameToken = consume(TokenType::IDENTIFIER, "Expected template name.");
    auto templateNode = std::make_shared<TemplateDefinitionNode>(type, nameToken.value);

    consume(TokenType::LBRACE, "Expected '{' after template name.");

    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (type == TemplateType::STYLE) {
            // A style template can contain properties or other style template usages
            if (check(TokenType::AT_SIGN)) {
                templateNode->addChild(parseTemplateUsage());
            } else {
                Token key = consume(TokenType::IDENTIFIER, "Expected CSS property name.");
                consume(TokenType::COLON, "Expected ':' after CSS property name.");
                auto value = parseExpression();
                templateNode->addChild(std::make_shared<PropertyNode>(key.value, value));
                consume(TokenType::SEMICOLON, "Expected ';' after property value.");
            }
        } else if (type == TemplateType::ELEMENT) {
            templateNode->addChild(parseStatement()); // Element templates can contain other elements, etc.
        }
    }

    consume(TokenType::RBRACE, "Expected '}' to close template definition.");
    return templateNode;
}

std::shared_ptr<TemplateUsageNode> Parser::parseTemplateUsage() {
    consume(TokenType::AT_SIGN, "Expected '@' for template usage.");
    Token typeToken = consume(TokenType::IDENTIFIER, "Expected template type (Style, Element, Var).");
    TemplateType type;
    if (typeToken.value == "Style") type = TemplateType::STYLE;
    else if (typeToken.value == "Element") type = TemplateType::ELEMENT;
    else if (typeToken.value == "Var") type = TemplateType::VAR;
    else throw std::runtime_error("Unknown template type: " + typeToken.value);

    Token nameToken = consume(TokenType::IDENTIFIER, "Expected template name.");
    consume(TokenType::SEMICOLON, "Expected ';' after template usage.");

    return std::make_shared<TemplateUsageNode>(type, nameToken.value);
}

std::shared_ptr<ElementNode> Parser::parseElement() {
    Token tagName = consume(TokenType::IDENTIFIER, "Expected element name.");
    auto element = std::make_shared<ElementNode>(tagName.value);
    consume(TokenType::LBRACE, "Expected '{' after element name.");
    parseBlock(element);
    consume(TokenType::RBRACE, "Expected '}' to close element block.");
    return element;
}

void Parser::parseBlock(std::shared_ptr<ElementNode> element) {
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (check(TokenType::AT_SIGN)) {
            element->addChild(parseTemplateUsage());
        } else if ((check(TokenType::IDENTIFIER) || check(TokenType::KEYWORD_TEXT)) && (tokens[current + 1].type == TokenType::COLON || tokens[current + 1].type == TokenType::EQUALS)) {
            parseAttribute(element);
        } else if (check(TokenType::KEYWORD_STYLE) && tokens[current + 1].type == TokenType::LBRACE) {
            element->addChild(parseStyleBlock());
        } else if (check(TokenType::KEYWORD_TEXT)) {
            element->addChild(parseTextObject());
        } else if (check(TokenType::IDENTIFIER)) {
            element->addChild(parseElement());
        } else {
            throw std::runtime_error("Unexpected token in block at line " + std::to_string(peek().line) + ": " + peek().value);
        }
    }
}

void Parser::parseAttribute(std::shared_ptr<ElementNode> element) {
    Token key = advance();
    if (key.type != TokenType::IDENTIFIER && key.type != TokenType::KEYWORD_TEXT) { throw std::runtime_error("Expected attribute name or 'text' keyword at line " + std::to_string(key.line)); }
    if (!match({TokenType::COLON, TokenType::EQUALS})) { throw std::runtime_error("Expected ':' or '=' after attribute name at line " + std::to_string(peek().line)); }
    Token valueToken = advance();
    std::string value;
    if(valueToken.type == TokenType::STRING_LITERAL || valueToken.type == TokenType::IDENTIFIER || valueToken.type == TokenType::UNQUOTED_LITERAL || valueToken.type == TokenType::NUMBER_LITERAL) { value = valueToken.value; }
    else { throw std::runtime_error("Expected attribute value at line " + std::to_string(valueToken.line)); }
    if (key.type == TokenType::KEYWORD_TEXT) { element->addChild(std::make_shared<TextNode>(value)); }
    else { element->addAttribute(std::make_shared<AttributeNode>(key.value, value)); }
    consume(TokenType::SEMICOLON, "Expected ';' after attribute value.");
}

std::shared_ptr<TextNode> Parser::parseTextObject() {
    consume(TokenType::KEYWORD_TEXT, "Expected 'text' keyword.");
    consume(TokenType::LBRACE, "Expected '{' after 'text' keyword.");
    Token value = advance();
    if (value.type != TokenType::STRING_LITERAL && value.type != TokenType::UNQUOTED_LITERAL && value.type != TokenType::IDENTIFIER) { throw std::runtime_error("Expected a string or unquoted literal inside text block at line " + std::to_string(value.line)); }
    consume(TokenType::RBRACE, "Expected '}' to close text block.");
    return std::make_shared<TextNode>(value.value);
}

std::shared_ptr<StyleNode> Parser::parseStyleBlock() {
    consume(TokenType::KEYWORD_STYLE, "Expected 'style' keyword.");
    consume(TokenType::LBRACE, "Expected '{' after 'style' keyword.");
    auto styleNode = std::make_shared<StyleNode>();
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        if (check(TokenType::AT_SIGN)) {
            styleNode->addChild(parseTemplateUsage());
        } else if (check(TokenType::DOT) || check(TokenType::HASH) || check(TokenType::AMPERSAND) || (check(TokenType::IDENTIFIER) && tokens[current+1].type == TokenType::LBRACE)) {
            styleNode->addChild(parseRule());
        } else {
            Token key = consume(TokenType::IDENTIFIER, "Expected CSS property name or rule selector.");
            consume(TokenType::COLON, "Expected ':' after CSS property name.");
            auto value = parseExpression();
            styleNode->addChild(std::make_shared<PropertyNode>(key.value, value));
            consume(TokenType::SEMICOLON, "Expected ';' after CSS property value.");
        }
    }
    consume(TokenType::RBRACE, "Expected '}' to close style block.");
    return styleNode;
}

std::shared_ptr<RuleNode> Parser::parseRule() {
    std::string selector;
    if (match({TokenType::DOT, TokenType::HASH})) {
        selector += previous().value;
        selector += consume(TokenType::IDENTIFIER, "Expected name after . or #").value;
    } else if (match({TokenType::AMPERSAND})) {
        selector += previous().value;
        if (match({TokenType::COLON})) {
             selector += previous().value;
             selector += consume(TokenType::IDENTIFIER, "Expected pseudo-class name").value;
        }
    } else {
        selector += consume(TokenType::IDENTIFIER, "Expected selector").value;
    }
    auto ruleNode = std::make_shared<RuleNode>(selector);
    consume(TokenType::LBRACE, "Expected '{' after rule selector.");
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        Token key = consume(TokenType::IDENTIFIER, "Expected CSS property name.");
        consume(TokenType::COLON, "Expected ':' after CSS property name.");
        auto value = parseExpression();
        ruleNode->addProperty(std::make_shared<PropertyNode>(key.value, value));
        consume(TokenType::SEMICOLON, "Expected ';' after CSS property value.");
    }
    consume(TokenType::RBRACE, "Expected '}' to close rule block.");
    return ruleNode;
}

std::shared_ptr<ExprNode> Parser::parseExpression() { return parseTerm(); }
std::shared_ptr<ExprNode> Parser::parseTerm() {
    auto expr = parseFactor();
    while (match({TokenType::PLUS, TokenType::MINUS})) {
        Token op = previous();
        auto right = parseFactor();
        expr = std::make_shared<BinaryExprNode>(expr, op, right);
    }
    return expr;
}
std::shared_ptr<ExprNode> Parser::parseFactor() {
    auto expr = parsePrimary();
    while (match({TokenType::STAR, TokenType::SLASH})) {
        Token op = previous();
        auto right = parsePrimary();
        expr = std::make_shared<BinaryExprNode>(expr, op, right);
    }
    return expr;
}
std::shared_ptr<ExprNode> Parser::parsePrimary() {
    if (match({TokenType::NUMBER_LITERAL, TokenType::IDENTIFIER, TokenType::STRING_LITERAL})) { return std::make_shared<LiteralExprNode>(previous()); }
    if (match({TokenType::L_PAREN})) {
        auto expr = parseExpression();
        consume(TokenType::R_PAREN, "Expected ')' after expression.");
        return expr;
    }
    throw std::runtime_error("Expected expression at line " + std::to_string(peek().line));
}
