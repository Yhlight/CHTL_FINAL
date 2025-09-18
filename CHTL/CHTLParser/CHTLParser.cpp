#include "CHTLParser.h"
#include "../CHTLLoader/CHTLLoader.h"
#include "../CHTLLexer/CHTLLexer.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../Expression/Expr.h"
#include "../Expression/Value.h"
#include "../CHTLNode/DocumentNode.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <stdexcept>

namespace CHTL {

// Helper to get filename without extension for default namespace
std::string getFilename(const std::string& path) {
    size_t last_slash = path.find_last_of("/\\");
    std::string filename = (last_slash == std::string::npos) ? path : path.substr(last_slash + 1);
    size_t last_dot = filename.rfind('.');
    if (last_dot != std::string::npos) {
        return filename.substr(0, last_dot);
    }
    return filename;
}

CHTLParser::CHTLParser(const std::string& source, const std::vector<Token>& tokens, const std::string& file_path)
    : source(source), tokens(tokens), file_path(file_path) {
        this->current_namespace = getFilename(file_path);
    }

// --- Expression Parser Implementation ---
// ... (This part is unchanged)
std::unique_ptr<Expr> CHTLParser::parseExpression() { return parseConditional(); }
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
        std::string unit;
        if (check(TokenType::IDENTIFIER)) { unit = advance().lexeme; }
        else if (check(TokenType::PERCENT)) { unit = advance().lexeme; }
        try {
            return std::make_unique<LiteralExpr>(Value(std::stod(number.lexeme), unit));
        }
        catch (const std::invalid_argument& e) { error(number, "Invalid number format."); }
    }
    if (match({TokenType::IDENTIFIER})) {
        Token first_part = previous();
        if (check(TokenType::DOT)) {
            consume(TokenType::DOT, "Expect '.' after selector.");
            Token property = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
            return std::make_unique<ReferenceExpr>(first_part, property);
        } else if (check(TokenType::LEFT_PAREN)) {
            consume(TokenType::LEFT_PAREN, "Expect '(' after variable group name.");
            std::string key_name;
            while (!check(TokenType::RIGHT_PAREN) && !isAtEnd()) { key_name += advance().lexeme; }
            consume(TokenType::RIGHT_PAREN, "Expect ')' after variable key name.");
            return std::make_unique<VarExpr>(first_part.lexeme, key_name);
        } else {
             // This is for an unquoted string literal like `color: red;`
            return std::make_unique<LiteralExpr>(Value(first_part.lexeme));
        }
    }
    if (check(TokenType::SYMBOL) && (peek().lexeme == "#" || peek().lexeme == ".")) {
        Token first_part = advance();
        if (tokens.size() > current && tokens[current + 1].type == TokenType::DOT) {
             Token selector_name = consume(TokenType::IDENTIFIER, "Expect selector name.");
             consume(TokenType::DOT, "Expect '.' after selector.");
             Token property = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
             Token full_selector = {first_part.type, first_part.lexeme + selector_name.lexeme, first_part.line, first_part.position};
             return std::make_unique<ReferenceExpr>(full_selector, property);
        } else {
            std::string value = first_part.lexeme;
             while(check(TokenType::IDENTIFIER) || check(TokenType::NUMBER)) { value += advance().lexeme; }
            return std::make_unique<LiteralExpr>(Value(value));
        }
    }
    if (match({TokenType::STRING})) {
        return std::make_unique<LiteralExpr>(Value(previous().lexeme));
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
    auto docNode = std::make_unique<DocumentNode>();

    while (!isAtEnd() && peek().type != TokenType::END_OF_FILE) {
        if (peek().type == TokenType::LEFT_BRACKET) {
            if (tokens.size() > current + 1 && tokens[current + 1].lexeme == "Template") {
                parseTemplateDeclaration();
            } else if (tokens.size() > current + 1 && tokens[current + 1].lexeme == "Origin") {
                docNode->addChild(parseOriginBlock());
            } else if (tokens.size() > current + 1 && tokens[current + 1].lexeme == "Import") {
                parseImportStatement();
            } else {
                error(peek(), "Unexpected left bracket.");
            }
        } else {
            for (auto& child : parseDeclaration()) {
                docNode->addChild(std::move(child));
            }
        }
    }

    return docNode;
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
            if (tokens[current + i].type == TokenType::COLON) { isInlineProp = true; break; }
            if (tokens[current + i].type == TokenType::LEFT_BRACE) { isInlineProp = false; break; }
            i++;
        }
        if (check(TokenType::AT)) {
            parseStyleTemplateUsage(styleNode.get());
        } else if (isInlineProp) {
            std::string key_str;
            while (!check(TokenType::COLON) && !isAtEnd()) { key_str += advance().lexeme; }
            consume(TokenType::COLON, "Expect ':' after style property name.");
            auto value_expr = parseExpression();
            consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
            styleNode->inline_properties.push_back({key_str, std::move(value_expr)});
        } else {
            CssRuleNode rule;
            while (!check(TokenType::LEFT_BRACE) && !isAtEnd()) { rule.selector += advance().lexeme; }
            consume(TokenType::LEFT_BRACE, "Expect '{' after rule selector.");
            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                std::string key_str;
                while (!check(TokenType::COLON) && !isAtEnd()) { key_str += advance().lexeme; }
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
// ... (unchanged)
bool CHTLParser::isAtEnd() { return peek().type == TokenType::END_OF_FILE; }
Token CHTLParser::peek() { return tokens[current]; }
Token CHTLParser::previous() { return tokens[current - 1]; }
Token CHTLParser::advance() { if (!isAtEnd()) current++; return previous(); }
bool CHTLParser::check(TokenType type) { if (isAtEnd()) return false; return peek().type == type; }
Token CHTLParser::consume(TokenType type, const std::string& message) { if (check(type)) return advance(); error(peek(), message); throw std::runtime_error(message); }
bool CHTLParser::match(const std::vector<TokenType>& types) { for (TokenType type : types) { if (check(type)) { advance(); return true; } } return false; }
void CHTLParser::error(const Token& token, const std::string& message) { std::cerr << "[line " << token.line << "] Error"; if (token.type == TokenType::END_OF_FILE) { std::cerr << " at end"; } else { std::cerr << " at '" << token.lexeme << "'"; } std::cerr << ": " << message << std::endl; throw std::runtime_error(message); }

// --- New and Updated Methods for Imports and Namespaces ---

void CHTLParser::parseImportStatement() {
    consume(TokenType::LEFT_BRACKET, "Expect '[' to start import statement.");
    consume(TokenType::IDENTIFIER, "Expect 'Import' keyword.");
    consume(TokenType::RIGHT_BRACKET, "Expect ']' to end import keyword.");

    consume(TokenType::AT, "Expect '@' for import type.");
    Token typeToken = consume(TokenType::IDENTIFIER, "Expect import type (e.g., Chtl).");

    if (typeToken.lexeme == "Chtl") {
        consume(TokenType::FROM, "Expect 'from' keyword.");
        Token pathToken = consume(TokenType::STRING, "Expect file path in quotes.");

        std::string imported_content = CHTLLoader::load(this->file_path, pathToken.lexeme);
        std::string imported_path = FileSystem::getDirectory(this->file_path) + pathToken.lexeme;

        CHTLLexer sub_lexer(imported_content);
        std::vector<Token> sub_tokens = sub_lexer.scanTokens();

        CHTLParser sub_parser(imported_content, sub_tokens, imported_path);
        sub_parser.parse(); // This will parse and populate its own template_definitions

        // Merge the templates under a namespace
        std::string import_namespace = getFilename(pathToken.lexeme);

        // Get a mutable reference to the sub-parser's templates
        auto& imported_template_map = sub_parser.getMutableTemplateDefinitions();

        // Find the namespace map from the imported file
        auto it = imported_template_map.find(import_namespace);
        if (it != imported_template_map.end()) {
            // Move the entire map of templates for that namespace
            this->template_definitions[import_namespace] = std::move(it->second);
        }
        consume(TokenType::SEMICOLON, "Expect ';' after import statement.");
    } else {
        error(typeToken, "Unsupported import type.");
    }
}

std::unique_ptr<BaseNode> CHTLParser::parseOriginBlock() { /* ... unchanged ... */ }

void CHTLParser::parseStyleTemplateUsage(StyleNode* styleNode) {
    consume(TokenType::AT, "Expect '@' for template usage.");
    Token type = consume(TokenType::IDENTIFIER, "Expect template type.");
    if (type.lexeme != "Style") { error(type, "Expect '@Style' template usage here."); }
    Token name = consume(TokenType::IDENTIFIER, "Expect template name.");
    consume(TokenType::SEMICOLON, "Expect ';' after template usage.");

    // Simple lookup (no namespace support yet)
    for (const auto& ns_pair : template_definitions) {
        if (ns_pair.second.count(name.lexeme)) {
            const auto& def = ns_pair.second.at(name.lexeme);
            if (def.type != TemplateType::STYLE) { error(name, "Template '" + name.lexeme + "' is not a Style template."); }
            for (const auto& prop : def.style_properties) {
                styleNode->inline_properties.push_back(prop.clone());
            }
            return;
        }
    }
    error(name, "Style template '" + name.lexeme + "' not found in any namespace.");
}

std::vector<std::unique_ptr<BaseNode>> CHTLParser::parseElementTemplateUsage() {
    consume(TokenType::AT, "Expect '@' for template usage.");
    Token type = consume(TokenType::IDENTIFIER, "Expect template type.");
    if (type.lexeme != "Element") { error(type, "Expect '@Element' template usage here."); }
    Token name = consume(TokenType::IDENTIFIER, "Expect template name.");
    consume(TokenType::SEMICOLON, "Expect ';' after template usage.");

    // Simple lookup (no namespace support yet)
    for (const auto& ns_pair : template_definitions) {
        if (ns_pair.second.count(name.lexeme)) {
            const auto& def = ns_pair.second.at(name.lexeme);
            if (def.type != TemplateType::ELEMENT) { error(name, "Template '" + name.lexeme + "' is not an Element template."); }
            std::vector<std::unique_ptr<BaseNode>> cloned_nodes;
            for (const auto& node : def.element_body) {
                cloned_nodes.push_back(node->clone());
            }
            return cloned_nodes;
        }
    }
    error(name, "Element template '" + name.lexeme + "' not found in any namespace.");
    return {};
}

void CHTLParser::parseTemplateDeclaration() {
    consume(TokenType::LEFT_BRACKET, "Expect '[' to start template declaration.");
    Token keyword = consume(TokenType::IDENTIFIER, "Expect 'Template' keyword.");
    if (keyword.lexeme != "Template") { error(keyword, "Expect 'Template' keyword in declaration."); }
    consume(TokenType::RIGHT_BRACKET, "Expect ']' to end template keyword.");
    consume(TokenType::AT, "Expect '@' for template type.");
    Token typeToken = consume(TokenType::IDENTIFIER, "Expect template type (e.g., Style, Element).");
    TemplateDefinitionNode def;
    if (typeToken.lexeme == "Style") { def.type = TemplateType::STYLE; }
    else if (typeToken.lexeme == "Element") { def.type = TemplateType::ELEMENT; }
    else if (typeToken.lexeme == "Var") { def.type = TemplateType::VAR; }
    else { error(typeToken, "Unknown template type."); }
    def.name = consume(TokenType::IDENTIFIER, "Expect template name.").lexeme;
    consume(TokenType::LEFT_BRACE, "Expect '{' to start template body.");
    if (def.type == TemplateType::STYLE) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            if (check(TokenType::AT) || check(TokenType::INHERIT)) {
                if (check(TokenType::INHERIT)) { advance(); }
                consume(TokenType::AT, "Expect '@' for template usage.");
                Token type = consume(TokenType::IDENTIFIER, "Expect template type.");
                if (type.lexeme != "Style") { error(type, "Can only inherit from another @Style template here."); }
                Token name = consume(TokenType::IDENTIFIER, "Expect template name.");
                consume(TokenType::SEMICOLON, "Expect ';' after template usage.");
                bool found = false;
                for (const auto& ns_pair : template_definitions) {
                    if (ns_pair.second.count(name.lexeme)) {
                        const auto& base_def = ns_pair.second.at(name.lexeme);
                        if (base_def.type != TemplateType::STYLE) { error(name, "Template '" + name.lexeme + "' is not a Style template."); }
                        for (const auto& prop : base_def.style_properties) {
                            def.style_properties.push_back(prop.clone());
                        }
                        found = true;
                        break;
                    }
                }
                if (!found) { error(name, "Base style template '" + name.lexeme + "' not found."); }
            } else {
                std::string key_str;
                while (!check(TokenType::COLON) && !isAtEnd()) { key_str += advance().lexeme; }
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
    } else if (def.type == TemplateType::VAR) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            std::string key_str;
            while (!check(TokenType::COLON) && !isAtEnd()) { key_str += advance().lexeme; }
            consume(TokenType::COLON, "Expect ':' after variable name.");
            auto value_expr = parseExpression();
            consume(TokenType::SEMICOLON, "Expect ';' after variable value.");
            def.variables[key_str] = std::move(value_expr);
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' to end template body.");
    template_definitions[current_namespace][def.name] = std::move(def);
}

} // namespace CHTL
