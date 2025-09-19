#include "CHTLParser.h"
#include "../CHTLLoader/CHTLLoader.h"
#include "../CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/OriginNode.h"
#include "CHTL/CHTLNode/NamespaceNode.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <sstream>
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

#include "CHTL/CHTLNode/ConfigNode.h"

CHTLParser::CHTLParser(const std::string& source, const std::vector<Token>& tokens, const std::string& file_path, std::shared_ptr<Configuration> config)
    : source(source), tokens(tokens), file_path(file_path), config(config) {
    // The namespace stack starts empty. It will be populated either by an explicit
    // [Namespace] tag or by the default filename-based namespace before the root element is parsed.
}

std::string CHTLParser::getCurrentNamespace() {
    if (namespace_stack.empty()) {
        return "";
    }
    std::stringstream ss;
    for (size_t i = 0; i < namespace_stack.size(); ++i) {
        ss << namespace_stack[i];
        if (i < namespace_stack.size() - 1) {
            ss << ".";
        }
    }
    return ss.str();
}

const std::map<std::string, std::map<std::string, std::shared_ptr<TemplateDefinitionNode>>>& CHTLParser::getTemplateDefinitions() const { return template_definitions; }
std::map<std::string, std::map<std::string, std::shared_ptr<TemplateDefinitionNode>>>& CHTLParser::getMutableTemplateDefinitions() { return template_definitions; }
bool CHTLParser::getUseHtml5Doctype() const { return use_html5_doctype; }


