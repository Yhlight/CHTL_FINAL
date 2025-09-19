#include "CHTLParser.h"
#include "../CHTLLoader/CHTLLoader.h"
#include "../CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/TemplateUsageNode.h"
#include "CHTL/CHTLNode/OriginNode.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <stdexcept>
#include <algorithm>

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
	auto root = std::make_unique<ElementNode>("root");
	std::vector<std::unique_ptr<BaseNode>> top_level_nodes;

	while (!isAtEnd() && peek().type != TokenType::END_OF_FILE) {
		if (check(TokenType::LEFT_BRACKET)) {
			if (tokens.size() > current + 1 && tokens[current + 1].type == TokenType::KEYWORD_TEMPLATE) {
				parseSymbolDeclaration(false);
			}
			else if (tokens.size() > current + 1 && tokens[current + 1].type == TokenType::KEYWORD_CUSTOM) {
				parseSymbolDeclaration(true);
			}
			else if (tokens.size() > current + 1 && tokens[current + 1].type == TokenType::KEYWORD_ORIGIN) {
				top_level_nodes.push_back(parseOriginBlock());
			}
			else if (tokens.size() > current + 1 && tokens[current + 1].type == TokenType::KEYWORD_IMPORT) {
				top_level_nodes.push_back(parseImportStatement());
			}
			else if (tokens.size() > current + 1 && tokens[current + 1].type == TokenType::KEYWORD_NAMESPACE) {
				top_level_nodes.push_back(parseNamespace());
			}
			else if (tokens.size() > current + 1 && tokens[current + 1].type == TokenType::KEYWORD_CONFIGURATION) {
				top_level_nodes.push_back(parseConfiguration());
			}
			else {
				break;
			}
		}
		else if (check(TokenType::IDENTIFIER) || check(TokenType::AT) || check(TokenType::TEXT) || check(TokenType::STYLE)) {
			for (auto& node : parseDeclaration()) {
				top_level_nodes.push_back(std::move(node));
			}
		}
		else {
			break;
		}
	}

	if (top_level_nodes.empty()) {
		return nullptr;
	}

	if (top_level_nodes.size() == 1 && dynamic_cast<ElementNode*>(top_level_nodes[0].get())) {
		return std::move(top_level_nodes[0]);
	}

	for (auto& node : top_level_nodes) {
		root->addChild(std::move(node));
	}

	return root;
}

