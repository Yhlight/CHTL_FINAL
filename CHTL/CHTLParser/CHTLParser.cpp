#include "CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/TemplateDeclarationNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLNode/ReactiveValueNode.h"
#include "../Expression/Expr.h"
#include <iostream>
#include <stdexcept>
#include <algorithm> // For std::remove_if

namespace CHTL {

CHTLParser::CHTLParser(const std::string& source, const std::vector<Token>& tokens, std::map<std::string, TemplateDefinitionNode>& templates, std::string default_namespace_alias)
    : source(source), tokens(tokens), template_definitions(templates), default_namespace_alias(std::move(default_namespace_alias)) {}

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

        } else if (match({TokenType::COLON_COLON})) { // Namespace::Group(key)
            Token groupToken = consume(TokenType::IDENTIFIER, "Expect group name after '::'.");
            consume(TokenType::LEFT_PAREN, "Expect '(' after variable group name.");
            std::string key_name;
            while (!check(TokenType::RIGHT_PAREN) && !isAtEnd()) { key_name += advance().lexeme; }
            consume(TokenType::RIGHT_PAREN, "Expect ')' after variable key name.");
            return std::make_unique<VarExpr>(groupToken.lexeme, key_name, first_part.lexeme);
        }
        else if (check(TokenType::LEFT_PAREN)) { // Variable usage like Group(key)
            consume(TokenType::LEFT_PAREN, "Expect '(' after variable group name.");

            std::string key_name;
            while (!check(TokenType::RIGHT_PAREN) && !isAtEnd()) {
                key_name += advance().lexeme;
            }

            consume(TokenType::RIGHT_PAREN, "Expect ')' after variable key name.");
            return std::make_unique<VarExpr>(first_part.lexeme, key_name, ""); // No namespace
        } else {
            // It's an unquoted string literal, as per the spec.
            return std::make_unique<LiteralExpr>(first_part.lexeme);
        }
    }

    if (match({TokenType::STRING})) {
        // Use the string constructor for LiteralExpr
        return std::make_unique<LiteralExpr>(previous().lexeme);
    }

    // Handle selectors like #box.width or hex codes like #fff
    if (check(TokenType::SYMBOL) && peek().lexeme == "#") {
        Token hash = advance(); // Consume '#'
        // Disambiguate by looking for a following IDENTIFIER and then a DOT.
        if (check(TokenType::IDENTIFIER) && tokens.size() > current + 1 && tokens[current+1].type == TokenType::DOT) {
            Token selector_name = advance();
            consume(TokenType::DOT, "Expect '.' after selector.");
            Token property = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
            Token full_selector = {TokenType::IDENTIFIER, "#" + selector_name.lexeme, hash.line, hash.position};
            return std::make_unique<ReferenceExpr>(full_selector, property);
        } else {
            // It's a hex code or similar literal
            std::string value = "#";
             while (isalnum(peek().lexeme[0])) { // Consume the rest of the hex code
                value += advance().lexeme;
            }
            return std::make_unique<LiteralExpr>(value);
        }
    }

    // Handle .class.prop
    if (match({TokenType::DOT})) {
        Token selector_name = consume(TokenType::IDENTIFIER, "Expect class name after '.'");
        consume(TokenType::DOT, "Expect '.' after class name.");
        Token property = consume(TokenType::IDENTIFIER, "Expect property name after '.'");
        Token full_selector = {TokenType::IDENTIFIER, "." + selector_name.lexeme, selector_name.line, selector_name.position - 1}; // Reconstruct
        return std::make_unique<ReferenceExpr>(full_selector, property);
    }

    if (match({TokenType::DOLLAR})) {
        Token var_name = consume(TokenType::IDENTIFIER, "Expect variable name after '$'.");
        consume(TokenType::DOLLAR, "Expect '$' after variable name.");
        return std::make_unique<ReactiveValueNode>(var_name.lexeme);
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
                case TokenType::NAMESPACE:
                    return parseNamespace();
                // etc. will be added here later.
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

std::unique_ptr<NamespaceNode> CHTLParser::parseNamespace() {
    consume(TokenType::LEFT_BRACKET, "Expect '[' to start namespace statement.");
    consume(TokenType::NAMESPACE, "Expect 'Namespace' keyword.");
    consume(TokenType::RIGHT_BRACKET, "Expect ']' to end namespace keyword.");

    Token name = consume(TokenType::IDENTIFIER, "Expect namespace name.");
    auto namespaceNode = std::make_unique<NamespaceNode>(name.lexeme);

    std::string previous_namespace = current_namespace;
    current_namespace = current_namespace.empty() ? name.lexeme : current_namespace + "::" + name.lexeme;

    if (match({TokenType::LEFT_BRACE})) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            namespaceNode->addChild(parseTopLevelDeclaration());
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after namespace body.");
    } else {
        // Namespace without braces, applies to the rest of the file
        // This is tricky, we'll assume it doesn't contain other top-level namespaces for now.
        while (!isAtEnd()) {
            namespaceNode->addChild(parseTopLevelDeclaration());
        }
    }

    current_namespace = previous_namespace; // Restore namespace

    return namespaceNode;
}

