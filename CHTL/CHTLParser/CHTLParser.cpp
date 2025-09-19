#include "CHTLParser.h"
#include "../CHTLLoader/CHTLLoader.h"
#include "../CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/OriginNode.h"
#include "CHTL/CHTLNode/DocumentNode.h"
#include "CHTL/CHTLNode/ScriptNode.h"
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

CHTLParser::CHTLParser(const std::string& source, const std::vector<Token>& tokens, const std::string& file_path)
    : source(source), tokens(tokens), file_path(file_path) {
        this->current_namespace = getFilename(file_path);
}

const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& CHTLParser::getTemplateDefinitions() const { return template_definitions; }
std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& CHTLParser::getMutableTemplateDefinitions() { return template_definitions; }


std::unique_ptr<BaseNode> CHTLParser::parse() {
    auto docNode = std::make_unique<DocumentNode>();
    while (!isAtEnd() && peek().type != TokenType::END_OF_FILE) {
        if (peek().type == TokenType::LEFT_BRACKET) {
            if (tokens.size() > current + 1 && (tokens[current + 1].lexeme == "Template" || tokens[current + 1].lexeme == "Custom")) {
                // Templates modify parser state but don't produce a renderable node in the main tree.
                parseSymbolDeclaration(tokens[current + 1].lexeme == "Custom");
            } else if (tokens.size() > current + 1 && tokens[current + 1].lexeme == "Import") {
                // Imports also modify parser state.
                parseImportStatement();
            } else {
                // For other bracket types like [Origin], treat them as declarations.
                for (auto& node : parseDeclaration()) {
                    docNode->addChild(std::move(node));
                }
            }
        } else {
            // This handles root elements like `body` or `html`, or `style` blocks.
            for (auto& node : parseDeclaration()) {
                docNode->addChild(std::move(node));
            }
        }
    }
    return docNode;
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
    if (check(TokenType::IDENTIFIER) && peek().lexeme == "script") {
        advance(); // Consume the 'script' token
        std::vector<std::unique_ptr<BaseNode>> nodes;
        nodes.push_back(parseScriptBlock());
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

    auto value_expr = parseExpression();

    consume(TokenType::SEMICOLON, "Expect ';' after attribute value.");
    element->addAttribute({key.lexeme, std::move(value_expr)});
}

std::unique_ptr<StyleNode> CHTLParser::parseStyleBlock() {
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'style' keyword.");
    auto styleNode = std::make_unique<StyleNode>();

    // Check for a single placeholder token from the Unified Scanner
    if (peek().type == TokenType::IDENTIFIER && previous().type == TokenType::LEFT_BRACE) {
        const std::string& lexeme = peek().lexeme;
        if (lexeme.find("__CHTL_CSS_PLACEHOLDER_") != std::string::npos) {
            styleNode->placeholder_key = lexeme;
            advance(); // Consume placeholder token
            consume(TokenType::RIGHT_BRACE, "Expect '}' after placeholder in style block.");
            return styleNode;
        }
    }

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
    consume(TokenType::LEFT_BRACKET, "Expect '[' to start declaration.");
    Token keyword = consume(TokenType::IDENTIFIER, "Expect 'Template' or 'Custom' keyword.");
    if (is_custom && keyword.lexeme != "Custom") { error(keyword, "Expected 'Custom' keyword."); }
    else if (!is_custom && keyword.lexeme != "Template") { error(keyword, "Expected 'Template' keyword."); }
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
            if (match({TokenType::INHERIT})) {
                consume(TokenType::AT, "Expect '@' after 'inherit'.");
                Token type = consume(TokenType::IDENTIFIER, "Expect template type, e.g. 'Style'.");
                Token name = consume(TokenType::IDENTIFIER, "Expect template name.");
                consume(TokenType::SEMICOLON, "Expect ';' after inherit statement.");

                InheritanceInfo info;
                if (type.lexeme == "Style") info.type = TemplateType::STYLE;
                else if (type.lexeme == "Element") info.type = TemplateType::ELEMENT;
                else if (type.lexeme == "Var") info.type = TemplateType::VAR;
                else error(type, "Unknown template type for inheritance.");
                info.name = name.lexeme;
                def.inherits.push_back(info);
            } else if (match({TokenType::DELETE})) {
                if (!def.is_custom) {
                    error(previous(), "'delete' is only allowed in a [Custom] template.");
                }
                do {
                    def.deleted_properties.push_back(consume(TokenType::IDENTIFIER, "Expect property name to delete.").lexeme);
                } while (match({TokenType::COMMA}));
                consume(TokenType::SEMICOLON, "Expect ';' after delete statement.");
            } else {
                // If not an inherit or delete statement, it must be a property definition.
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
    } else if (def.type == TemplateType::VAR) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            Token key = consume(TokenType::IDENTIFIER, "Expect variable name.");
            consume(TokenType::COLON, "Expect ':' after variable name.");
            auto value_expr = parseExpression();
            consume(TokenType::SEMICOLON, "Expect ';' after variable value.");
            def.variables[key.lexeme] = std::move(value_expr);
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' to end symbol body.");
    template_definitions[current_namespace][def.name] = std::move(def);
}

void CHTLParser::parseImportStatement() {
    consume(TokenType::LEFT_BRACKET, "Expect '['.");
    consume(TokenType::IDENTIFIER, "Expect 'Import'.");
    consume(TokenType::RIGHT_BRACKET, "Expect ']'.");
    consume(TokenType::AT, "Expect '@'.");
    Token typeToken = consume(TokenType::IDENTIFIER, "Expect import type.");
    if (typeToken.lexeme == "Chtl") {
        consume(TokenType::FROM, "Expect 'from'.");
        Token pathToken = consume(TokenType::STRING, "Expect file path.");
        std::string imported_content = CHTLLoader::load(this->file_path, pathToken.lexeme);
        std::string imported_path = FileSystem::getDirectory(this->file_path) + pathToken.lexeme;
        CHTLLexer sub_lexer(imported_content);
        std::vector<Token> sub_tokens = sub_lexer.scanTokens();
        CHTLParser sub_parser(imported_content, sub_tokens, imported_path);
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
    // Basic implementation, doesn't handle nested braces
    consume(TokenType::LEFT_BRACKET, "Expect '['.");
    consume(TokenType::IDENTIFIER, "Expect 'Origin'.");
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

std::unique_ptr<BaseNode> CHTLParser::parseScriptBlock() {
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'script' keyword.");

    // Check for a single placeholder token from the Unified Scanner
    if (peek().type == TokenType::IDENTIFIER && previous().type == TokenType::LEFT_BRACE) {
        const std::string& lexeme = peek().lexeme;
        if (lexeme.find("__CHTL_JS_PLACEHOLDER_") != std::string::npos) {
            auto scriptNode = std::make_unique<ScriptNode>("");
            scriptNode->placeholder_key = lexeme;
            advance(); // Consume placeholder token
            consume(TokenType::RIGHT_BRACE, "Expect '}' after placeholder in script block.");
            return scriptNode;
        }
    }

    // This logic is simple and doesn't use the lexer, so it won't handle
    // comments or strings with braces correctly. Fine for a first pass.
    int start = current;
    int brace_level = 1;
    while(brace_level > 0 && !isAtEnd()) {
        if(peek().type == TokenType::LEFT_BRACE) brace_level++;
        else if(peek().type == TokenType::RIGHT_BRACE) brace_level--;

        if (brace_level > 0) advance();
    }
    int end = current;

    std::string content;
    if (end > start) {
        // Reconstruct the content string from the original source
        int content_start_pos = tokens[start].position;
        const auto& end_token = tokens[end - 1];
        int content_end_pos = end_token.position + end_token.lexeme.length();
        content = source.substr(content_start_pos, content_end_pos - content_start_pos);
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after script block.");
    return std::make_unique<ScriptNode>(content);
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
            // A lone identifier is a reference to a property on the current element.
            // The selector token is default-constructed, which is how the evaluator
            // identifies a self-reference.
            return std::make_unique<ReferenceExpr>(Token(), first_part);
        }
    }
    if (check(TokenType::SYMBOL) && (peek().lexeme == "#" || peek().lexeme == ".")) {
        Token symbol = advance(); // Consume '#' or '.'
        Token selector_name = consume(TokenType::IDENTIFIER, "Expect selector name after '#' or '.'.");

        Token full_selector_token = {
            symbol.type,
            symbol.lexeme + selector_name.lexeme,
            symbol.line,
            symbol.position
        };

        if (match({TokenType::DOT})) {
            Token property = consume(TokenType::IDENTIFIER, "Expect property name after '.'.");
            return std::make_unique<ReferenceExpr>(full_selector_token, property);
        } else {
            // This case would be a standalone selector, e.g. `{{#box}}` in CHTL JS
            // In a style expression, it's not valid on its own.
            error(peek(), "Standalone selector not valid in property expression.");
            return nullptr;
        }
    }
    if (match({TokenType::STRING})) {
        // The lexer provides the string content without quotes.
        return std::make_unique<LiteralExpr>(previous().lexeme);
    }
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

} // namespace CHTL