std::unique_ptr<BaseNode> CHTLParser::parse() {
    if (check(TokenType::USE)) {
        if (tokens.size() > current + 2 && tokens[current + 1].type == TokenType::IDENTIFIER && tokens[current + 1].lexeme == "html5" && tokens[current + 2].type == TokenType::SEMICOLON) {
            this->use_html5_doctype = true;
            advance(); advance(); advance();
        }
    }

    while (!isAtEnd() && peek().type == TokenType::LEFT_BRACKET) {
        if (tokens.size() > current + 1) {
            const std::string& keyword = tokens[current + 1].lexeme;
            if (keyword == "Template") {
                parseSymbolDeclaration(false);
            } else if (keyword == "Custom") {
                parseSymbolDeclaration(true);
            } else if (keyword == "Import") {
                parseImportStatement();
            } else if (keyword == "Configuration") {
                parseConfigurationBlock();
            } else if (keyword == "Namespace") {
                parseNamespaceBlock();
            } else {
                break;
            }
        } else {
            break;
        }
    }

    if (!isAtEnd() && peek().type != TokenType::END_OF_FILE) {
        if (namespace_stack.empty() && !config->disable_default_namespace) {
            namespace_stack.push_back(getFilename(file_path));
        }
        auto nodes = parseDeclaration();
        if (nodes.size() == 1) {
            return std::move(nodes[0]);
        } else {
            error(peek(), "Expected a single root element declaration after top-level definitions.");
        }
    }
    return nullptr;
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
    if (match({TokenType::SCRIPT})) {
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
            std::string raw_value;
            while (!check(TokenType::SEMICOLON) && !isAtEnd()) {
                raw_value += advance().lexeme;
                if (!check(TokenType::SEMICOLON)) raw_value += " ";
            }
            auto value_expr = std::make_unique<LiteralExpr>(0, raw_value);
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
                std::string raw_value;
                while (!check(TokenType::SEMICOLON) && !isAtEnd()) {
                    raw_value += advance().lexeme;
                    if (!check(TokenType::SEMICOLON)) raw_value += " ";
                }
                auto value_expr = std::make_unique<LiteralExpr>(0, raw_value);
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

    std::string target_namespace;
    if (match({TokenType::FROM})) {
        target_namespace += consume(TokenType::IDENTIFIER, "Expect namespace name.").lexeme;
        while (match({TokenType::DOT})) {
            target_namespace += ".";
            target_namespace += consume(TokenType::IDENTIFIER, "Expect namespace name part.").lexeme;
        }
    }

    std::shared_ptr<const TemplateDefinitionNode> def = nullptr;
    if (!target_namespace.empty()) {
        if (template_definitions.count(target_namespace) && template_definitions.at(target_namespace).count(name.lexeme)) {
            def = template_definitions.at(target_namespace).at(name.lexeme);
        }
    } else {
        std::string current_ns = getCurrentNamespace();
        if (!current_ns.empty() && template_definitions.count(current_ns) && template_definitions.at(current_ns).count(name.lexeme)) {
            def = template_definitions.at(current_ns).at(name.lexeme);
        } else {
            std::vector<std::shared_ptr<const TemplateDefinitionNode>> found;
            for (const auto& ns_pair : template_definitions) {
                if (ns_pair.first == current_ns) continue;
                if (ns_pair.second.count(name.lexeme)) { found.push_back(ns_pair.second.at(name.lexeme)); }
            }
            if (found.size() == 1) { def = found[0]; }
            else if (found.size() > 1) { error(name, "Ambiguous style template reference '" + name.lexeme + "'."); }
        }
    }

    if (!def) { error(name, "Style template '" + name.lexeme + "' not found."); }
    if (def->type != TemplateType::STYLE) { error(name, "Template '" + name.lexeme + "' is not a Style template."); }

    StyleNode::StyleApplication app;
    app.definition = def;

    if (match({TokenType::LEFT_BRACE})) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            if (match({TokenType::DELETE})) {
                do {
                    app.deleted_properties.push_back(consume(TokenType::IDENTIFIER, "Expect property name after 'delete'.").lexeme);
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

    std::string target_namespace;
    if (match({TokenType::FROM})) {
        target_namespace += consume(TokenType::IDENTIFIER, "Expect namespace name.").lexeme;
        while (match({TokenType::DOT})) {
            target_namespace += ".";
            target_namespace += consume(TokenType::IDENTIFIER, "Expect namespace name part.").lexeme;
        }
    }

    std::shared_ptr<const TemplateDefinitionNode> def = nullptr;
    if (!target_namespace.empty()) {
        if (template_definitions.count(target_namespace) && template_definitions.at(target_namespace).count(name.lexeme)) {
            def = template_definitions.at(target_namespace).at(name.lexeme);
        }
    } else {
        std::string current_ns = getCurrentNamespace();
        if (!current_ns.empty() && template_definitions.count(current_ns) && template_definitions.at(current_ns).count(name.lexeme)) {
            def = template_definitions.at(current_ns).at(name.lexeme);
        } else {
            std::vector<std::shared_ptr<const TemplateDefinitionNode>> found;
            for (const auto& ns_pair : template_definitions) {
                if (ns_pair.first == current_ns) continue;
                if (ns_pair.second.count(name.lexeme)) {
                    found.push_back(ns_pair.second.at(name.lexeme));
                }
            }
            if (found.size() == 1) { def = found[0]; }
            else if (found.size() > 1) { error(name, "Ambiguous element template reference '" + name.lexeme + "'. Use 'from' to specify the namespace."); }
        }
    }

    if (!def) {
        std::string err_msg = "Element template '" + name.lexeme + "' not found.";
        if (!target_namespace.empty()) { err_msg += " in namespace '" + target_namespace + "'."; }
        error(name, err_msg);
        return {};
    }
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
                    index = std::stoi(consume(TokenType::NUMBER, "Expect index.").lexeme);
                    consume(TokenType::RIGHT_BRACKET, "Expect ']'.");
                }
                if (!findAndDelete(cloned_nodes, element_to_delete.lexeme, index)) {
                    error(element_to_delete, "Could not find element to delete.");
                }
                consume(TokenType::SEMICOLON, "Expect ';'.");
            } else if (match({TokenType::INSERT})) {
                Token position = advance();
                Token selector = consume(TokenType::IDENTIFIER, "Expect tag name.");
                int index = 0;
                if (match({TokenType::LEFT_BRACKET})) {
                    index = std::stoi(consume(TokenType::NUMBER, "Expect index.").lexeme);
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

    auto def = std::make_shared<TemplateDefinitionNode>();
    def->is_custom = is_custom;

    if (typeToken.lexeme == "Style") { def->type = TemplateType::STYLE; }
    else if (typeToken.lexeme == "Element") { def->type = TemplateType::ELEMENT; }
    else if (typeToken.lexeme == "Var") { def->type = TemplateType::VAR; }
    else { error(typeToken, "Unknown symbol type."); }

    def->name = consume(TokenType::IDENTIFIER, "Expect symbol name.").lexeme;
    consume(TokenType::LEFT_BRACE, "Expect '{' to start symbol body.");
    if (def->type == TemplateType::STYLE) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            if (check(TokenType::AT) || check(TokenType::INHERIT)) {
                // ... inheritance logic ...
            } else {
                std::string key_str;
                while (!check(TokenType::COLON) && !isAtEnd()) { key_str += advance().lexeme; }
                consume(TokenType::COLON, "Expect ':'.");
                auto value_expr = parseExpression();
                consume(TokenType::SEMICOLON, "Expect ';'.");
                def->style_properties.push_back({key_str, std::move(value_expr)});
            }
        }
    } else if (def->type == TemplateType::ELEMENT) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
             if ((peek().type == TokenType::IDENTIFIER || peek().type == TokenType::TEXT) && tokens.size() > current + 1 && tokens[current + 1].type == TokenType::COLON) {
                Token key = advance();
                consume(TokenType::COLON, "Expect ':' after attribute name.");
                Token value_token;
                if (match({TokenType::STRING, TokenType::IDENTIFIER, TokenType::NUMBER})) {
                    value_token = previous();
                } else {
                    error(peek(), "Expect attribute value.");
                }
                consume(TokenType::SEMICOLON, "Expect ';' after attribute value.");
                def->element_attributes.push_back({key.lexeme, value_token.lexeme});
            } else {
                for (auto& node : parseDeclaration()) {
                    def->element_body.push_back(std::move(node));
                }
            }
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' to end symbol body.");
    template_definitions[getCurrentNamespace()][def->name] = def;
}

std::unique_ptr<NamespaceNode> CHTLParser::parseNamespaceBlock() {
    consume(TokenType::LEFT_BRACKET, "Expect '['.");
    consume(TokenType::IDENTIFIER, "Expect 'Namespace'.");
    consume(TokenType::RIGHT_BRACKET, "Expect ']'.");
    Token name = consume(TokenType::IDENTIFIER, "Expect namespace name.");
    auto node = std::make_unique<NamespaceNode>(name.lexeme);

    namespace_stack.push_back(name.lexeme);

    if (match({TokenType::LEFT_BRACE})) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            if (peek().type == TokenType::LEFT_BRACKET && tokens.size() > current + 1) {
                const std::string& keyword = tokens[current + 1].lexeme;
                if (keyword == "Template") {
                    parseSymbolDeclaration(false);
                } else if (keyword == "Custom") {
                    parseSymbolDeclaration(true);
                } else if (keyword == "Namespace") {
                    auto ns_node = parseNamespaceBlock();
                    if (ns_node) {
                        node->children.push_back(std::move(ns_node));
                    }
                } else {
                     node->children.push_back(parseElement());
                }
            } else {
                for (auto& child : parseDeclaration()) {
                    node->children.push_back(std::move(child));
                }
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}'.");
    }

    namespace_stack.pop_back();
    return node;
}

