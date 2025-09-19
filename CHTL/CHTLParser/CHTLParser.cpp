#include "CHTLParser.h"
#include "../CHTLLoader/CHTLLoader.h"
#include "../CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/OriginNode.h"
#include "CHTL/CHTLNode/ConfigNode.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <stdexcept>

namespace CHTL {

// --- Recursive Helpers for Specialization ---
bool findAndDelete(std::vector<std::unique_ptr<BaseNode>>& nodes, const std::string& tagName, int& index) {
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        if (ElementNode* elem = dynamic_cast<ElementNode*>(it->get())) {
            if (elem->tagName == tagName) {
                if (index == 0) {
                    nodes.erase(it);
                    return true;
                }
                index--;
            }
        }
    }
    for (auto& node : nodes) {
        if (ElementNode* elem = dynamic_cast<ElementNode*>(node.get())) {
            if (findAndDelete(elem->children, tagName, index)) {
                return true;
            }
        }
    }
    return false;
}

bool findAndInsert(std::vector<std::unique_ptr<BaseNode>>& nodes, const std::string& tagName, int& index, TokenType position, std::vector<std::unique_ptr<BaseNode>>& nodes_to_insert) {
    for (auto it = nodes.begin(); it != nodes.end(); ++it) {
        if (ElementNode* elem = dynamic_cast<ElementNode*>(it->get())) {
            if (elem->tagName == tagName) {
                if (index == 0) {
                    if (position == TokenType::AFTER) {
                        nodes.insert(std::next(it), std::make_move_iterator(nodes_to_insert.begin()), std::make_move_iterator(nodes_to_insert.end()));
                    } else if (position == TokenType::BEFORE) {
                        nodes.insert(it, std::make_move_iterator(nodes_to_insert.begin()), std::make_move_iterator(nodes_to_insert.end()));
                    } else if (position == TokenType::REPLACE) {
                        it = nodes.erase(it);
                        nodes.insert(it, std::make_move_iterator(nodes_to_insert.begin()), std::make_move_iterator(nodes_to_insert.end()));
                    }
                    return true;
                }
                index--;
            }
        }
    }
    for (auto& node : nodes) {
        if (ElementNode* elem = dynamic_cast<ElementNode*>(node.get())) {
            if (findAndInsert(elem->children, tagName, index, position, nodes_to_insert)) {
                return true;
            }
        }
    }
    return false;
}


// --- Parser Implementation ---
std::string getFilename(const std::string& path) {
    size_t last_slash = path.find_last_of("/\\");
    std::string filename = (last_slash == std::string::npos) ? path : path.substr(last_slash + 1);
    size_t last_dot = filename.rfind('.');
    if (last_dot != std::string::npos) {
        return filename.substr(0, last_dot);
    }
    return filename;
}

CHTLParser::CHTLParser(const std::string& source, const std::vector<Token>& tokens, const std::string& file_path, std::shared_ptr<Configuration> config)
    : source(source), tokens(tokens), file_path(file_path), config(config) {
        this->current_namespace = getFilename(file_path);
}

const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& CHTLParser::getTemplateDefinitions() const { return template_definitions; }
std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& CHTLParser::getMutableTemplateDefinitions() { return template_definitions; }


std::unique_ptr<BaseNode> CHTLParser::parse() {
    while (peek().type == TokenType::LEFT_BRACKET) {
        advance(); // Consume '['
        if (match({TokenType::TEMPLATE})) {
            parseSymbolDeclaration(false);
        } else if (match({TokenType::CUSTOM})) {
            parseSymbolDeclaration(true);
        } else if (match({TokenType::ORIGIN})) {
            parseOriginBlock();
        } else if (match({TokenType::IMPORT})) {
            parseImportStatement();
        } else if (match({TokenType::CONFIGURATION})) {
            parseConfigurationBlock();
        } else {
            error(peek(), "Unknown block type inside [].");
        }
        if (isAtEnd()) return nullptr; // File can legally end after a block.
    }

    // After all top-level blocks, we expect a single root element.
    if (isAtEnd()) return nullptr; // No root element, just declarations.

    auto nodes = parseDeclaration();
    if (nodes.empty()) {
        error(peek(), "Expected a root element declaration.");
        return nullptr;
    }
    if (nodes.size() > 1) {
        error(peek(), "Expected only one root element declaration.");
    }

    if (!isAtEnd()) {
        // This check might be too strict if whitespace is allowed at the end.
        // But for now, it's a good sanity check.
        if (peek().type != TokenType::END_OF_FILE) {
           error(peek(), "Unexpected tokens after root element.");
        }
    }

    return std::move(nodes[0]);
}