std::vector<std::unique_ptr<BaseNode>> CHTLParser::parseDeclaration() {
    if (check(TokenType::AT)) {
        if (is_parsing_template_definition) {
            consume(TokenType::AT, "Expect '@'.");
            Token type = consume(TokenType::IDENTIFIER, "Expect 'Element' or 'Style'.");
            if (type.lexeme != "Element") error(type, "Only @Element templates can be nested in this context.");
            Token name = consume(TokenType::IDENTIFIER, "Expect template name.");
            consume(TokenType::SEMICOLON, "Expect ';'.");
            std::vector<std::unique_ptr<BaseNode>> nodes;
            nodes.push_back(std::make_unique<TemplateUsageNode>(TemplateType::ELEMENT, name.lexeme));
            return nodes;
        } else {
            return parseElementTemplateUsage();
        }
    }
    if (peek().type == TokenType::LEFT_BRACKET && tokens.size() > current + 1 && tokens[current + 1].type == TokenType::KEYWORD_ORIGIN) {
        // This could be a definition or a usage.
        // Let's peek ahead to see if there's a '{'
        bool is_definition = false;
        for (int i = 0; !isAtEnd() && tokens[current + i].type != TokenType::SEMICOLON; ++i) {
            if (tokens[current + i].type == TokenType::LEFT_BRACE) {
                is_definition = true;
                break;
            }
        }

        if (is_definition) {
            auto node = parseOriginBlock();
            if (node) { // parseOriginBlock returns nullptr for named (defined) blocks
                std::vector<std::unique_ptr<BaseNode>> nodes;
                nodes.push_back(std::move(node));
                return nodes;
            }
            return {}; // It was a named block, not part of the tree here.
        } else {
            // This is a usage of a named origin block
            consume(TokenType::LEFT_BRACKET, "Expect '['.");
            consume(TokenType::KEYWORD_ORIGIN, "Expect 'Origin'.");
            consume(TokenType::RIGHT_BRACKET, "Expect ']'.");
            consume(TokenType::AT, "Expect '@'.");
            consume(TokenType::IDENTIFIER, "Expect origin type."); // Type is still needed for lookup
            Token name = consume(TokenType::IDENTIFIER, "Expect origin block name.");
            consume(TokenType::SEMICOLON, "Expect ';'.");

            const OriginNode* def = nullptr;
            for (const auto& ns_pair : origin_definitions) {
                if (ns_pair.second.count(name.lexeme)) {
                    def = ns_pair.second.at(name.lexeme).get();
                    break;
                }
            }
            if (!def) {
                error(name, "Named origin block '" + name.lexeme + "' not found.");
            }
            std::vector<std::unique_ptr<BaseNode>> nodes;
            nodes.push_back(def->clone());
            return nodes;
        }
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

    if (match({TokenType::FROM})) {
        std::string target_namespace;
        while(!check(TokenType::SEMICOLON) && !check(TokenType::LEFT_BRACE)) {
            target_namespace += advance().lexeme;
        }
        // The generator will need to handle the namespaced lookup.
        // For now, we prepend the namespace to the template name.
        name.lexeme = target_namespace + "." + name.lexeme;
    }

    StyleNode::StyleApplication app;
    app.template_name = name.lexeme;
    if (match({TokenType::LEFT_BRACE})) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            if (match({TokenType::DELETE})) {
                if (check(TokenType::AT)) {
                    consume(TokenType::AT, "Expect '@'.");
                    Token type = consume(TokenType::IDENTIFIER, "Expect 'Style' keyword.");
                    if (type.lexeme != "Style") error(type, "Can only delete @Style templates.");
                    app.deleted_templates.push_back(consume(TokenType::IDENTIFIER, "Expect template name.").lexeme);
                } else {
                    do {
                        Token prop_to_delete = consume(TokenType::IDENTIFIER, "Expect property name after 'delete'.");
                        app.deleted_properties.push_back(prop_to_delete.lexeme);
                    } while (match({TokenType::COMMA}));
                }
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
        while(!check(TokenType::SEMICOLON) && !check(TokenType::LEFT_BRACE)) {
            target_namespace += advance().lexeme;
        }
    }

    const TemplateDefinitionNode* def = nullptr;
    if (!target_namespace.empty()) {
        if (template_definitions.count(target_namespace) && template_definitions.at(target_namespace).count(name.lexeme)) {
            def = &template_definitions.at(target_namespace).at(name.lexeme);
        }
    } else {
        for (const auto& ns_pair : template_definitions) {
            if (ns_pair.second.count(name.lexeme)) {
                def = &ns_pair.second.at(name.lexeme);
                break;
            }
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
                if (check(TokenType::AT)) {
                    consume(TokenType::AT, "Expect '@'.");
                    Token type = consume(TokenType::IDENTIFIER, "Expect 'Element' keyword.");
                    if (type.lexeme != "Element") error(type, "Can only delete @Element templates.");
                    Token name = consume(TokenType::IDENTIFIER, "Expect template name.");
                    cloned_nodes.erase(std::remove_if(cloned_nodes.begin(), cloned_nodes.end(),
                        [&](const std::unique_ptr<BaseNode>& node) {
                            if (auto* tu = dynamic_cast<TemplateUsageNode*>(node.get())) {
                                return tu->name == name.lexeme;
                            }
                            return false;
                        }), cloned_nodes.end());
                } else {
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
                }
                consume(TokenType::SEMICOLON, "Expect ';'.");
            } else if (match({TokenType::INSERT})) {
                Token position;
                Token selector;
                bool is_global_insert = false;

                if (check(TokenType::AT_TOP) || check(TokenType::AT_BOTTOM)) {
                    position = advance();
                    is_global_insert = true;
                } else {
                    if (!match({TokenType::AFTER, TokenType::BEFORE, TokenType::REPLACE})) {
                        error(peek(), "Expect 'after', 'before', or 'replace'.");
                    }
                    position = previous();
                    selector = consume(TokenType::IDENTIFIER, "Expect tag name.");
                }
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

                if (is_global_insert) {
                    if (position.type == TokenType::AT_TOP) {
                        cloned_nodes.insert(cloned_nodes.begin(), std::make_move_iterator(nodes_to_insert.begin()), std::make_move_iterator(nodes_to_insert.end()));
                    } else { // AT_BOTTOM
                        cloned_nodes.insert(cloned_nodes.end(), std::make_move_iterator(nodes_to_insert.begin()), std::make_move_iterator(nodes_to_insert.end()));
                    }
                } else {
                    if (!findAndInsert(cloned_nodes, selector.lexeme, index, position.type, nodes_to_insert)) {
                        error(selector, "Could not find target for insert.");
                    }
                }
            } else if (check(TokenType::IDENTIFIER)) {
                Token selector = consume(TokenType::IDENTIFIER, "Expect tag name.");
                // For now, find the first element with this tag name in the cloned nodes.
                ElementNode* target_node = nullptr;
                for (const auto& node : cloned_nodes) {
                    if (auto* en = dynamic_cast<ElementNode*>(node.get())) {
                        if (en->tagName == selector.lexeme) {
                            target_node = en;
                            break;
                        }
                    }
                }

                if (target_node) {
                    consume(TokenType::LEFT_BRACE, "Expect '{' for specialization.");
                    if (match({TokenType::STYLE})) {
                        auto new_style_node = parseStyleBlock();
                        // Find existing style node and merge, or add new one.
                        StyleNode* existing_style = nullptr;
                        for (const auto& child : target_node->children) {
                            if(auto* sn = dynamic_cast<StyleNode*>(child.get())) {
                                existing_style = sn;
                                break;
                            }
                        }
                        if (existing_style) {
                            // Merge properties and rules
                            existing_style->direct_properties.insert(existing_style->direct_properties.end(), std::make_move_iterator(new_style_node->direct_properties.begin()), std::make_move_iterator(new_style_node->direct_properties.end()));
                            existing_style->template_applications.insert(existing_style->template_applications.end(), std::make_move_iterator(new_style_node->template_applications.begin()), std::make_move_iterator(new_style_node->template_applications.end()));
                        } else {
                            target_node->addChild(std::move(new_style_node));
                        }
                    }
                    consume(TokenType::RIGHT_BRACE, "Expect '}' after specialization.");
                } else {
                     error(selector, "Could not find element to specialize.");
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
    if (is_custom) {
        consume(TokenType::KEYWORD_CUSTOM, "Expect 'Custom' keyword.");
    } else {
        consume(TokenType::KEYWORD_TEMPLATE, "Expect 'Template' keyword.");
    }
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

    this->is_parsing_template_definition = true;
    consume(TokenType::LEFT_BRACE, "Expect '{' to start symbol body.");
    if (def.type == TemplateType::STYLE) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            if (match({TokenType::INHERIT})) {
                consume(TokenType::AT, "Expect '@' after 'inherit'.");
                Token type = consume(TokenType::IDENTIFIER, "Expect 'Style' keyword.");
                if (type.lexeme != "Style") error(type, "Can only inherit from @Style templates.");
                def.inherited_templates.push_back(consume(TokenType::IDENTIFIER, "Expect template name.").lexeme);
                consume(TokenType::SEMICOLON, "Expect ';' after inheritance.");
            }
            else if (check(TokenType::AT)) {
                consume(TokenType::AT, "Expect '@'.");
                Token type = consume(TokenType::IDENTIFIER, "Expect 'Style' keyword.");
                if (type.lexeme != "Style") error(type, "Can only inherit from @Style templates.");
                def.inherited_templates.push_back(consume(TokenType::IDENTIFIER, "Expect template name.").lexeme);
                consume(TokenType::SEMICOLON, "Expect ';' after inheritance.");
            } else {
                std::string key_str;
                while (!check(TokenType::COLON) && !check(TokenType::SEMICOLON) && !isAtEnd()) {
                    key_str += advance().lexeme;
                }

                if (is_custom && match({TokenType::SEMICOLON})) {
                    // Valueless property for [Custom] @Style
                    def.style_properties.push_back({key_str, nullptr});
                } else {
                    consume(TokenType::COLON, "Expect ':'.");
                    auto value_expr = parseExpression();
                    consume(TokenType::SEMICOLON, "Expect ';'.");
                    def.style_properties.push_back({key_str, std::move(value_expr)});
                }
            }
        }
    } else if (def.type == TemplateType::ELEMENT) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            for (auto& node : parseDeclaration()) {
                def.element_body.push_back(std::move(node));
            }
        }
    }
    else if (def.type == TemplateType::VAR) {
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            Token key = consume(TokenType::IDENTIFIER, "Expect variable name.");
            consume(TokenType::COLON, "Expect ':'.");
            auto value_expr = parseExpression();
            consume(TokenType::SEMICOLON, "Expect ';'.");
            def.variables[key.lexeme] = std::move(value_expr);
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' to end symbol body.");
    this->is_parsing_template_definition = false;
    template_definitions[current_namespace][def.name] = std::move(def);
}

std::unique_ptr<NamespaceNode> CHTLParser::parseNamespace() {
	consume(TokenType::LEFT_BRACKET, "Expect '['.");
	consume(TokenType::KEYWORD_NAMESPACE, "Expect 'Namespace'.");
	consume(TokenType::RIGHT_BRACKET, "Expect ']'.");
	Token name = consume(TokenType::IDENTIFIER, "Expect namespace name.");
	auto node = std::make_unique<NamespaceNode>();
	node->name = name.lexeme;

	std::string previous_namespace = this->current_namespace;
	if (!previous_namespace.empty() && previous_namespace != getFilename(this->file_path)) {
		this->current_namespace = previous_namespace + "." + name.lexeme;
	} else {
		this->current_namespace = name.lexeme;
	}
	node->name = this->current_namespace;


	if (match({ TokenType::LEFT_BRACE })) {
		while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
			if (check(TokenType::LEFT_BRACKET)) {
                if (tokens.size() > current + 1 && tokens[current + 1].type == TokenType::KEYWORD_TEMPLATE) {
                    parseSymbolDeclaration(false);
                } else if (tokens.size() > current + 1 && tokens[current + 1].type == TokenType::KEYWORD_CUSTOM) {
                    parseSymbolDeclaration(true);
                } else if (tokens.size() > current + 1 && tokens[current + 1].type == TokenType::KEYWORD_NAMESPACE) {
                    node->children.push_back(parseNamespace());
                } else {
                    error(peek(), "Invalid declaration inside namespace.");
                }
            } else {
                error(peek(), "Only declarations are allowed inside a namespace block.");
            }
		}
		consume(TokenType::RIGHT_BRACE, "Expect '}'.");
	}

	this->current_namespace = previous_namespace;

	return node;
}