void CHTLParser::parseImportStatement() {
    consume(TokenType::LEFT_BRACKET, "Expect '['.");
    consume(TokenType::IDENTIFIER, "Expect 'Import'.");
    consume(TokenType::RIGHT_BRACKET, "Expect ']'.");

    bool is_custom_import = false;
    bool is_template_import = false;

    if (peek().type == TokenType::LEFT_BRACKET) {
        advance(); // consume [
        Token specifier = consume(TokenType::IDENTIFIER, "Expect 'Custom' or 'Template'.");
        if (specifier.lexeme == "Custom") is_custom_import = true;
        else if (specifier.lexeme == "Template") is_template_import = true;
        else error(specifier, "Invalid import specifier.");
        consume(TokenType::RIGHT_BRACKET, "Expect ']'.");
    }

    consume(TokenType::AT, "Expect '@'.");
    Token typeToken = consume(TokenType::IDENTIFIER, "Expect import type.");

    if (typeToken.lexeme == "Chtl") {
        consume(TokenType::FROM, "Expect 'from'.");
        Token pathToken = consume(TokenType::STRING, "Expect file path.");
        std::string imported_content = CHTLLoader::load(this->file_path, pathToken.lexeme);
        std::string imported_path = FileSystem::getDirectory(this->file_path) + pathToken.lexeme;
        CHTLLexer sub_lexer(imported_content, this->config);
        std::vector<Token> sub_tokens = sub_lexer.scanTokens();
        CHTLParser sub_parser(imported_content, sub_tokens, imported_path, this->config);
        sub_parser.parse();
        auto& imported_template_map = sub_parser.getMutableTemplateDefinitions();
        for (auto& pair : imported_template_map) {
            this->template_definitions[pair.first] = std::move(pair.second);
        }
    } else {
        std::vector<std::string> symbols_to_import;
        if (peek().type != TokenType::FROM) {
            do {
                symbols_to_import.push_back(consume(TokenType::IDENTIFIER, "Expect symbol name.").lexeme);
            } while (match({TokenType::COMMA}));
        }

        consume(TokenType::FROM, "Expect 'from'.");
        Token pathToken = consume(TokenType::STRING, "Expect file path.");

        std::string alias;
        if (match({TokenType::AS})) {
            alias = consume(TokenType::IDENTIFIER, "Expect alias name.").lexeme;
            if (symbols_to_import.size() != 1 && !alias.empty()) {
                error(previous(), "Cannot use 'as' with multiple symbol imports.");
            }
        }

        std::string imported_content = CHTLLoader::load(this->file_path, pathToken.lexeme);
        std::string imported_path = FileSystem::getDirectory(this->file_path) + pathToken.lexeme;
        CHTLLexer sub_lexer(imported_content, this->config);
        std::vector<Token> sub_tokens = sub_lexer.scanTokens();
        CHTLParser sub_parser(imported_content, sub_tokens, imported_path, this->config);
        sub_parser.parse();
        auto& source_definitions = sub_parser.getMutableTemplateDefinitions();

        for (auto const& [ns, def_map] : source_definitions) {
            for (auto const& [name, def] : def_map) {
                bool type_match = (typeToken.lexeme == "Style" && def->type == TemplateType::STYLE) ||
                                  (typeToken.lexeme == "Element" && def->type == TemplateType::ELEMENT) ||
                                  (typeToken.lexeme == "Var" && def->type == TemplateType::VAR);
                bool custom_match = (is_custom_import && def->is_custom) ||
                                    (is_template_import && !def->is_custom) ||
                                    (!is_custom_import && !is_template_import);

                if (type_match && custom_match) {
                    if (symbols_to_import.empty()) { // Type import
                        this->template_definitions[ns][name] = def;
                    } else { // Precise import
                        for (const auto& sym_name : symbols_to_import) {
                            if (name == sym_name) {
                                std::string final_name = alias.empty() ? name : alias;
                                this->template_definitions[getCurrentNamespace()][final_name] = def;
                                break;
                            }
                        }
                    }
                }
            }
        }
    }
    consume(TokenType::SEMICOLON, "Expect ';' after import statement.");
}

