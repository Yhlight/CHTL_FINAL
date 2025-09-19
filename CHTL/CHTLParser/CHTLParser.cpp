#include "CHTLParser.h"
#include "../CHTLLoader/CHTLLoader.h"
#include "../CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLNode/RootNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/OriginNode.h"
#include "CHTL/CHTLNode/CommentNode.h"
#include "CHTL/CHTLNode/ImportNode.h"
#include "CHTL/CHTLNode/NamespaceNode.h"
#include "CHTL/CHTLNode/ConfigNode.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <stdexcept>

namespace CHTL {

// --- Recursive Helpers for Specialization (will be used later) ---
// (These helpers are preserved from the original file but may need refactoring)
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
                    if (position == TokenType::KEYWORD_AFTER) {
                        nodes.insert(std::next(it), std::make_move_iterator(nodes_to_insert.begin()), std::make_move_iterator(nodes_to_insert.end()));
                    } else if (position == TokenType::KEYWORD_BEFORE) {
                        nodes.insert(it, std::make_move_iterator(nodes_to_insert.begin()), std::make_move_iterator(nodes_to_insert.end()));
                    } else if (position == TokenType::KEYWORD_REPLACE) {
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
        namespace_stack.push_back(getFilename(file_path));
}

std::string CHTLParser::getCurrentNamespace() {
    if (namespace_stack.empty()) {
        // This case should ideally not be hit if constructor initializes it.
        return "global";
    }
    return namespace_stack.back();
}

const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& CHTLParser::getTemplateDefinitions() const { return template_definitions; }
std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& CHTLParser::getMutableTemplateDefinitions() { return template_definitions; }

void CHTLParser::skipComments() {
    while(peek().type == TokenType::COMMENT) {
        advance();
    }
}

std::unique_ptr<RootNode> CHTLParser::parse() {
    auto root = std::make_unique<RootNode>();
    while (!isAtEnd()) {
        skipComments();
        if(isAtEnd()) break;
        auto declarations = parseTopLevelDeclaration();
        for (auto& decl : declarations) {
            root->addChild(std::move(decl));
        }
    }
    return root;
}

std::vector<std::unique_ptr<BaseNode>> CHTLParser::parseTopLevelDeclaration() {
    std::vector<std::unique_ptr<BaseNode>> nodes;

    skipComments();

    if (peek().type == TokenType::HASH) {
        nodes.push_back(parseComment());
        return nodes;
    }

    switch (peek().type) {
        case TokenType::KEYWORD_TEMPLATE:
            parseSymbolDeclaration(false);
            break;
        case TokenType::KEYWORD_CUSTOM:
            parseSymbolDeclaration(true);
            break;
        case TokenType::KEYWORD_ORIGIN:
            nodes.push_back(parseOriginBlock());
            break;
        case TokenType::KEYWORD_IMPORT:
            nodes.push_back(parseImportStatement());
            break;
        case TokenType::KEYWORD_NAMESPACE:
            nodes.push_back(parseNamespace());
            break;
        case TokenType::KEYWORD_CONFIGURATION:
            nodes.push_back(parseConfig());
            break;
        case TokenType::KEYWORD_TEXT: {
            consume(TokenType::KEYWORD_TEXT, "Expect 'text'.");
            consume(TokenType::LEFT_BRACE, "Expect '{' after 'text'.");
            Token content = consume(TokenType::STRING, "Expect string literal."); // Should also support unquoted
            consume(TokenType::RIGHT_BRACE, "Expect '}' after text block.");
            nodes.push_back(std::make_unique<TextNode>(content.lexeme));
            break;
        }
        case TokenType::KEYWORD_STYLE:
            nodes.push_back(parseStyleBlock());
            break;
        case TokenType::KEYWORD_SCRIPT:
            nodes.push_back(parseScriptBlock());
            break;
        case TokenType::AT:
            // This is simplistic, a better way would be to peek ahead to see if it's @Element, @Style etc.
            nodes = parseElementTemplateUsage();
            break;
        case TokenType::IDENTIFIER:
             nodes.push_back(parseElement());
             break;
        case TokenType::END_OF_FILE:
        case TokenType::RIGHT_BRACE: // Let the parent handle this
            break;
        default:
            error(peek(), "Unexpected token at top level.");
            advance();
            break;
    }
    return nodes;
}

std::vector<std::unique_ptr<BaseNode>> CHTLParser::parseDeclaration() {
    return parseTopLevelDeclaration();
}


// --- NEW PARSING FUNCTIONS ---

std::unique_ptr<CommentNode> CHTLParser::parseComment() {
    Token hash = consume(TokenType::HASH, "Expect '#'.");
    int start_pos = hash.position + hash.lexeme.length();
    int end_pos = start_pos;
    while(end_pos < source.length() && source[end_pos] != '\n' && source[end_pos] != '\r') {
        end_pos++;
    }
    while(!isAtEnd() && peek().line == hash.line) {
        advance();
    }
    auto node = std::make_unique<CommentNode>();
    node->comment = source.substr(start_pos, end_pos - start_pos);
    return node;
}

std::unique_ptr<NamespaceNode> CHTLParser::parseNamespace() {
    consume(TokenType::KEYWORD_NAMESPACE, "Expect '[Namespace]'.");
    auto node = std::make_unique<NamespaceNode>();
    node->name = consume(TokenType::IDENTIFIER, "Expect namespace name.").lexeme;

    namespace_stack.push_back(node->name);

    if(match({TokenType::LEFT_BRACE})) {
        while(!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            auto children = parseTopLevelDeclaration();
            for(auto& child : children) {
                node->addChild(std::move(child));
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' to close namespace.");
    }

    namespace_stack.pop_back();
    return node;
}

std::unique_ptr<ConfigNode> CHTLParser::parseConfig() {
    consume(TokenType::KEYWORD_CONFIGURATION, "Expect '[Configuration]'.");
    auto node = std::make_unique<ConfigNode>();
    if(check(TokenType::AT)) {
        consume(TokenType::AT, "Expect '@'.");
        node->name = consume(TokenType::IDENTIFIER, "Expect configuration name.").lexeme;
    }
    consume(TokenType::LEFT_BRACE, "Expect '{' for configuration body.");
    // TODO: Parse settings inside
    while(!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        advance();
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' to close configuration.");
    return node;
}


// --- EXISTING PARSING LOGIC (PRESERVED & TO BE REFACTORED) ---

std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    Token tagName = consume(TokenType::IDENTIFIER, "Expect element name.");
    auto element = std::make_unique<ElementNode>(tagName.lexeme);
    consume(TokenType::LEFT_BRACE, "Expect '{' after element name.");
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        skipComments();
        if ((peek().type == TokenType::IDENTIFIER || peek().type == TokenType::KEYWORD_TEXT) && tokens.size() > current + 1 && tokens[current + 1].type == TokenType::COLON) {
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
    if (!check(TokenType::IDENTIFIER) && !check(TokenType::KEYWORD_TEXT)) {
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
    consume(TokenType::KEYWORD_STYLE, "Expect 'style'.");
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'style' keyword.");
    auto styleNode = std::make_unique<StyleNode>();

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        skipComments();
        if (check(TokenType::RIGHT_BRACE)) break;

        // Lookahead to distinguish property from rule
        int lookahead = 0;
        bool isRule = false;
        while(current + lookahead < tokens.size()) {
            TokenType type = tokens[current + lookahead].type;
            if (type == TokenType::LEFT_BRACE) {
                isRule = true;
                break;
            }
            if (type == TokenType::COLON || type == TokenType::SEMICOLON) {
                isRule = false;
                break;
            }
            lookahead++;
        }

        if (isRule) {
            styleNode->global_rules.push_back(parseCssRule());
        } else {
            Token keyToken = consume(TokenType::IDENTIFIER, "Expect style property name.");
            consume(TokenType::COLON, "Expect ':' after property name.");
            auto valueExpr = parseExpression();
            consume(TokenType::SEMICOLON, "Expect ';' after property value.");
            styleNode->direct_properties.emplace_back(keyToken.lexeme, std::move(valueExpr));
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after style block.");
    return styleNode;
}

CssRuleNode CHTLParser::parseCssRule() {
    CssRuleNode rule;
    // This is a simplification. A real CSS selector can be very complex.
    // For now, we consume tokens until we hit the brace.
    while(!check(TokenType::LEFT_BRACE) && !isAtEnd()) {
        rule.selector += advance().lexeme;
    }
    consume(TokenType::LEFT_BRACE, "Expect '{' to start CSS rule block.");

    while(!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        skipComments();
        if(check(TokenType::RIGHT_BRACE)) break;

        Token keyToken = consume(TokenType::IDENTIFIER, "Expect style property name.");
        consume(TokenType::COLON, "Expect ':' after property name.");
        auto valueExpr = parseExpression();
        consume(TokenType::SEMICOLON, "Expect ';' after property value.");
        rule.properties.emplace_back(keyToken.lexeme, std::move(valueExpr));
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' to end CSS rule block.");
    return rule;
}

void CHTLParser::parseStyleTemplateUsage(StyleNode* styleNode) {
}

std::vector<std::unique_ptr<BaseNode>> CHTLParser::parseElementTemplateUsage() {
    consume(TokenType::AT, "Expect '@'.");
    Token type = consume(TokenType::IDENTIFIER, "Expect template type.");
    if (type.lexeme != "Element") {
        error(type, "Expected '@Element' usage.");
        return {};
    }
    Token name = consume(TokenType::IDENTIFIER, "Expect template name.");

    const TemplateDefinitionNode* def = nullptr;
    // Simple lookup in current namespace for now
    std::string current_ns = getCurrentNamespace();
    if (template_definitions.count(current_ns) && template_definitions.at(current_ns).count(name.lexeme)) {
        def = &template_definitions.at(current_ns).at(name.lexeme);
    }

    if (!def) {
        error(name, "Element template '" + name.lexeme + "' not found.");
        return {};
    }
    if (def->type != TemplateType::ELEMENT) {
        error(name, "Template '" + name.lexeme + "' is not an Element template.");
        return {};
    }

    std::vector<std::unique_ptr<BaseNode>> cloned_nodes;
    for (const auto& node : def->element_body) {
        cloned_nodes.push_back(node->clone());
    }

    // TODO: Implement specialization for [Custom] templates
    if (match({TokenType::LEFT_BRACE})) {
        if (!def->is_custom) {
            error(name, "Cannot specialize a non-custom [Template].");
        }
        while(!check(TokenType::RIGHT_BRACE) && !isAtEnd()) advance(); // Stub
        consume(TokenType::RIGHT_BRACE, "Expect '}' after specialization.");
    } else {
        consume(TokenType::SEMICOLON, "Expect ';' after template usage.");
    }

    return cloned_nodes;
}

void CHTLParser::parseSymbolDeclaration(bool is_custom) {
    if(is_custom) consume(TokenType::KEYWORD_CUSTOM, "Expect [Custom].");
    else consume(TokenType::KEYWORD_TEMPLATE, "Expect [Template].");

    consume(TokenType::AT, "Expect '@'.");
    Token typeToken = consume(TokenType::IDENTIFIER, "Expect symbol type, e.g., 'Element'.");

    TemplateDefinitionNode def;
    def.is_custom = is_custom;
    def.name = consume(TokenType::IDENTIFIER, "Expect symbol name.").lexeme;

    if (typeToken.lexeme == "Element") {
        def.type = TemplateType::ELEMENT;
        consume(TokenType::LEFT_BRACE, "Expect '{' to start element template body.");
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            for (auto& node : parseDeclaration()) {
                def.element_body.push_back(std::move(node));
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' to end element template body.");
    } else if (typeToken.lexeme == "Style") {
        // TODO: Implement Style template parsing
    } else if (typeToken.lexeme == "Var") {
        // TODO: Implement Var template parsing
    } else {
        error(typeToken, "Unknown template type.");
    }

    template_definitions[getCurrentNamespace()][def.name] = std::move(def);
}

std::unique_ptr<ImportNode> CHTLParser::parseImportStatement() {
    consume(TokenType::KEYWORD_IMPORT, "Expect '[Import]'.");
    auto node = std::make_unique<ImportNode>();
    if (check(TokenType::LEFT_BRACKET)) {
    }
    consume(TokenType::AT, "Expect '@'.");
    node->info.import_specifier = consume(TokenType::IDENTIFIER, "Expect import type.").lexeme;

    if(node->info.import_specifier == "Chtl") {
        consume(TokenType::KEYWORD_FROM, "Expect 'from'.");
        node->info.path = consume(TokenType::STRING, "Expect file path.").lexeme;
    }

    if (match({TokenType::KEYWORD_AS})) {
        node->info.alias = consume(TokenType::IDENTIFIER, "Expect alias name.").lexeme;
    }

    // Semicolon is optional in some cases, for now we require it
    if(peek().type == TokenType::SEMICOLON) {
        consume(TokenType::SEMICOLON, "Expect ';' after import statement.");
    }
    return node;
}

std::unique_ptr<ScriptNode> CHTLParser::parseScriptBlock() {
    consume(TokenType::KEYWORD_SCRIPT, "Expect 'script'.");
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'script' keyword.");

    if (check(TokenType::RIGHT_BRACE)) {
        consume(TokenType::RIGHT_BRACE, "Expect '}' after script block.");
        return std::make_unique<ScriptNode>("");
    }

    Token start_token = peek();
    int brace_level = 1;

    while (brace_level > 0 && !isAtEnd()) {
        if (peek().type == TokenType::LEFT_BRACE) brace_level++;
        if (peek().type == TokenType::RIGHT_BRACE) brace_level--;

        if (brace_level == 0) {
            break;
        }
        advance();
    }

    int start_pos = start_token.position;
    int end_pos = previous().position + previous().lexeme.length();
    std::string content = source.substr(start_pos, end_pos - start_pos);

    consume(TokenType::RIGHT_BRACE, "Expect '}' after script block.");
    return std::make_unique<ScriptNode>(content);
}

std::unique_ptr<BaseNode> CHTLParser::parseOriginBlock() {
    consume(TokenType::KEYWORD_ORIGIN, "Expect '[Origin]'.");
    consume(TokenType::AT, "Expect '@'.");
    Token type = consume(TokenType::IDENTIFIER, "Expect origin type.");
    consume(TokenType::LEFT_BRACE, "Expect '{'.");

    int start_pos = peek().position;
    while(!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        advance();
    }
    int end_pos = previous().position + previous().lexeme.length();
    std::string content = source.substr(start_pos, end_pos - start_pos);

    consume(TokenType::RIGHT_BRACE, "Expect '}'.");
    OriginType originType = OriginType::HTML;
    if (type.lexeme == "Style") originType = OriginType::STYLE;
    else if (type.lexeme == "JavaScript") originType = OriginType::JAVASCRIPT;
    return std::make_unique<OriginNode>(content, originType);
}


// --- Expression Parser (Preserved) ---
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
            return std::make_unique<ReferenceExpr>(Token(), first_part);
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
    std::cerr << "[line " << token.line << "] Error at '" << token.lexeme << "': " << message << std::endl;
    // In a real parser, we would have synchronization logic here.
}

} // namespace CHTL