std::vector<std::unique_ptr<BaseNode>> CHTLParser::parseDeclaration() {
    if (check(TokenType::AT)) return parseElementTemplateUsage();
    if (peek().type == TokenType::LEFT_BRACKET && tokens.size() > current + 1 && tokens[current + 1].lexeme == "Origin") {
        std::vector<std::unique_ptr<BaseNode>> nodes;
        nodes.push_back(parseOriginBlock());
        return nodes;
    }
    if (match({TokenType::TEXT})) {
        consume(TokenType::LEFT_BRACE, "Expect '{' after 'text'.");
        Token content = consume(TokenType::STRING, "Expect string literal.");
        consume(TokenType::RIGHT_BRACE, "Expect '}' after text block.");
        std::vector<std::unique_ptr<BaseNode>> nodes;
        nodes.push_back(std::make_unique<TextNode>(content.lexeme));
        return nodes;
    }
    if (match({TokenType::STYLE})) {
        std::vector<std::unique_ptr<BaseNode>> nodes;
        nodes.push_back(parseStyleBlock());
        return nodes;
    }
    if (check(TokenType::IDENTIFIER)) {
        std::vector<std::unique_ptr<BaseNode>> nodes;
        nodes.push_back(parseElement());
        return nodes;
    }
    error(peek(), "Expect a declaration.");
    return {};
}

