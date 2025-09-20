#include "CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/TemplateDeclarationNode.h"
#include "../CHTLNode/ImportNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../Expression/Expr.h"
#include <iostream>
#include <stdexcept>
#include <algorithm> // For std::remove_if

namespace CHTL {

CHTLParser::CHTLParser(const std::string& source, const std::vector<Token>& tokens, std::map<std::string, TemplateDefinitionNode>& template_definitions)
    : source(source), tokens(tokens), template_definitions(template_definitions) {}

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
    if (match({TokenType::NUMBER})) {
        Token number = previous();
        std::string unit = "";
        if (check(TokenType::IDENTIFIER)) { // Check for a unit like 'px'
            unit = advance().lexeme;
        } else if (check(TokenType::PERCENT)) { // Check for '%' as a unit
            unit = advance().lexeme;
        }
        try {
            // Use the numeric constructor for LiteralExpr
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
        // Use the string constructor for LiteralExpr
        return std::make_unique<LiteralExpr>(previous().lexeme);
    }

    // Handle selectors like #box.width or .container.height, or hex codes like #fff
    if (check(TokenType::SYMBOL) && (peek().lexeme == "#" || peek().lexeme == ".")) {
        Token first_part = advance(); // Consume '#' or '.'

        // To disambiguate, we look ahead for a dot.
        if (tokens.size() > current + 1 && tokens[current + 1].type == TokenType::DOT) {
            Token selector_name = consume(TokenType::IDENTIFIER, "Expect selector name.");
            consume(TokenType::DOT, "Expect '.' after selector.");
            Token property = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");

            // Reconstruct the full selector token for the ReferenceExpr
            Token full_selector = {first_part.type, first_part.lexeme + selector_name.lexeme, first_part.line, first_part.position};
            return std::make_unique<ReferenceExpr>(full_selector, property);
        } else {
            // It's not a reference, so treat as a literal (e.g., hex code or unquoted string)
            std::string value = first_part.lexeme;
            while(check(TokenType::IDENTIFIER) || check(TokenType::NUMBER)) {
                value += advance().lexeme;
            }
            return std::make_unique<LiteralExpr>(value);
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
    auto root = std::make_unique<ElementNode>("<root>");
    while (!isAtEnd()) {
        root->addChild(parseTopLevelDeclaration());
    }
    return root;
}

std::unique_ptr<BaseNode> CHTLParser::parseTopLevelDeclaration() {
    if (check(TokenType::LEFT_BRACKET)) {
        // Look ahead to see what kind of block it is.
        if (tokens.size() > current + 1) {
            TokenType keywordType = tokens[current + 1].type;
            switch(keywordType) {
                case TokenType::TEMPLATE:
                    return parseTemplateDeclaration();
                case TokenType::CUSTOM:
                    return parseCustomDeclaration();
                case TokenType::ORIGIN:
                    return parseOriginBlock();
                case TokenType::IMPORT:
                    return parseImportStatement();
                default:
                    // If it's not a recognized block keyword, parse as a normal element.
                    break;
            }
        }
    }

    // If it's not a special block, assume it's a standard element declaration
    auto nodes = parseDeclaration();
    if (nodes.size() == 1) {
        return std::move(nodes[0]);
    } else {
        error(peek(), "Expected a single top-level declaration.");
        return nullptr; // Should not be reached due to error throw
    }
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

    if (match({TokenType::SCRIPT})) {
        nodes.push_back(parseScriptBlock());
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
    consume(TokenType::STYLE, "Expect 'Style' keyword for style template usage.");
    Token name = consume(TokenType::IDENTIFIER, "Expect template name.");

    if (!template_definitions.count(name.lexeme)) {
        error(name, "Style template '" + name.lexeme + "' not found.");
        return;
    }

    const auto& def = template_definitions.at(name.lexeme);
    if (def.type != TemplateType::STYLE) {
        error(name, "Template '" + name.lexeme + "' is not a Style template.");
    }

    // Clone the base properties. We will modify this list.
    std::vector<AttributeNode> final_properties;
    for (const auto& prop : def.style_properties) {
        final_properties.push_back(prop.clone());
    }

    if (match({TokenType::LEFT_BRACE})) { // Specialization block
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            if (match({TokenType::DELETE})) {
                // Handle 'delete prop1, prop2;'
                do {
                    std::string prop_key_to_delete;
                    while(!check(TokenType::COMMA) && !check(TokenType::SEMICOLON) && !isAtEnd()) {
                        prop_key_to_delete += advance().lexeme;
                    }

                    final_properties.erase(
                        std::remove_if(final_properties.begin(), final_properties.end(),
                            [&](const AttributeNode& attr) { return attr.key == prop_key_to_delete; }),
                        final_properties.end()
                    );
                } while (match({TokenType::COMMA}));
                consume(TokenType::SEMICOLON, "Expect ';' after delete statement.");
            } else {
                // Handle 'prop: value;' to fill in placeholders
                std::string key_str;
                while (!check(TokenType::COLON) && !isAtEnd()) {
                    key_str += advance().lexeme;
                }
                consume(TokenType::COLON, "Expect ':' after property name.");
                auto value_expr = parseExpression();
                consume(TokenType::SEMICOLON, "Expect ';' after property value.");

                bool filled = false;
                for (auto& prop : final_properties) {
                    if (prop.key == key_str && prop.value_expr == nullptr) {
                        prop.value_expr = std::move(value_expr);
                        filled = true;
                        break;
                    }
                }
                if (!filled) {
                    // If not filling a placeholder, it's a new/override property
                    final_properties.push_back({key_str, std::move(value_expr)});
                }
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after specialization block.");
    } else {
        consume(TokenType::SEMICOLON, "Expect ';' or specialization block after template usage.");
    }

    // Add the final, specialized properties to the style node.
    for (auto& prop : final_properties) {
        if (prop.value_expr == nullptr) {
            error(name, "Property '" + prop.key + "' was not given a value in the specialization of '" + name.lexeme + "'.");
        }
        styleNode->inline_properties.push_back(std::move(prop));
    }
}

std::vector<std::unique_ptr<BaseNode>> CHTLParser::parseElementTemplateUsage() {
    consume(TokenType::AT, "Expect '@' for template usage.");
    consume(TokenType::ELEMENT, "Expect 'Element' keyword for element template usage.");
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

std::unique_ptr<TemplateDeclarationNode> CHTLParser::parseTemplateDeclaration() {
    consume(TokenType::LEFT_BRACKET, "Expect '[' to start template declaration.");
    consume(TokenType::TEMPLATE, "Expect 'Template' keyword.");
    consume(TokenType::RIGHT_BRACKET, "Expect ']' to end template keyword.");

    consume(TokenType::AT, "Expect '@' for template type.");
    Token typeToken = advance(); // Consume type like Style, Element, Var

    auto def = std::make_unique<TemplateDefinitionNode>();
    if (typeToken.type == TokenType::STYLE) {
        def->type = TemplateType::STYLE;
    } else if (typeToken.type == TokenType::ELEMENT) {
        def->type = TemplateType::ELEMENT;
    } else if (typeToken.type == TokenType::VAR) {
        def->type = TemplateType::VAR;
    } else {
        error(typeToken, "Unknown template type. Expected Style, Element, or Var.");
    }

    def->name = consume(TokenType::IDENTIFIER, "Expect template name.").lexeme;
    consume(TokenType::LEFT_BRACE, "Expect '{' to start template body.");

    if (def->type == TemplateType::STYLE) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            if (check(TokenType::AT) || check(TokenType::INHERIT)) {
                // Style template inheritance
                if (check(TokenType::INHERIT)) advance();
                consume(TokenType::AT, "Expect '@' for template usage.");
                consume(TokenType::STYLE, "Can only inherit from another @Style template here.");
                Token name = consume(TokenType::IDENTIFIER, "Expect template name.");
                consume(TokenType::SEMICOLON, "Expect ';' after template usage.");

                if (template_definitions.count(name.lexeme)) {
                    const auto& base_def = template_definitions.at(name.lexeme);
                    if (base_def.type != TemplateType::STYLE) {
                        error(name, "Template '" + name.lexeme + "' is not a Style template.");
                    }
                    for (const auto& prop : base_def.style_properties) {
                        def->style_properties.push_back(prop.clone());
                    }
                } else {
                    error(name, "Base style template '" + name.lexeme + "' not found.");
                }
            } else {
                // Regular style property
                std::string key_str;
                while (!check(TokenType::COLON) && !isAtEnd()) {
                    key_str += advance().lexeme;
                }
                consume(TokenType::COLON, "Expect ':' after style property name.");
                auto value_expr = parseExpression();
                consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
                def->style_properties.push_back({key_str, std::move(value_expr)});
            }
        }
    } else if (def->type == TemplateType::ELEMENT) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            for (auto& node : parseDeclaration()) {
                def->element_body.push_back(std::move(node));
            }
        }
    } else if (def->type == TemplateType::VAR) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            std::string key_str;
            while (!check(TokenType::COLON) && !isAtEnd()) {
                key_str += advance().lexeme;
            }
            consume(TokenType::COLON, "Expect ':' after variable name.");
            auto value_expr = parseExpression();
            consume(TokenType::SEMICOLON, "Expect ';' after variable value.");
            def->variables[key_str] = std::move(value_expr);
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' to end template body.");

    // Clone the definition to store in the parser's map for later lookups.
    auto def_for_map = def->clone();
    template_definitions[def->name] = std::move(*def_for_map);

    // Create the AST node, transferring ownership of the original definition.
    return std::make_unique<TemplateDeclarationNode>(std::move(def));
}

std::unique_ptr<CustomDeclarationNode> CHTLParser::parseCustomDeclaration() {
    consume(TokenType::LEFT_BRACKET, "Expect '[' to start custom declaration.");
    consume(TokenType::CUSTOM, "Expect 'Custom' keyword.");
    consume(TokenType::RIGHT_BRACKET, "Expect ']' to end custom keyword.");

    consume(TokenType::AT, "Expect '@' for custom type.");
    Token typeToken = advance(); // Consume type like Style, Element, Var

    auto def = std::make_unique<TemplateDefinitionNode>();
    if (typeToken.type == TokenType::STYLE) {
        def->type = TemplateType::STYLE;
    } else if (typeToken.type == TokenType::ELEMENT) {
        def->type = TemplateType::ELEMENT;
    } else if (typeToken.type == TokenType::VAR) {
        def->type = TemplateType::VAR;
    } else {
        error(typeToken, "Unknown custom type. Expected Style, Element, or Var.");
    }

    def->name = consume(TokenType::IDENTIFIER, "Expect custom name.").lexeme;
    consume(TokenType::LEFT_BRACE, "Expect '{' to start custom body.");

    // NOTE: For now, the body parsing is identical to templates.
    // Specialization logic will be added here later.
    if (def->type == TemplateType::STYLE) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            if (check(TokenType::AT) || check(TokenType::INHERIT)) {
                if (check(TokenType::INHERIT)) advance();
                consume(TokenType::AT, "Expect '@' for template usage.");
                consume(TokenType::STYLE, "Can only inherit from another @Style template here.");
                Token name = consume(TokenType::IDENTIFIER, "Expect template name.");
                consume(TokenType::SEMICOLON, "Expect ';' after template usage.");

                if (template_definitions.count(name.lexeme)) {
                    const auto& base_def = template_definitions.at(name.lexeme);
                    if (base_def.type != TemplateType::STYLE) {
                        error(name, "Template '" + name.lexeme + "' is not a Style template.");
                    }
                    for (const auto& prop : base_def.style_properties) {
                        def->style_properties.push_back(prop.clone());
                    }
                } else {
                    error(name, "Base style template '" + name.lexeme + "' not found.");
                }
            } else {
                std::string key_str;
                // A property key can be a single identifier or multiple (e.g. font-family)
                while (!check(TokenType::COLON) && !check(TokenType::SEMICOLON) && !isAtEnd()) {
                    key_str += advance().lexeme;
                }

                if (match({TokenType::COLON})) {
                    // Property with value
                    auto value_expr = parseExpression();
                    consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
                    def->style_properties.push_back({key_str, std::move(value_expr)});
                } else if (match({TokenType::SEMICOLON})) {
                    // Valueless (placeholder) property
                    def->style_properties.push_back({key_str, nullptr});
                } else {
                    error(peek(), "Expect ':' or ';' after style property name.");
                }
            }
        }
    } else if (def->type == TemplateType::ELEMENT) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            for (auto& node : parseDeclaration()) {
                def->element_body.push_back(std::move(node));
            }
        }
    } else if (def->type == TemplateType::VAR) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            std::string key_str;
            while (!check(TokenType::COLON) && !isAtEnd()) {
                key_str += advance().lexeme;
            }
            consume(TokenType::COLON, "Expect ':' after variable name.");
            auto value_expr = parseExpression();
            consume(TokenType::SEMICOLON, "Expect ';' after variable value.");
            def->variables[key_str] = std::move(value_expr);
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' to end custom body.");

    // Custom definitions are also stored in the same template map.
    auto def_for_map = def->clone();
    template_definitions[def->name] = std::move(*def_for_map);

    return std::make_unique<CustomDeclarationNode>(std::move(def));
}