std::unique_ptr<ConfigurationNode> CHTLParser::parseConfiguration() {
	consume(TokenType::LEFT_BRACKET, "Expect '['.");
	consume(TokenType::KEYWORD_CONFIGURATION, "Expect 'Configuration'.");
	consume(TokenType::RIGHT_BRACKET, "Expect ']'.");
	auto node = std::make_unique<ConfigurationNode>();
	if (check(TokenType::AT)) {
		advance();
		node->name = consume(TokenType::IDENTIFIER, "Expect configuration name.").lexeme;
	}

	consume(TokenType::LEFT_BRACE, "Expect '{' for configuration body.");
	while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
		if (check(TokenType::LEFT_BRACKET) && tokens.size() > current + 1 && tokens[current + 1].type == TokenType::KEYWORD_NAME) {
			consume(TokenType::LEFT_BRACKET, "Expect '['.");
			consume(TokenType::KEYWORD_NAME, "Expect 'Name'.");
			consume(TokenType::RIGHT_BRACKET, "Expect ']'.");
			consume(TokenType::LEFT_BRACE, "Expect '{' for Name group.");
			while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
				Token key = consume(TokenType::IDENTIFIER, "Expect keyword variable.");
				consume(TokenType::EQUAL, "Expect '='.");
				if (check(TokenType::LEFT_BRACKET)) {
					consume(TokenType::LEFT_BRACKET, "Expect '[' for group options.");
					std::vector<std::string> values;
					do {
						values.push_back(consume(TokenType::IDENTIFIER, "Expect option.").lexeme);
					} while (match({ TokenType::COMMA }));
					consume(TokenType::RIGHT_BRACKET, "Expect ']'.");
					node->name_group[key.lexeme] = values;

				}
				else {
					Token value = consume(TokenType::IDENTIFIER, "Expect keyword value.");
					node->settings[key.lexeme] = value.lexeme;
				}
				consume(TokenType::SEMICOLON, "Expect ';'.");
			}
			consume(TokenType::RIGHT_BRACE, "Expect '}' for Name group.");
		}
		else {
			Token key = consume(TokenType::IDENTIFIER, "Expect setting name.");
			consume(TokenType::EQUAL, "Expect '='.");
			Token value = consume(TokenType::STRING, "Expect setting value.");
			node->settings[key.lexeme] = value.lexeme;
			consume(TokenType::SEMICOLON, "Expect ';'.");
		}
	}
	consume(TokenType::RIGHT_BRACE, "Expect '}' for configuration body.");

	return node;
}

