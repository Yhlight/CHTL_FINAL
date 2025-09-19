#include "CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../Expression/Expr.h"
#include <iostream>
#include <stdexcept>
#include <set>

namespace CHTL {

CHTLParser::CHTLParser(const std::string& source, const std::vector<Token>& tokens)
    : source(source), tokens(tokens) {}

// --- Expression Parser Implementation ---

std::unique_ptr<Expr> CHTLParser::parseExpression() {
    return parseConditional();
}

std::unique_ptr<Expr> CHTLParser::parseConditional() {
    auto expr = parseLogicalOr();
    if (match({TokenType::QUESTION})) {
        auto then_branch = parseExpression();
        consume(TokenType::COLON, "Expect ':' after then branch of conditional expression.");
        auto else_branch = parseConditional();
        expr = std::make_unique<ConditionalExpr>(std::move(expr), std::move(then_branch), std::move(else_branch));
    }
    return expr;
}

std::unique_ptr<Expr> CHTLParser::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    while (match({TokenType::PIPE_PIPE})) {
        Token op = previous();
        auto right = parseLogicalAnd();
        expr = std::make_unique<LogicalExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> CHTLParser::parseLogicalAnd() {
    auto expr = parseEquality();
    while (match({TokenType::AMPERSAND_AMPERSAND})) {
        Token op = previous();
        auto right = parseEquality();
        expr = std::make_unique<LogicalExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> CHTLParser::parseEquality() {
    auto expr = parseComparison();
    while (match({TokenType::BANG_EQUAL, TokenType::EQUAL_EQUAL})) {
        Token op = previous();
        auto right = parseComparison();
        expr = std::make_unique<ComparisonExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<Expr> CHTLParser::parseComparison() {
    auto expr = parseTerm();
    while (match({TokenType::GREATER, TokenType::GREATER_EQUAL, TokenType::LESS, TokenType::LESS_EQUAL})) {
        Token op = previous();
        auto right = parseTerm();
        expr = std::make_unique<ComparisonExpr>(std::move(expr), op, std::move(right));
    }
    return expr;
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
    // Handle ID and Class selectors for property references, e.g., #main.width or .box.height
    if (check(TokenType::HASH) || check(TokenType::DOT)) {
        Token selector_start = advance(); // Consume '#' or '.'
        Token selector_name = consume(TokenType::IDENTIFIER, "Expect selector name after '#' or '.'.");

        // This could be a reference or just a literal value (like a hex code or class name).
        // A reference *must* be followed by a dot.
        if (check(TokenType::DOT)) {
            consume(TokenType::DOT, "Expect '.' after selector to access a property.");
            Token property = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");

            // Reconstruct the full selector string for the evaluator.
            std::string full_selector_str = selector_start.lexeme + selector_name.lexeme;
            Token full_selector_token = {selector_start.type, full_selector_str, selector_start.line, selector_start.position};

            return std::make_unique<ReferenceExpr>(full_selector_token, property);
        } else {
            // It's not a reference, so treat it as a literal value.
            // This is for cases like `color: #fff` or `font-family: .Helvetica`.
            // The evaluator will treat an unresolved reference as a literal, so we can
            // create a "fake" reference expression here.
            std::string literal_value = selector_start.lexeme + selector_name.lexeme;
            // The property name is the value itself. The selector is empty.
            return std::make_unique<ReferenceExpr>(Token(), Token{TokenType::IDENTIFIER, literal_value, selector_start.line, selector_start.position});
        }
    }

    if (match({TokenType::NUMBER})) {
        Token number = previous();
        std::string unit = "";
        if (check(TokenType::IDENTIFIER)) { // Check for a unit like 'px'
            unit = advance().lexeme;
        } else if (check(TokenType::PERCENT)) { // Check for '%' as a unit
            unit = advance().lexeme;
        }
        try {
            return std::make_unique<LiteralExpr>(std::stod(number.lexeme), unit);
        } catch (const std::invalid_argument& e) {
            error(number, "Invalid number format.");
        }
    }

    if (match({TokenType::IDENTIFIER})) {
        Token first_part = previous();

        if (check(TokenType::DOT)) { // Reference like box.width
            consume(TokenType::DOT, "Expect '.' after selector.");
            Token property = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
            return std::make_unique<ReferenceExpr>(first_part, property);

        } else if (check(TokenType::LEFT_PAREN)) { // Variable usage like Group(key)
            consume(TokenType::LEFT_PAREN, "Expect '(' after variable group name.");

            std::string key_name;
            while (!check(TokenType::RIGHT_PAREN) && !isAtEnd()) {
                key_name += advance().lexeme;
            }

            consume(TokenType::RIGHT_PAREN, "Expect ')' after variable key name.");
            return std::make_unique<VarExpr>(first_part.lexeme, key_name);
        } else {
            // It's an implicit self-reference to a property.
            return std::make_unique<ReferenceExpr>(Token(), first_part);
        }
    }

    if (match({TokenType::STRING})) {
        return std::make_unique<LiteralExpr>(0, previous().lexeme);
    }

    // Handle selectors like #box.width or .container.height, or hex codes like #fff
    if (check(TokenType::SYMBOL) && (peek().lexeme == "#" || peek().lexeme == ".")) {
        Token first_part = advance(); // Consume '#' or '.'

        // To disambiguate, we look ahead for a dot.
        if (tokens[current + 1].type == TokenType::DOT) {
            Token selector_name = consume(TokenType::IDENTIFIER, "Expect selector name.");
            consume(TokenType::DOT, "Expect '.' after selector.");
            Token property = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");

            // Reconstruct the full selector token for the ReferenceExpr
            Token full_selector = {first_part.type, first_part.lexeme + selector_name.lexeme, first_part.line, first_part.position};
            return std::make_unique<ReferenceExpr>(full_selector, property);
        } else {
            // It's not a reference, so treat as a literal (e.g., hex code)
            std::string value = first_part.lexeme;
            while(check(TokenType::IDENTIFIER) || check(TokenType::NUMBER)) {
                value += advance().lexeme;
            }
            return std::make_unique<LiteralExpr>(0, value);
        }
    }

    if (match({TokenType::LEFT_PAREN})) {
        auto expr = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return expr;
    }

    error(peek(), "Expect expression.");
    return nullptr;
}


// --- Document Parser Implementation ---

std::unique_ptr<BaseNode> CHTLParser::parse() {
    // Top-level declarations can be templates or a single root element.
    while (peek().type == TokenType::LEFT_BRACKET) {
        if (tokens.size() > current + 1 && tokens[current + 1].lexeme == "Template") {
            parseTemplateDeclaration();
        } else if (tokens.size() > current + 1 && tokens[current + 1].lexeme == "Origin") {
            // This is likely a global [Origin] block, handle if necessary
            // For now, we assume [Origin] is handled inside elements.
            break;
        } else {
            break;
        }
    }

    if (!isAtEnd() && peek().type != TokenType::END_OF_FILE) {
        // After templates, we expect a single root element declaration.
        auto nodes = parseDeclaration();
        if (nodes.size() == 1) {
            return std::move(nodes[0]);
        } else if (nodes.empty()) {
            // It's possible the file only contained template definitions.
            return nullptr;
        }
        else {
            error(peek(), "Expected a single root element declaration after templates.");
        }
    }

    return nullptr;
}

std::vector<std::unique_ptr<BaseNode>> CHTLParser::parseDeclaration() {
    std::vector<std::unique_ptr<BaseNode>> nodes;

    if (check(TokenType::HASHTAG_COMMENT)) {
        nodes.push_back(parseComment());
    } else if (check(TokenType::TEXT)) {
        advance(); // Consume 'text'
        consume(TokenType::LEFT_BRACE, "Expect '{' after 'text'.");

        std::string text_content;
        if (check(TokenType::STRING)) {
            text_content = advance().lexeme;
        } else {
            // Less greedy version for unquoted text
            static const std::set<TokenType> textLikeTokens = {
                TokenType::IDENTIFIER, TokenType::NUMBER, TokenType::DOT, TokenType::MINUS,
                TokenType::PLUS, TokenType::STAR, TokenType::SLASH, TokenType::PERCENT,
                TokenType::EQUAL, TokenType::COLON, TokenType::QUESTION, TokenType::AMPERSAND,
                TokenType::PIPE, TokenType::LESS, TokenType::GREATER
            };
            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                if (textLikeTokens.count(peek().type)) {
                    if (!text_content.empty()) text_content += " ";
                    text_content += advance().lexeme;
                } else {
                    // Stop if we see a token that's definitely not part of text content
                    break;
                }
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after text block content.");
        nodes.push_back(std::make_unique<TextNode>(text_content));

    } else if (check(TokenType::STYLE)) {
        advance(); // Consume 'style'
        nodes.push_back(parseStyleBlock());
    } else if (check(TokenType::AT)) {
        return parseElementTemplateUsage(); // This returns a vector
    } else if (peek().type == TokenType::LEFT_BRACKET && tokens.size() > current + 1 && tokens[current + 1].lexeme == "Origin") {
        nodes.push_back(parseOriginBlock());
    } else if (check(TokenType::IDENTIFIER)) {
        nodes.push_back(parseElement());
    } else if (!isAtEnd() && !check(TokenType::RIGHT_BRACE)) {
        // Only error if we are not at the end and not about to close a block.
        // This prevents errors on empty blocks.
        error(peek(), "Expect a declaration (element, text, style, comment, or template usage).");
    }

    return nodes;
}

std::unique_ptr<BaseNode> CHTLParser::parseComment() {
    Token commentToken = consume(TokenType::HASHTAG_COMMENT, "Expect comment.");
    return std::make_unique<CommentNode>(commentToken.lexeme);
}

std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    Token tagName = consume(TokenType::IDENTIFIER, "Expect element name.");
    auto element = std::make_unique<ElementNode>(tagName.lexeme);

    consume(TokenType::LEFT_BRACE, "Expect '{' after element name.");

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        // Special case for `text: "value";`
        if (check(TokenType::TEXT) && tokens.size() > current + 1 && tokens[current + 1].type == TokenType::COLON) {
            advance(); // consume 'text'
            consume(TokenType::COLON, "Expect ':' after 'text'.");
            Token value_token;
            if (match({TokenType::STRING, TokenType::IDENTIFIER, TokenType::NUMBER})) {
                value_token = previous();
            } else {
                error(peek(), "Expect a value for text attribute.");
            }
            consume(TokenType::SEMICOLON, "Expect ';' after text value.");
            element->addChild(std::make_unique<TextNode>(value_token.lexeme));
        }
        // Generic attribute parsing
        else if (check(TokenType::IDENTIFIER) && tokens.size() > current + 1 && tokens[current + 1].type == TokenType::COLON) {
            parseAttribute(element.get());
        }
        // Child node parsing
        else {
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
        // Look ahead to see if this is a rule block (contains '{') or an inline property (ends with ';').
        bool isInlineProp = true;
        int i = 0;
        while (tokens.size() > current + i && tokens[current + i].type != TokenType::END_OF_FILE && tokens[current + i].type != TokenType::RIGHT_BRACE) {
            if (tokens[current + i].type == TokenType::LEFT_BRACE) {
                isInlineProp = false; // Found a '{', so it's a rule block.
                break;
            }
            if (tokens[current + i].type == TokenType::SEMICOLON) {
                isInlineProp = true; // Found a ';', so it's an inline property.
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
    } else if (typeToken.lexeme == "Var") {
        def.type = TemplateType::VAR;
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
                AttributeNode attr = {key_str, std::move(value_expr)};
                def.style_properties.push_back(std::move(attr));
            }
        }
    } else if (def.type == TemplateType::ELEMENT) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            for (auto& node : parseDeclaration()) {
                def.element_body.push_back(std::move(node));
            }
        }
    } else if (def.type == TemplateType::VAR) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            std::string key_str;
            while (!check(TokenType::COLON) && !isAtEnd()) {
                key_str += advance().lexeme;
            }
            consume(TokenType::COLON, "Expect ':' after variable name.");
            auto value_expr = parseExpression();
            consume(TokenType::SEMICOLON, "Expect ';' after variable value.");
            def.variables[key_str] = std::move(value_expr);
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' to end template body.");
    template_definitions[def.name] = std::move(def);
}

} // namespace CHTL