std::unique_ptr<ImportNode> CHTLParser::parseImportStatement() {
    consume(TokenType::LEFT_BRACKET, "Expect '[' to start import statement.");
    consume(TokenType::IMPORT, "Expect 'Import' keyword.");
    consume(TokenType::RIGHT_BRACKET, "Expect ']' to end import keyword.");

    auto node = std::make_unique<ImportNode>();

    // For now, we only support simple @Chtl imports
    consume(TokenType::AT, "Expect '@' for import type.");
    consume(TokenType::CHTL, "Expect 'Chtl' keyword for import type.");
    node->type = ImportType::Chtl;

    consume(TokenType::FROM, "Expect 'from' keyword.");

    Token pathToken = consume(TokenType::STRING, "Expect a string literal for the path.");
    node->path = pathToken.lexeme;

    // Optional 'as' alias
    if (match({TokenType::AS})) {
        node->alias = consume(TokenType::IDENTIFIER, "Expect alias name.").lexeme;
    }

    // Imports should end with a semicolon, but we can make it optional for now.
    match({TokenType::SEMICOLON});

    return node;
}

std::unique_ptr<ScriptNode> CHTLParser::parseScriptBlock() {
    Token open_brace = consume(TokenType::LEFT_BRACE, "Expect '{' to start script body.");

    int brace_depth = 1;
    int content_start_pos = current;

    while (brace_depth > 0 && !isAtEnd()) {
        if (peek().type == TokenType::LEFT_BRACE) brace_depth++;
        else if (peek().type == TokenType::RIGHT_BRACE) brace_depth--;

        if (brace_depth > 0) {
            advance();
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' to end script body.");

    // Extract the raw string content from the source
    int content_end_pos = previous().position;
    std::string content = source.substr(tokens[content_start_pos].position, content_end_pos - tokens[content_start_pos].position);

    return std::make_unique<ScriptNode>(content);
}


} // namespace CHTL