std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    Token tagName = consume(TokenType::IDENTIFIER, "Expect element name.");
    auto element = std::make_unique<ElementNode>(tagName.lexeme);
    consume(TokenType::LEFT_BRACE, "Expect '{' after element name.");
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if ((peek().type == TokenType::IDENTIFIER || peek().type == TokenType::TEXT) && tokens.size() > current + 1 && tokens[current + 1].type == TokenType::COLON) {
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
    if (!check(TokenType::IDENTIFIER) && !check(TokenType::TEXT)) {
        error(peek(), "Expect attribute name.");
    }
    Token key = advance();
    consume(TokenType::COLON, "Expect ':' after attribute name.");
    Token value_token;
    if (match({TokenType::STRING, TokenType::IDENTIFIER, TokenType::NUMBER})) {
        value_token = previous();
    } else {
        error(peek(), "Expect attribute value.");
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
            styleNode->direct_properties.push_back({key_str, std::move(value_expr)});
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

void CHTLParser::parseStyleTemplateUsage(StyleNode* styleNode) {
    consume(TokenType::AT, "Expect '@' for template usage.");
    Token type = consume(TokenType::IDENTIFIER, "Expect template type.");
    if (type.lexeme != "Style") { error(type, "Expect '@Style' template usage here."); }
    Token name = consume(TokenType::IDENTIFIER, "Expect template name.");
    StyleNode::StyleApplication app;
    app.template_name = name.lexeme;
    if (match({TokenType::LEFT_BRACE})) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            if (match({TokenType::DELETE})) {
                do {
                    Token prop_to_delete = consume(TokenType::IDENTIFIER, "Expect property name after 'delete'.");
                    app.deleted_properties.push_back(prop_to_delete.lexeme);
                } while (match({TokenType::COMMA}));
                consume(TokenType::SEMICOLON, "Expect ';' after delete statement.");
            } else {
                std::string key_str;
                while (!check(TokenType::COLON) && !isAtEnd()) { key_str += advance().lexeme; }
                consume(TokenType::COLON, "Expect ':' after style property name.");
                auto value_expr = parseExpression();
                consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
                app.new_or_overridden_properties.push_back({key_str, std::move(value_expr)});
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after customization block.");
    } else {
        consume(TokenType::SEMICOLON, "Expect ';' after template usage.");
    }
    styleNode->template_applications.push_back(std::move(app));
}

std::vector<std::unique_ptr<BaseNode>> CHTLParser::parseElementTemplateUsage() {
    consume(TokenType::AT, "Expect '@' for template usage.");
    Token type = consume(TokenType::IDENTIFIER, "Expect template type.");
    if (type.lexeme != "Element") { error(type, "Expect '@Element' template usage here."); }
    Token name = consume(TokenType::IDENTIFIER, "Expect template name.");
    const TemplateDefinitionNode* def = nullptr;
    for (const auto& ns_pair : template_definitions) {
        if (ns_pair.second.count(name.lexeme)) {
            def = &ns_pair.second.at(name.lexeme);
            break;
        }
    }
    if (!def) { error(name, "Element template '" + name.lexeme + "' not found."); return {}; }
    if (def->type != TemplateType::ELEMENT) { error(name, "Template '" + name.lexeme + "' is not an Element template."); }
    std::vector<std::unique_ptr<BaseNode>> cloned_nodes;
    for (const auto& node : def->element_body) {
        cloned_nodes.push_back(node->clone());
    }
    if (match({TokenType::LEFT_BRACE})) {
        if (!def->is_custom) {
            error(name, "Cannot specialize a non-custom [Template].");
        }
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            if (match({TokenType::DELETE})) {
                Token element_to_delete = consume(TokenType::IDENTIFIER, "Expect tag name.");
                int index = 0;
                if (match({TokenType::LEFT_BRACKET})) {
                    Token index_token = consume(TokenType::NUMBER, "Expect index.");
                    index = std::stoi(index_token.lexeme);
                    consume(TokenType::RIGHT_BRACKET, "Expect ']'.");
                }
                if (!findAndDelete(cloned_nodes, element_to_delete.lexeme, index)) {
                    error(element_to_delete, "Could not find element to delete.");
                }
                consume(TokenType::SEMICOLON, "Expect ';'.");
            } else if (match({TokenType::INSERT})) {
                if (!match({TokenType::AFTER, TokenType::BEFORE, TokenType::REPLACE})) {
                    error(peek(), "Expect 'after', 'before', or 'replace'.");
                }
                Token position = previous();
                Token selector = consume(TokenType::IDENTIFIER, "Expect tag name.");
                int index = 0;
                if (match({TokenType::LEFT_BRACKET})) {
                    Token index_token = consume(TokenType::NUMBER, "Expect index.");
                    index = std::stoi(index_token.lexeme);
                    consume(TokenType::RIGHT_BRACKET, "Expect ']'.");
                }
                consume(TokenType::LEFT_BRACE, "Expect '{'.");
                std::vector<std::unique_ptr<BaseNode>> nodes_to_insert;
                while(!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                    for (auto& node : parseDeclaration()) {
                        nodes_to_insert.push_back(std::move(node));
                    }
                }
                consume(TokenType::RIGHT_BRACE, "Expect '}'.");
                if (!findAndInsert(cloned_nodes, selector.lexeme, index, position.type, nodes_to_insert)) {
                    error(selector, "Could not find target for insert.");
                }
            } else {
                error(peek(), "Unsupported specialization keyword.");
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}'.");
    } else {
        consume(TokenType::SEMICOLON, "Expect ';'.");
    }
    return cloned_nodes;
}

void CHTLParser::parseSymbolDeclaration(bool is_custom) {
    // Assumes '[' and 'Template'/'Custom' keyword have been consumed.
    consume(TokenType::RIGHT_BRACKET, "Expect ']' to end keyword.");
    consume(TokenType::AT, "Expect '@' for symbol type.");
    Token typeToken = consume(TokenType::IDENTIFIER, "Expect symbol type.");
    TemplateDefinitionNode def;
    def.is_custom = is_custom;
    if (typeToken.lexeme == "Style") { def.type = TemplateType::STYLE; }
    else if (typeToken.lexeme == "Element") { def.type = TemplateType::ELEMENT; }
    else if (typeToken.lexeme == "Var") { def.type = TemplateType::VAR; }
    else { error(typeToken, "Unknown symbol type."); }
    def.name = consume(TokenType::IDENTIFIER, "Expect symbol name.").lexeme;
    consume(TokenType::LEFT_BRACE, "Expect '{' to start symbol body.");
    if (def.type == TemplateType::STYLE) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            // Optional 'inherit' keyword
            match({TokenType::INHERIT});

            if (check(TokenType::AT)) {
                consume(TokenType::AT, "Expect '@'.");
                Token type_token = consume(TokenType::IDENTIFIER, "Expect 'Style' keyword.");
                if (type_token.lexeme != "Style") {
                    error(type_token, "Can only inherit from another @Style template.");
                }
                Token parent_name = consume(TokenType::IDENTIFIER, "Expect parent template name.");
                def.inherited_templates.push_back(parent_name.lexeme);
                consume(TokenType::SEMICOLON, "Expect ';' after inheritance statement.");
            } else {
                std::string key_str;
                while (!check(TokenType::COLON) && !isAtEnd()) { key_str += advance().lexeme; }
                consume(TokenType::COLON, "Expect ':'.");
                auto value_expr = parseExpression();
                consume(TokenType::SEMICOLON, "Expect ';'.");
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
    consume(TokenType::RIGHT_BRACE, "Expect '}' to end symbol body.");
    template_definitions[current_namespace][def.name] = std::move(def);
}

void CHTLParser::parseImportStatement() {
    // Assumes '[' and 'Import' have been consumed.
    consume(TokenType::RIGHT_BRACKET, "Expect ']'.");
    consume(TokenType::AT, "Expect '@'.");
    Token typeToken = consume(TokenType::IDENTIFIER, "Expect import type.");
    if (typeToken.lexeme == "Chtl") {
        consume(TokenType::FROM, "Expect 'from'.");
        Token pathToken = consume(TokenType::STRING, "Expect file path.");
        std::string imported_content = CHTLLoader::load(this->file_path, pathToken.lexeme);
        std::string imported_path = FileSystem::getDirectory(this->file_path) + pathToken.lexeme;
        // When importing, we use the current configuration for the sub-parser.
        // A more advanced implementation might have per-file configurations.
        CHTLLexer sub_lexer(imported_content, this->config);
        std::vector<Token> sub_tokens = sub_lexer.scanTokens();
        CHTLParser sub_parser(imported_content, sub_tokens, imported_path, this->config);
        sub_parser.parse();
        auto& imported_template_map = sub_parser.getMutableTemplateDefinitions();
        std::string import_namespace = getFilename(pathToken.lexeme);
        auto it = imported_template_map.find(import_namespace);
        if (it != imported_template_map.end()) {
            this->template_definitions[import_namespace] = std::move(it->second);
        }
        consume(TokenType::SEMICOLON, "Expect ';'.");
    } else {
        error(typeToken, "Unsupported import type.");
    }
}

std::unique_ptr<BaseNode> CHTLParser::parseOriginBlock() {
    // Assumes '[' and 'Origin' have been consumed.
    consume(TokenType::RIGHT_BRACKET, "Expect ']'.");
    consume(TokenType::AT, "Expect '@'.");
    Token type = consume(TokenType::IDENTIFIER, "Expect origin type.");
    consume(TokenType::LEFT_BRACE, "Expect '{'.");
    int start = current;
    while(!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        advance();
    }
    int end = current;
    std::string content = source.substr(tokens[start].position, tokens[end-1].position + tokens[end-1].lexeme.length() - tokens[start].position);
    consume(TokenType::RIGHT_BRACE, "Expect '}'.");
    OriginType originType = OriginType::HTML;
    if (type.lexeme == "Style") originType = OriginType::STYLE;
    else if (type.lexeme == "JavaScript") originType = OriginType::JAVASCRIPT;
    return std::make_unique<OriginNode>(content, originType);
}

// --- Expression Parser (Full Implementation) ---
std::unique_ptr<Expr> CHTLParser::parseExpression() { return parseConditional(); }
std::unique_ptr<Expr> CHTLParser::parseConditional() {
    auto expr = parseLogicalOr();
    if (match({TokenType::QUESTION})) {
        auto then_branch = parseExpression();
        consume(TokenType::COLON, "Expect ':' after then branch.");
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
        if (check(TokenType::IDENTIFIER)) { unit = advance().lexeme; }
        else if (check(TokenType::PERCENT)) { unit = advance().lexeme; }
        try { return std::make_unique<LiteralExpr>(std::stod(number.lexeme), unit); }
        catch (const std::invalid_argument&) { error(number, "Invalid number format."); }
    }
    if (match({TokenType::IDENTIFIER})) {
        Token first_part = previous();
        if (first_part.lexeme == "true") {
            return std::make_unique<BoolExpr>(true);
        }
        if (first_part.lexeme == "false") {
            return std::make_unique<BoolExpr>(false);
        }
        if (check(TokenType::DOT)) {
            consume(TokenType::DOT, "Expect '.' after selector.");
            Token property = consume(TokenType::IDENTIFIER, "Expect property name.");
            return std::make_unique<ReferenceExpr>(first_part, property);
        } else if (check(TokenType::LEFT_PAREN)) {
            consume(TokenType::LEFT_PAREN, "Expect '('.");
            std::string key_name;
            while (!check(TokenType::RIGHT_PAREN) && !isAtEnd()) { key_name += advance().lexeme; }
            consume(TokenType::RIGHT_PAREN, "Expect ')'.");
            return std::make_unique<VarExpr>(first_part.lexeme, key_name);
        } else {
            return std::make_unique<ReferenceExpr>(Token(), first_part);
        }
    }
    if (check(TokenType::SYMBOL) && (peek().lexeme == "#" || peek().lexeme == ".")) {
        Token first_part = advance();
        if (tokens.size() > current && tokens[current + 1].type == TokenType::DOT) {
             Token selector_name = consume(TokenType::IDENTIFIER, "Expect selector name.");
             consume(TokenType::DOT, "Expect '.'.");
             Token property = consume(TokenType::IDENTIFIER, "Expect property name.");
             Token full_selector = {first_part.type, first_part.lexeme + selector_name.lexeme, first_part.line, first_part.position};
             return std::make_unique<ReferenceExpr>(full_selector, property);
        } else {
            std::string value = first_part.lexeme;
             while(check(TokenType::IDENTIFIER) || check(TokenType::NUMBER)) { value += advance().lexeme; }
            return std::make_unique<LiteralExpr>(0, value);
        }
    }
    if (match({TokenType::STRING})) { return std::make_unique<LiteralExpr>(0, previous().lexeme); }
    if (match({TokenType::LEFT_PAREN})) {
        auto expr = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expect ')'.");
        return expr;
    }
    error(peek(), "Expect expression.");
    return nullptr;
}


// --- Helper Method Implementations ---
bool CHTLParser::isAtEnd() { return peek().type == TokenType::END_OF_FILE; }
Token CHTLParser::peek() { return tokens[current]; }
Token CHTLParser::previous() { return tokens[current - 1]; }
Token CHTLParser::advance() { if (!isAtEnd()) current++; return previous(); }
bool CHTLParser::check(TokenType type) { if (isAtEnd()) return false; return peek().type == type; }
Token CHTLParser::consume(TokenType type, const std::string& message) { if (check(type)) return advance(); error(peek(), message); throw std::runtime_error(message); }
bool CHTLParser::match(const std::vector<TokenType>& types) { for (TokenType type : types) { if (check(type)) { advance(); return true; } } return false; }
void CHTLParser::error(const Token& token, const std::string& message) { std::cerr << "[line " << token.line << "] Error: " << message << std::endl; throw std::runtime_error(message); }

std::unique_ptr<ConfigNode> CHTLParser::preParseConfiguration() {
    int initial_pos = current;
    while (!isAtEnd() && peek().type != TokenType::END_OF_FILE) {
        if (peek().type == TokenType::LEFT_BRACKET) {
            if (tokens.size() > current + 1 && tokens[current + 1].type == TokenType::CONFIGURATION) {
                // Check if it's an unnamed config block
                if (tokens.size() > current + 2 && tokens[current + 2].type == TokenType::AT) {
                    // This is a named config block, skip it for now.
                    while(!check(TokenType::RIGHT_BRACE) && !isAtEnd()) advance();
                    consume(TokenType::RIGHT_BRACE, "Expect '}' to end named config block.");
                    continue;
                }
                // We need to advance past the `[` and `Configuration` tokens to parse the block
                advance(); // '['
                advance(); // 'Configuration'
                auto configNode = parseConfigurationBlock();
                current = initial_pos; // Reset for main parse
                return configNode;
            }
        }
        // If we hit anything that's not a top-level declaration, stop.
        if (peek().type != TokenType::LEFT_BRACKET) {
            break;
        }
        // Skip other top-level blocks like [Template] to find a [Configuration]
        advance(); // Consume '['
        while(peek().type != TokenType::RIGHT_BRACKET && !isAtEnd()) {
            advance();
        }
        consume(TokenType::RIGHT_BRACKET, "Expect ']' to close block header.");

        if (check(TokenType::LEFT_BRACE)) {
            consume(TokenType::LEFT_BRACE, "Expect '{' to start block body.");
            int brace_level = 1;
            while(brace_level > 0 && !isAtEnd()) {
                if(peek().type == TokenType::LEFT_BRACE) brace_level++;
                else if(peek().type == TokenType::RIGHT_BRACE) brace_level--;
                advance();
            }
        } else {
            // It's a declaration without a body, like an import. Just find the semicolon.
            while(!check(TokenType::SEMICOLON) && !isAtEnd()) advance();
            consume(TokenType::SEMICOLON, "Expect ';' after declaration.");
        }

    }
    current = initial_pos; // Reset for main parse
    return nullptr;
}


std::unique_ptr<ConfigNode> CHTLParser::parseConfigurationBlock() {
    // Assumes the [Configuration] part has been consumed.
    consume(TokenType::RIGHT_BRACKET, "Expect ']' to end keyword.");

    auto configNode = std::make_unique<ConfigNode>();
    consume(TokenType::LEFT_BRACE, "Expect '{' to start configuration body.");

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (peek().type == TokenType::LEFT_BRACKET && tokens[current + 1].type == TokenType::NAME) {
            consume(TokenType::LEFT_BRACKET, "Expect '['.");
            consume(TokenType::NAME, "Expect 'Name'.");
            consume(TokenType::RIGHT_BRACKET, "Expect ']'.");
            consume(TokenType::LEFT_BRACE, "Expect '{'.");
            while(!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                Token name_key = consume(TokenType::IDENTIFIER, "Expect name setting key.");
                consume(TokenType::EQUAL, "Expect '=' after name setting key.");
                std::vector<std::string> values;
                if (match({TokenType::LEFT_BRACKET})) { // Array of values
                    do {
                        std::string val;
                        if(check(TokenType::AT)) val += advance().lexeme;
                        Token name_val = advance();
                        if (name_val.type != TokenType::IDENTIFIER && !this->config->keyword_map.count(name_val.lexeme)) {
                            error(name_val, "Value must be an identifier or a known keyword.");
                        }
                        val += name_val.lexeme;
                        values.push_back(val);
                    } while(match({TokenType::COMMA}));
                    consume(TokenType::RIGHT_BRACKET, "Expect ']' to end value list.");
                } else { // Single value
                    std::string val;
                    if(check(TokenType::AT)) val += advance().lexeme;
                    Token name_val = advance();
                    if (name_val.type != TokenType::IDENTIFIER && !this->config->keyword_map.count(name_val.lexeme)) {
                        error(name_val, "Value must be an identifier or a known keyword.");
                    }
                    val += name_val.lexeme;
                    values.push_back(val);
                }
                configNode->name_settings[name_key.lexeme] = values;
                consume(TokenType::SEMICOLON, "Expect ';'.");
            }
            consume(TokenType::RIGHT_BRACE, "Expect '}'.");
        } else {
            Token key = consume(TokenType::IDENTIFIER, "Expect configuration key.");
            if (!match({TokenType::EQUAL, TokenType::COLON})) {
                error(peek(), "Expect '=' or ':' after configuration key.");
            }
            auto value = parseExpression();
            consume(TokenType::SEMICOLON, "Expect ';' after configuration value.");
            configNode->settings[key.lexeme] = std::move(value);
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' to end configuration body.");
    return configNode;
}

} // namespace CHTL