std::unique_ptr<ConstraintNode> CHTLParser::parseConstraint() {
	consume(TokenType::EXCEPT, "Expect 'except' keyword.");
	auto node = std::make_unique<ConstraintNode>();

	do {
		Constraint constraint;
		if (check(TokenType::AT) || (check(TokenType::LEFT_BRACKET))) {
			constraint.type = ConstraintType::TYPE;
			//...
		}
		else {
			constraint.type = ConstraintType::PRECISE;
			//...
		}
		node->constraints.push_back(constraint);
	} while (match({ TokenType::COMMA }));

	consume(TokenType::SEMICOLON, "Expect ';' after except statement.");
	return node;
}

std::unique_ptr<ImportNode> CHTLParser::parseImportStatement() {
	auto node = std::make_unique<ImportNode>();
	consume(TokenType::LEFT_BRACKET, "Expect '['.");
	consume(TokenType::KEYWORD_IMPORT, "Expect 'Import'.");
	consume(TokenType::RIGHT_BRACKET, "Expect ']'.");

	if (match({ TokenType::LEFT_BRACKET })) {
		if (match({ TokenType::KEYWORD_CUSTOM })) {
			node->is_custom = true;
		}
		else if (match({ TokenType::KEYWORD_TEMPLATE })) {
			node->is_custom = false;
		}
		else {
			error(peek(), "Expect 'Custom' or 'Template' keyword after '[' in import.");
		}
		consume(TokenType::RIGHT_BRACKET, "Expect ']'.");
	}

	consume(TokenType::AT, "Expect '@'.");
	Token typeToken = consume(TokenType::IDENTIFIER, "Expect import type.");

	if (typeToken.lexeme == "Chtl") node->import_type = ImportType::CHTL;
	else if (typeToken.lexeme == "Html") node->import_type = ImportType::HTML;
	else if (typeToken.lexeme == "Style") node->import_type = ImportType::STYLE;
	else if (typeToken.lexeme == "JavaScript") node->import_type = ImportType::JAVASCRIPT;
	else if (typeToken.lexeme == "CJmod") node->import_type = ImportType::CJMOD;
	else if (typeToken.lexeme == "Config") node->import_type = ImportType::CONFIG;
	else error(typeToken, "Unknown import type.");

	if (node->import_type == ImportType::CHTL && check(TokenType::IDENTIFIER)) {
		node->item_name = advance().lexeme;
	}

	consume(TokenType::FROM, "Expect 'from'.");
	if (check(TokenType::STRING)) {
		node->path = advance().lexeme;
	} else if (check(TokenType::IDENTIFIER)) {
		std::string path_str;
		do {
			path_str += advance().lexeme;
		} while (match({TokenType::DOT, TokenType::SLASH}));
		node->path = path_str;
	} else {
		error(peek(), "Expect file path as string or unquoted literal.");
	}

	if (match({ TokenType::AS })) {
		node->alias = consume(TokenType::IDENTIFIER, "Expect alias name.").lexeme;
	}

	consume(TokenType::SEMICOLON, "Expect ';'.");


    if (node->import_type == ImportType::CHTL) {
        if (node->item_name.empty()) { // Is it a full file import?
            std::string imported_content = CHTLLoader::load(this->file_path, node->path);
            std::string imported_path = FileSystem::getDirectory(this->file_path) + node->path;
            CHTLLexer sub_lexer(imported_content);
            std::vector<Token> sub_tokens = sub_lexer.scanTokens();
            CHTLParser sub_parser(imported_content, sub_tokens, imported_path);
            sub_parser.parse();
            auto& imported_template_map = sub_parser.getMutableTemplateDefinitions();
            std::string import_namespace = getFilename(node->path);
            auto it = imported_template_map.find(import_namespace);
            if (it != imported_template_map.end()) {
                this->template_definitions[import_namespace] = std::move(it->second);
            }
        } else {
            // TODO: Handle precise/type/wildcard CHTL imports
        }
    } else { // HTML, Style, JS
        if (node->alias.empty()) {
            error(previous(), "Import of non-CHTL files must use an 'as' alias.");
        }
        std::string imported_content = CHTLLoader::load(this->file_path, node->path);
        OriginType originType = OriginType::HTML;
        if (node->import_type == ImportType::STYLE) originType = OriginType::STYLE;
        else if (node->import_type == ImportType::JAVASCRIPT) originType = OriginType::JAVASCRIPT;

        origin_definitions[current_namespace][node->alias] = std::make_unique<OriginNode>(imported_content, originType, node->alias);
    }

	return node;
}