std::vector<std::unique_ptr<BaseNode>> CHTLParser::parseDeclaration() {
    if (check(TokenType::AT)) {
        return parseElementTemplateUsage();
    }

    if (peek().type == TokenType::LEFT_BRACKET && tokens.size() > current + 1) {
        if (tokens[current + 1].lexeme == "Origin") {
             std::vector<std::unique_ptr<BaseNode>> nodes;
            nodes.push_back(parseOriginBlock());
            return nodes;
        }
        if (tokens[current + 1].type == TokenType::IMPORT) {
             std::vector<std::unique_ptr<BaseNode>> nodes;
            nodes.push_back(parseImportStatement());
            return nodes;
        }
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
    consume(TokenType::STYLE, "Expect 'Style' keyword for style template usage.");
    Token nameToken = consume(TokenType::IDENTIFIER, "Expect template name.");
    std::string name = nameToken.lexeme;

    if(match({TokenType::FROM})) {
        Token ns = consume(TokenType::IDENTIFIER, "Expect namespace name after 'from'.");
        name = ns.lexeme + "::" + name;
    }

    if (!template_definitions.count(name)) {
        // If not found, try to find a unique match in any namespace
        std::string found_name = "";
        for (const auto& pair : template_definitions) {
            if (pair.first.find("::" + name) != std::string::npos) {
                if (!found_name.empty()) {
                    error(nameToken, "Ambiguous template reference: '" + name + "'. Please specify a namespace using 'from'.");
                }
                found_name = pair.first;
            }
        }
        if (!found_name.empty()) {
            name = found_name;
        } else if (!suppress_not_found_errors) {
            error(nameToken, "Style template '" + name + "' not found.");
        }
        // If we're suppressing errors, we can't continue parsing this rule,
        // so we just skip to the end of the statement.
        while(!check(TokenType::SEMICOLON) && !check(TokenType::LEFT_BRACE) && !isAtEnd()) {
            advance();
        }
        if(check(TokenType::SEMICOLON)) advance();
        return;
    }

    const auto& def = template_definitions.at(name);
    if (def.type != TemplateType::STYLE) {
        error(nameToken, "Template '" + name + "' is not a Style template.");
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
            error(nameToken, "Property '" + prop.key + "' was not given a value in the specialization of '" + name + "'.");
        }
        styleNode->inline_properties.push_back(std::move(prop));
    }
}