std::unique_ptr<ScriptNode> CHTLParser::parseScriptBlock() {
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'script' keyword.");
    if (check(TokenType::RIGHT_BRACE)) {
        consume(TokenType::RIGHT_BRACE, "Expect '}' after script block.");
        return std::make_unique<ScriptNode>("");
    }
    Token start_token = peek();
    int brace_level = 1;
    Token end_token = start_token;
    while (brace_level > 0 && !isAtEnd()) {
        if (peek().type == TokenType::LEFT_BRACE) brace_level++;
        if (peek().type == TokenType::RIGHT_BRACE) brace_level--;
        if (brace_level == 0) { break; }
        advance();
        end_token = previous();
    }
    int start_pos = start_token.position;
    int end_pos = end_token.position + end_token.lexeme.length();
    std::string content = source.substr(start_pos, end_pos - start_pos);
    consume(TokenType::RIGHT_BRACE, "Expect '}' after script block.");
    return std::make_unique<ScriptNode>(content);
}

std::unique_ptr<BaseNode> CHTLParser::parseOriginBlock() {
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
            return std::make_unique<LiteralExpr>(0, first_part.lexeme);
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

std::unique_ptr<ConfigNode> CHTLParser::parseConfigurationBlock() {
    consume(TokenType::LEFT_BRACKET, "Expect '[' to start [Configuration] block.");
    consume(TokenType::IDENTIFIER, "Expect 'Configuration' keyword.");
    consume(TokenType::RIGHT_BRACKET, "Expect ']' to end keyword.");
    consume(TokenType::LEFT_BRACE, "Expect '{' to start configuration body.");
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (peek().type == TokenType::LEFT_BRACKET && tokens.size() > current + 1 && tokens[current + 1].lexeme == "Name") {
            consume(TokenType::LEFT_BRACKET, "Expect '['.");
            consume(TokenType::IDENTIFIER, "Expect 'Name'.");
            consume(TokenType::RIGHT_BRACKET, "Expect ']'.");
            consume(TokenType::LEFT_BRACE, "Expect '{'.");
            while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                Token key = consume(TokenType::IDENTIFIER, "Expect keyword configuration name.");
                consume(TokenType::EQUAL, "Expect '=' after keyword name.");
                std::vector<std::string> values;
                if (match({TokenType::LEFT_BRACKET})) {
                    do {
                        values.push_back(consume(TokenType::IDENTIFIER, "Expect value.").lexeme);
                    } while (match({TokenType::COMMA}));
                    consume(TokenType::RIGHT_BRACKET, "Expect ']'.");
                } else {
                    Token value_token = advance();
                    values.push_back(value_token.lexeme);
                }
                config->keyword_map[key.lexeme] = values;
                consume(TokenType::SEMICOLON, "Expect ';' after value.");
            }
            consume(TokenType::RIGHT_BRACE, "Expect '}'.");
        } else {
            Token key = consume(TokenType::IDENTIFIER, "Expect configuration key.");
            consume(TokenType::EQUAL, "Expect '=' after configuration key.");
            Token value = advance();
            consume(TokenType::SEMICOLON, "Expect ';' after configuration value.");
            if (key.lexeme == "INDEX_INITIAL_COUNT") {
                if (value.type == TokenType::NUMBER) config->index_initial_count = std::stoi(value.lexeme);
            } else if (key.lexeme == "DEBUG_MODE") {
                if (value.lexeme == "true") config->debug_mode = true;
                else if (value.lexeme == "false") config->debug_mode = false;
            } else if (key.lexeme == "DISABLE_NAME_GROUP") {
                if (value.lexeme == "true") config->disable_name_group = true;
                else if (value.lexeme == "false") config->disable_name_group = false;
            } else if (key.lexeme == "DISABLE_STYLE_AUTO_ADD_CLASS") {
                if (value.lexeme == "true") config->disable_style_auto_add_class = true;
                else if (value.lexeme == "false") config->disable_style_auto_add_class = false;
            } else if (key.lexeme == "DISABLE_STYLE_AUTO_ADD_ID") {
                if (value.lexeme == "true") config->disable_style_auto_add_id = true;
                else if (value.lexeme == "false") config->disable_style_auto_add_id = false;
            } else if (key.lexeme == "DISABLE_DEFAULT_NAMESPACE") {
                if (value.lexeme == "true") config->disable_default_namespace = true;
                else if (value.lexeme == "false") config->disable_default_namespace = false;
            } else if (key.lexeme == "DISABLE_CUSTOM_ORIGIN_TYPE") {
                if (value.lexeme == "true") config->disable_custom_origin_type = true;
                else if (value.lexeme == "false") config->disable_custom_origin_type = false;
            }
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' to end configuration body.");
    return std::make_unique<ConfigNode>();
}

}