std::unique_ptr<BaseNode> CHTLParser::parseOriginBlock() {
    consume(TokenType::LEFT_BRACKET, "Expect '['.");
    consume(TokenType::KEYWORD_ORIGIN, "Expect 'Origin'.");
    consume(TokenType::RIGHT_BRACKET, "Expect ']'.");
    consume(TokenType::AT, "Expect '@'.");
    Token type = consume(TokenType::IDENTIFIER, "Expect origin type.");

    Token name;
    bool is_named = false;
    if (check(TokenType::IDENTIFIER)) {
        name = advance();
        is_named = true;
    }

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

    if (is_named) {
        origin_definitions[current_namespace][name.lexeme] = std::make_unique<OriginNode>(content, originType, name.lexeme);
        return nullptr; // Not part of the direct AST
    } else {
        return std::make_unique<OriginNode>(content, originType);
    }
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
            Token key = consume(TokenType::IDENTIFIER, "Expect variable name.");
            std::unique_ptr<Expr> specialization = nullptr;
            if (match({TokenType::EQUAL})) {
                specialization = parseExpression();
            }
            consume(TokenType::RIGHT_PAREN, "Expect ')'.");
            return std::make_unique<VarExpr>(first_part.lexeme, key.lexeme, std::move(specialization));
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

} // namespace CHTL
