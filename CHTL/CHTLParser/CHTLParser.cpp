#include "CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../Expression/Expr.h"
#include <iostream>
#include <stdexcept>

namespace CHTL {

CHTLParser::CHTLParser(const std::string& source, const std::vector<Token>& tokens)
    : source(source), tokens(tokens) {}

// --- Expression Parser Implementation ---

std::unique_ptr<Expr> CHTLParser::parseExpression() {
    return parseTerm();
}

std::unique_ptr<Expr> CHTLParser::parseTerm() {
    auto expr = parseFactor();
    while (match({TokenType::MINUS, TokenType::PLUS})) {
        Token op = previous();
        auto right = parseFactor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> CHTLParser::parseFactor() {
    auto expr = parsePower();
    while (match({TokenType::SLASH, TokenType::STAR, TokenType::PERCENT})) {
        Token op = previous();
        auto right = parsePower();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> CHTLParser::parsePower() {
    auto expr = parsePrimary();
    while (match({TokenType::STAR_STAR})) {
        Token op = previous();
        auto right = parsePrimary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> CHTLParser::parsePrimary() {
    if (match({TokenType::NUMBER})) {
        Token number = previous();
        std::string unit = "";
        if (check(TokenType::IDENTIFIER)) { // Check for a unit
            unit = advance().lexeme;
        }
        try {
            return std::make_unique<LiteralExpr>(std::stod(number.lexeme), unit);
        } catch (const std::invalid_argument& e) {
            error(number, "Invalid number format.");
        }
    }

    if (match({TokenType::STRING, TokenType::IDENTIFIER})) {
        return std::make_unique<LiteralExpr>(0, previous().lexeme);
    }

    error(peek(), "Expect expression.");
    return nullptr;
}


// --- Document Parser Implementation ---

std::unique_ptr<BaseNode> CHTLParser::parse() {
    while (peek().type == TokenType::LEFT_BRACKET) {
        if (tokens.size() > current + 1 && tokens[current + 1].lexeme == "Template") {
            parseTemplateDeclaration();
        } else if (tokens.size() > current + 1 && tokens[current + 1].lexeme == "Origin") {
            parseOriginBlock();
        } else {
            break;
        }
    }

    if (!isAtEnd() && peek().type != TokenType::END_OF_FILE) {
        auto nodes = parseDeclaration();
        if (nodes.size() == 1) {
            return std::move(nodes[0]);
        } else {
            error(peek(), "Expected a single root element declaration.");
        }
    }

    return nullptr;
}

std::vector<std::unique_ptr<BaseNode>> CHTLParser::parseDeclaration() {
    if (check(TokenType::AT)) {
        return parseElementTemplateUsage();
    }

    if (peek().type == TokenType::LEFT_BRACKET && tokens.size() > current + 1 && tokens[current + 1].lexeme == "Origin") {
        std::vector<std::unique_ptr<BaseNode>> nodes;
        nodes.push_back(parseOriginBlock());
        return nodes;
    }

    std::vector<std::unique_ptr<BaseNode>> nodes;
    if (match({TokenType::TEXT})) {
        consume(TokenType::LEFT_BRACE, "Expect '{' after 'text'.");
        Token content = consume(TokenType::STRING, "Expect string literal inside text block.");
        consume(TokenType::RIGHT_BRACE, "Expect '}' after text block content.");
        nodes.push_back(std::make_unique<TextNode>(content.lexeme));
        return nodes;
    }

    if (match({TokenType::STYLE})) {
        nodes.push_back(parseStyleBlock());
        return nodes;
    }

    if (check(TokenType::IDENTIFIER)) {
        nodes.push_back(parseElement());
        return nodes;
    }

    error(peek(), "Expect a declaration (element, text, style, or template usage).");
    return {};
}

std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    Token tagName = consume(TokenType::IDENTIFIER, "Expect element name.");
    auto element = std::make_unique<ElementNode>(tagName.lexeme);

    consume(TokenType::LEFT_BRACE, "Expect '{' after element name.");

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (peek().type == TokenType::IDENTIFIER && tokens.size() > current + 1 && tokens[current + 1].type == TokenType::COLON) {
            parseAttribute(element.get());
        } else {
            for (auto& child : parseDeclaration()) {
                element->addChild(std::move(child));
            }
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after element block.");
    return element;
}

void CHTLParser::parseAttribute(ElementNode* element) {
    Token key = consume(TokenType::IDENTIFIER, "Expect attribute name.");
    consume(TokenType::COLON, "Expect ':' after attribute name.");

    Token value_token;
    if (match({TokenType::STRING, TokenType::IDENTIFIER, TokenType::NUMBER})) {
        value_token = previous();
    } else {
        error(peek(), "Expect attribute value (string, identifier, or number).");
    }

    consume(TokenType::SEMICOLON, "Expect ';' after attribute value.");

    element->addAttribute({key.lexeme, value_token.lexeme});
}

std::unique_ptr<StyleNode> CHTLParser::parseStyleBlock() {
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'style' keyword.");
    auto styleNode = std::make_unique<StyleNode>();

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        bool isInlineProp = false;
        int i = 0;
        while (tokens.size() > current + i && tokens[current + i].type != TokenType::END_OF_FILE && tokens[current + i].type != TokenType::RIGHT_BRACE) {
            if (tokens[current + i].type == TokenType::COLON) {
                isInlineProp = true;
                break;
            }
            if (tokens[current + i].type == TokenType::LEFT_BRACE) {
                isInlineProp = false;
                break;
            }
            i++;
        }

        if (check(TokenType::AT)) {
            parseStyleTemplateUsage(styleNode.get());
        } else if (isInlineProp) {
            std::string key_str;
            while (!check(TokenType::COLON) && !isAtEnd()) {
                key_str += advance().lexeme;
            }
            consume(TokenType::COLON, "Expect ':' after style property name.");
            auto value_expr = parseExpression();
            consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
            styleNode->inline_properties.push_back({key_str, std::move(value_expr)});
        } else {
            CssRuleNode rule;
            while (!check(TokenType::LEFT_BRACE) && !isAtEnd()) {
                rule.selector += advance().lexeme;
            }
            consume(TokenType::LEFT_BRACE, "Expect '{' after rule selector.");
            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                std::string key_str;
                while (!check(TokenType::COLON) && !isAtEnd()) {
                    key_str += advance().lexeme;
                }
                consume(TokenType::COLON, "Expect ':' after style property name.");
                auto value_expr = parseExpression();
                consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
                rule.properties.push_back({key_str, std::move(value_expr)});
            }
            consume(TokenType::RIGHT_BRACE, "Expect '}' after rule block.");
            styleNode->global_rules.push_back(std::move(rule));
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after style block.");
    return styleNode;
}

// --- Helper Method Implementations ---

bool CHTLParser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token CHTLParser::peek() {
    return tokens[current];
}

Token CHTLParser::previous() {
    return tokens[current - 1];
}

Token CHTLParser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool CHTLParser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

Token CHTLParser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    error(peek(), message);
    throw std::runtime_error(message);
}

bool CHTLParser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

void CHTLParser::error(const Token& token, const std::string& message) {
    std::cerr << "[line " << token.line << "] Error";
    if (token.type == TokenType::END_OF_FILE) {
        std::cerr << " at end";
    } else {
        std::cerr << " at '" << token.lexeme << "'";
    }
    std::cerr << ": " << message << std::endl;
    throw std::runtime_error(message);
}

std::unique_ptr<BaseNode> CHTLParser::parseOriginBlock() {
    consume(TokenType::LEFT_BRACKET, "Expect '[' to start origin block.");
    Token keyword = consume(TokenType::IDENTIFIER, "Expect 'Origin' keyword.");
    if (keyword.lexeme != "Origin") {
        error(keyword, "Expect 'Origin' keyword.");
    }
    consume(TokenType::RIGHT_BRACKET, "Expect ']' to end origin keyword.");

    consume(TokenType::AT, "Expect '@' for origin type.");
    Token typeToken = consume(TokenType::IDENTIFIER, "Expect origin type (e.g., Html, Style).");

    OriginType type;
    if (typeToken.lexeme == "Html") {
        type = OriginType::HTML;
    } else if (typeToken.lexeme == "Style") {
        type = OriginType::STYLE;
    } else if (typeToken.lexeme == "JavaScript") {
        type = OriginType::JAVASCRIPT;
    } else {
        error(typeToken, "Unknown origin type.");
    }

    Token open_brace = consume(TokenType::LEFT_BRACE, "Expect '{' to start origin body.");

    int brace_depth = 1;
    while (brace_depth > 0 && !isAtEnd()) {
        if (peek().type == TokenType::LEFT_BRACE) brace_depth++;
        else if (peek().type == TokenType::RIGHT_BRACE) brace_depth--;

        if (brace_depth > 0) {
            advance();
        }
    }

    Token close_brace = consume(TokenType::RIGHT_BRACE, "Expect '}' to end origin body.");

    int start_pos = open_brace.position + open_brace.lexeme.length();
    int end_pos = close_brace.position;
    std::string content = source.substr(start_pos, end_pos - start_pos);

    return std::make_unique<OriginNode>(type, content);
}

void CHTLParser::parseStyleTemplateUsage(StyleNode* styleNode) {
    consume(TokenType::AT, "Expect '@' for template usage.");
    Token type = consume(TokenType::IDENTIFIER, "Expect template type.");
    if (type.lexeme != "Style") {
        error(type, "Expect '@Style' template usage here.");
    }
    Token name = consume(TokenType::IDENTIFIER, "Expect template name.");
    consume(TokenType::SEMICOLON, "Expect ';' after template usage.");

    if (template_definitions.count(name.lexeme)) {
        const auto& def = template_definitions.at(name.lexeme);
        if (def.type != TemplateType::STYLE) {
            error(name, "Template '" + name.lexeme + "' is not a Style template.");
        }
        for (const auto& prop : def.style_properties) {
            styleNode->inline_properties.push_back(prop.clone());
        }
    } else {
        error(name, "Style template '" + name.lexeme + "' not found.");
    }
}

std::vector<std::unique_ptr<BaseNode>> CHTLParser::parseElementTemplateUsage() {
    consume(TokenType::AT, "Expect '@' for template usage.");
    Token type = consume(TokenType::IDENTIFIER, "Expect template type.");
    if (type.lexeme != "Element") {
        error(type, "Expect '@Element' template usage here.");
    }
    Token name = consume(TokenType::IDENTIFIER, "Expect template name.");
    consume(TokenType::SEMICOLON, "Expect ';' after template usage.");

    if (template_definitions.count(name.lexeme)) {
        const auto& def = template_definitions.at(name.lexeme);
        if (def.type != TemplateType::ELEMENT) {
            error(name, "Template '" + name.lexeme + "' is not an Element template.");
        }
        std::vector<std::unique_ptr<BaseNode>> cloned_nodes;
        for (const auto& node : def.element_body) {
            cloned_nodes.push_back(node->clone());
        }
        return cloned_nodes;
    } else {
        error(name, "Element template '" + name.lexeme + "' not found.");
    }
    return {};
}

void CHTLParser::parseTemplateDeclaration() {
    consume(TokenType::LEFT_BRACKET, "Expect '[' to start template declaration.");
    Token keyword = consume(TokenType::IDENTIFIER, "Expect 'Template' keyword.");
    if (keyword.lexeme != "Template") {
        error(keyword, "Expect 'Template' keyword in declaration.");
    }
    consume(TokenType::RIGHT_BRACKET, "Expect ']' to end template keyword.");

    consume(TokenType::AT, "Expect '@' for template type.");
    Token typeToken = consume(TokenType::IDENTIFIER, "Expect template type (e.g., Style, Element).");

    TemplateDefinitionNode def;
    if (typeToken.lexeme == "Style") {
        def.type = TemplateType::STYLE;
    } else if (typeToken.lexeme == "Element") {
        def.type = TemplateType::ELEMENT;
    } else {
        error(typeToken, "Unknown template type.");
    }

    def.name = consume(TokenType::IDENTIFIER, "Expect template name.").lexeme;
    consume(TokenType::LEFT_BRACE, "Expect '{' to start template body.");

    if (def.type == TemplateType::STYLE) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            if (check(TokenType::AT) || check(TokenType::INHERIT)) {
                if (check(TokenType::INHERIT)) {
                    advance();
                }
                consume(TokenType::AT, "Expect '@' for template usage.");
                Token type = consume(TokenType::IDENTIFIER, "Expect template type.");
                if (type.lexeme != "Style") {
                    error(type, "Can only inherit from another @Style template here.");
                }
                Token name = consume(TokenType::IDENTIFIER, "Expect template name.");
                consume(TokenType::SEMICOLON, "Expect ';' after template usage.");

                if (template_definitions.count(name.lexeme)) {
                    const auto& base_def = template_definitions.at(name.lexeme);
                    if (base_def.type != TemplateType::STYLE) {
                        error(name, "Template '" + name.lexeme + "' is not a Style template.");
                    }
                    for (const auto& prop : base_def.style_properties) {
                        def.style_properties.push_back(prop.clone());
                    }
                } else {
                    error(name, "Base style template '" + name.lexeme + "' not found.");
                }

            } else {
                std::string key_str;
                while (!check(TokenType::COLON) && !isAtEnd()) {
                    key_str += advance().lexeme;
                }
                consume(TokenType::COLON, "Expect ':' after style property name.");
                auto value_expr = parseExpression();
                consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
                def.style_properties.push_back({key_str, std::move(value_expr)});
            }
        }
    } else if (def.type == TemplateType::ELEMENT) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            for (auto& node : parseDeclaration()) {
                def.element_body.push_back(std::move(node));
            }
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' to end template body.");
    template_definitions[def.name] = std::move(def);
}

} // namespace CHTL