std::vector<std::unique_ptr<BaseNode>> CHTLParser::parseElementTemplateUsage() {
    consume(TokenType::AT, "Expect '@' for template usage.");
    consume(TokenType::ELEMENT, "Expect 'Element' keyword for element template usage.");
    Token nameToken = consume(TokenType::IDENTIFIER, "Expect template name.");
    std::string name = nameToken.lexeme;

    if(match({TokenType::FROM})) {
        Token ns = consume(TokenType::IDENTIFIER, "Expect namespace name after 'from'.");
        name = ns.lexeme + "::" + name;
    }

    consume(TokenType::SEMICOLON, "Expect ';' after template usage.");

    if (!template_definitions.count(name)) {
        // If not found, try to find a unique match in any namespace
        std::string found_name = "";
        for (const auto& pair : template_definitions) {
            if (pair.first.find("::" + name) != std::string::npos) {
                if (!found_name.empty()) {
                    error(nameToken, "Ambiguous template reference: '" + name + "'. Please specify a namespace using 'from'.");
                }
                found_name = pair.first;
            }
        }
        if (!found_name.empty()) {
            name = found_name;
        }
    }

    if (template_definitions.count(name)) {
        const auto& def = template_definitions.at(name);
        if (def.type != TemplateType::ELEMENT) {
            error(nameToken, "Template '" + name + "' is not an Element template.");
        }
        std::vector<std::unique_ptr<BaseNode>> cloned_nodes;
        for (const auto& node : def.element_body) {
            cloned_nodes.push_back(node->clone());
        }
        return cloned_nodes;
    } else {
        if (!suppress_not_found_errors) {
            error(nameToken, "Element template '" + name + "' not found.");
        }
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

    std::string tpl_name = consume(TokenType::IDENTIFIER, "Expect template name.").lexeme;
    if (!current_namespace.empty()) {
        def->name = current_namespace + "::" + tpl_name;
    } else if (!default_namespace_alias.empty()) {
        def->name = default_namespace_alias + "::" + tpl_name;
    }
    else {
        def->name = tpl_name;
    }
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

    std::string custom_name = consume(TokenType::IDENTIFIER, "Expect custom name.").lexeme;
    if (!current_namespace.empty()) {
        def->name = current_namespace + "::" + custom_name;
    } else if (!default_namespace_alias.empty()) {
        def->name = default_namespace_alias + "::" + custom_name;
    } else {
        def->name = custom_name;
    }
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

    consume(TokenType::AT, "Expect '@' for import type.");
    Token typeToken = advance(); // Should be an identifier now

    ImportType type;
    if (typeToken.lexeme == "Chtl") {
        type = ImportType::CHTL;
    } else if (typeToken.lexeme == "Html") {
        type = ImportType::HTML;
    } else if (typeToken.lexeme == "Style") {
        type = ImportType::STYLE;
    } else if (typeToken.lexeme == "JavaScript") {
        type = ImportType::JAVASCRIPT;
    } else {
        error(typeToken, "Unsupported import type.");
    }

    consume(TokenType::FROM, "Expect 'from' after import type.");

    // The spec allows unquoted literals for paths, which can be a single identifier
    // or a path-like sequence of identifiers, dots, and slashes.
    std::string path;
    if (match({TokenType::STRING})) {
        path = previous().lexeme;
    } else {
        // Handle unquoted paths
        while (!check(TokenType::AS) && !check(TokenType::SEMICOLON) && !isAtEnd()) {
            path += advance().lexeme;
        }
    }

    if (path.empty()) {
        error(peek(), "Expect path string or literal after 'from'.");
    }

    std::string alias = "";
    if (match({TokenType::AS})) {
        alias = consume(TokenType::IDENTIFIER, "Expect alias name after 'as'.").lexeme;
    }

    consume(TokenType::SEMICOLON, "Expect ';' after import statement.");

    return std::make_unique<ImportNode>(type, path, alias);
}

std::vector<std::unique_ptr<ImportNode>> CHTLParser::discoverImports() {
    std::vector<std::unique_ptr<ImportNode>> imports;
    int original_pos = current; // Save state

    while (!isAtEnd()) {
        if (check(TokenType::LEFT_BRACKET) &&
            tokens.size() > current + 1 &&
            tokens[current + 1].type == TokenType::IMPORT)
        {
            // We have a potential import, let's parse it fully.
            int temp_pos = current;
            try {
                auto import_node = parseImportStatement();
                imports.push_back(std::move(import_node));
            } catch (const std::runtime_error& e) {
                // If parsing fails, it's not a valid import statement, so we just skip.
                // We need to recover the parser state to where it was before we tried.
                current = temp_pos;
                advance(); // Advance past the initial '[' to avoid an infinite loop.
            }
        } else {
            advance();
        }
    }

    current = original_pos; // Restore state
    return imports;
}

} // namespace CHTL
