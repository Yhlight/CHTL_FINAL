#include "CHTLParser.h"
#include "../CHTLLexer/CHTLLexer.h" // For recursive parsing
#include <iostream>
#include <stdexcept>
#include <unordered_set>
#include <set>
#include <algorithm>
#include "../CHTLNode/PropertyReferenceNode.h"

namespace CHTL {

CHTLParser::CHTLParser(const std::string& source, std::vector<Token>& tokens, CHTLLoader& loader, const std::string& initial_path, std::shared_ptr<ParserContext> context)
    : source_(source), tokens_(tokens), loader_(loader), current_path_(initial_path), context_(context) {
    if (!tokens_.empty() && tokens_[0].type == TokenType::Namespace) {
        if (tokens_.size() > 1 && tokens_[1].type == TokenType::Identifier) {
            current_namespace_ = tokens_[1].lexeme;
        }
    }
}

std::unique_ptr<RootNode> CHTLParser::parse() {
    auto root = std::make_unique<RootNode>();
    while (!isAtEnd()) {
        try {
            auto nodes = parseDeclaration();
            for (auto& node : nodes) {
                root->children_.push_back(std::move(node));
            }
        } catch (const std::runtime_error& e) {
            std::cerr << "Parse Error: " << e.what() << std::endl;
            synchronize();
            hadError_ = true;
        }
    }
    return root;
}

std::vector<std::unique_ptr<Node>> CHTLParser::parseDeclaration() {
    std::vector<std::unique_ptr<Node>> nodes;
    if (match({TokenType::Namespace})) {
        current_namespace_ = consume(TokenType::Identifier, "Expected namespace name.").lexeme;
        if (match({TokenType::OpenBrace})) {
            while (!check(TokenType::CloseBrace) && !isAtEnd()) {
                auto parsed_nodes = parseDeclaration();
                nodes.insert(nodes.end(), std::make_move_iterator(parsed_nodes.begin()), std::make_move_iterator(parsed_nodes.end()));
            }
            consume(TokenType::CloseBrace, "Expected '}' to close namespace block.");
            current_namespace_ = "";
        }
        return nodes;
    }
    if (match({TokenType::Configuration})) {
        parseConfigurationBlock();
        return nodes;
    }
    if (match({TokenType::Import})) {
        consume(TokenType::AtChtl, "Expected '@Chtl' for file import.");
        consume(TokenType::From, "Expected 'from' keyword in import statement.");
        const Token& pathToken = consume(TokenType::StringLiteral, "Expected file path string.");
        consume(TokenType::Semicolon, "Expected ';' after import statement.");

        std::string import_path = pathToken.lexeme;
        if(auto content = loader_.loadFile(import_path, current_path_)) {
            std::filesystem::path p(import_path);
            std::string default_namespace = p.stem().string();

            std::string imported_file_canonical_path;
            try {
                imported_file_canonical_path = std::filesystem::canonical(std::filesystem::path(current_path_).parent_path() / import_path).string();
            } catch (const std::filesystem::filesystem_error& e) {
                 throw std::runtime_error("Could not find imported file: " + import_path);
            }

            CHTLLexer imported_lexer(*content);
            std::vector<Token> imported_tokens = imported_lexer.scanTokens();
            CHTLParser imported_parser(*content, imported_tokens, loader_, imported_file_canonical_path, context_);

            if (imported_parser.current_namespace_.empty()) {
                imported_parser.current_namespace_ = default_namespace;
            }

            if (!imported_parser.current_namespace_.empty()) {
                context_->imported_namespaces_.insert(imported_parser.current_namespace_);
            }

            imported_parser.parse();
        }

        return nodes;
    }
    if (match({TokenType::Template, TokenType::Custom})) {
        bool is_custom = previous().type == TokenType::Custom;
        parseTemplateDefinition(is_custom);
        return nodes;
    }
    if (match({TokenType::AtElement})) {
        const Token& name = consume(TokenType::Identifier, "Expected template name after '@Element'.");
        std::string qualified_name;
        if (match({TokenType::From})) {
            const Token& ns = consume(TokenType::Identifier, "Expected namespace name after 'from'.");
            qualified_name = ns.lexeme + "::" + name.lexeme;
        } else {
            qualified_name = resolveUnqualifiedName(name.lexeme, TemplateType::Element);
        }

        if (!context_->element_templates_.count(qualified_name)) {
            // This should ideally not be reached if resolveUnqualifiedName works correctly, but it's good practice.
            throw std::runtime_error("Use of undefined element template '" + qualified_name + "'.");
        }

        const auto& templateNode = context_->element_templates_.at(qualified_name);
        for (const auto& child : templateNode->children_) {
            nodes.push_back(child->clone());
        }

        if (match({TokenType::OpenBrace})) {
            applySpecializations(nodes);
            consume(TokenType::CloseBrace, "Expected '}' to close specialization block.");
        } else {
            consume(TokenType::Semicolon, "Expected ';' after element template usage.");
        }

        return nodes;
    }

    std::unique_ptr<Node> singleNode = nullptr;
    if (match({TokenType::Style})) {
        singleNode = parseStyleBlock();
    } else if (match({TokenType::Identifier})) {
        if (peek().type == TokenType::OpenBrace) {
            singleNode = parseElement();
        } else if (check(TokenType::Semicolon)) {
            const Token& name = previous();
            consume(TokenType::Semicolon, "Expected ';' after empty element.");
            singleNode = std::make_unique<ElementNode>(name.lexeme);
        }
    } else if (match({TokenType::Text})) {
        singleNode = parseText();
    } else if (match({TokenType::GeneratorComment})) {
        singleNode = parseGeneratorComment();
    } else if (match({TokenType::Origin})) {
        singleNode = parseOriginBlock();
    }

    if(singleNode) {
        nodes.push_back(std::move(singleNode));
        return nodes;
    }

    advance();
    throw std::runtime_error("Expected a declaration (element, text, etc.).");
}

std::unique_ptr<OriginNode> CHTLParser::parseOriginBlock() {
    const Token& type = consume(TokenType::Identifier, "Expected origin type (e.g., @Html).");
    consume(TokenType::OpenBrace, "Expected '{' to open origin block.");

    std::stringstream content_ss;
    int brace_level = 1;
    while (brace_level > 0 && !isAtEnd()) {
        if (peek().type == TokenType::OpenBrace) brace_level++;
        else if (peek().type == TokenType::CloseBrace) brace_level--;

        if (brace_level == 0) break;

        content_ss << advance().lexeme << " ";
    }

    if (brace_level > 0) {
        throw std::runtime_error("Unterminated origin block.");
    }

    return std::make_unique<OriginNode>(type.lexeme, content_ss.str());
}

void CHTLParser::applySpecializations(std::vector<std::unique_ptr<Node>>& target_nodes) {
    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        if (match({TokenType::Delete})) {
            const Token& tagName = consume(TokenType::Identifier, "Expected tag name for deletion.");
            int tag_index = -1;
            if (match({TokenType::OpenBracket})) {
                tag_index = std::stoi(consume(TokenType::Number, "Expected index in brackets.").lexeme);
                consume(TokenType::CloseBracket, "Expected ']' after index.");
            }
            consume(TokenType::Semicolon, "Expected ';' after delete statement.");

            for (auto& top_level_node : target_nodes) {
                if (top_level_node->getType() == NodeType::Element) {
                    auto* element = static_cast<ElementNode*>(top_level_node.get());
                    int current_tag_count = 0;
                    element->children_.erase(
                        std::remove_if(element->children_.begin(), element->children_.end(),
                            [&](const std::unique_ptr<Node>& child) {
                                if (child->getType() == NodeType::Element) {
                                    auto* child_element = static_cast<ElementNode*>(child.get());
                                    if (child_element->tagName_ == tagName.lexeme) {
                                        bool match = (tag_index == -1 || current_tag_count == tag_index);
                                        current_tag_count++;
                                        return match;
                                    }
                                }
                                return false;
                            }),
                        element->children_.end()
                    );
                }
            }
            continue;
        }

        if (match({TokenType::Insert})) {
            Token position_token = advance();
            if (position_token.type != TokenType::After && position_token.type != TokenType::Before && position_token.type != TokenType::Replace && position_token.type != TokenType::AtTop && position_token.type != TokenType::AtBottom) {
                throw std::runtime_error("Expected a valid position for insert (after, before, replace, at top, at bottom).");
            }

            std::vector<std::unique_ptr<Node>> new_nodes;
            auto* element = static_cast<ElementNode*>(target_nodes[0].get());

            if (position_token.type == TokenType::AtTop) {
                 consume(TokenType::OpenBrace, "Expected '{' for insert block.");
                 while (!check(TokenType::CloseBrace) && !isAtEnd()) {
                    auto parsed = parseDeclaration();
                    new_nodes.insert(new_nodes.end(), std::make_move_iterator(parsed.begin()), std::make_move_iterator(parsed.end()));
                 }
                 consume(TokenType::CloseBrace, "Expected '}' after insert block.");
                 element->children_.insert(element->children_.begin(), std::make_move_iterator(new_nodes.begin()), std::make_move_iterator(new_nodes.end()));
            } else if (position_token.type == TokenType::AtBottom) {
                consume(TokenType::OpenBrace, "Expected '{' for insert block.");
                 while (!check(TokenType::CloseBrace) && !isAtEnd()) {
                    auto parsed = parseDeclaration();
                    new_nodes.insert(new_nodes.end(), std::make_move_iterator(parsed.begin()), std::make_move_iterator(parsed.end()));
                 }
                 consume(TokenType::CloseBrace, "Expected '}' after insert block.");
                 element->children_.insert(element->children_.end(), std::make_move_iterator(new_nodes.begin()), std::make_move_iterator(new_nodes.end()));
            }
            else {
                 const Token& tagName = consume(TokenType::Identifier, "Expected tag name for insertion target.");
                int tag_index = -1;
                if (match({TokenType::OpenBracket})) {
                    tag_index = std::stoi(consume(TokenType::Number, "Expected index in brackets.").lexeme);
                    consume(TokenType::CloseBracket, "Expected ']' after index.");
                }
                consume(TokenType::OpenBrace, "Expected '{' for insert block.");
                while (!check(TokenType::CloseBrace) && !isAtEnd()) {
                    auto parsed = parseDeclaration();
                    new_nodes.insert(new_nodes.end(), std::make_move_iterator(parsed.begin()), std::make_move_iterator(parsed.end()));
                }
                consume(TokenType::CloseBrace, "Expected '}' after insert block.");

                int current_tag_count = 0;
                auto it = element->children_.begin();
                bool found = false;
                while(it != element->children_.end()) {
                    if ((*it)->getType() == NodeType::Element) {
                        auto* child_element = static_cast<ElementNode*>((*it).get());
                        if (child_element->tagName_ == tagName.lexeme) {
                            if (tag_index == -1 || current_tag_count == tag_index) {
                                if (position_token.type == TokenType::Before) {
                                    element->children_.insert(it, std::make_move_iterator(new_nodes.begin()), std::make_move_iterator(new_nodes.end()));
                                } else if (position_token.type == TokenType::After) {
                                    it = element->children_.insert(it + 1, std::make_move_iterator(new_nodes.begin()), std::make_move_iterator(new_nodes.end()));
                                } else if (position_token.type == TokenType::Replace) {
                                    it = element->children_.erase(it);
                                    element->children_.insert(it, std::make_move_iterator(new_nodes.begin()), std::make_move_iterator(new_nodes.end()));
                                }
                                found = true;
                                break;
                            }
                            current_tag_count++;
                        }
                    }
                    ++it;
                }
                 if (!found) throw std::runtime_error("Could not find target for insert operation.");
            }
            continue;
        }

        const Token& tagName = consume(TokenType::Identifier, "Expected tag name for specialization.");
        int tag_index = -1;
        if (match({TokenType::OpenBracket})) {
            tag_index = std::stoi(consume(TokenType::Number, "Expected index in brackets.").lexeme);
            consume(TokenType::CloseBracket, "Expected ']' after index.");
        }

        consume(TokenType::OpenBrace, "Expected '{' for specialization body.");

        std::vector<ElementNode*> matched_nodes;
        int current_tag_count = 0;
        for (auto& top_level_node : target_nodes) {
            if (top_level_node->getType() == NodeType::Element) {
                auto* top_level_element = static_cast<ElementNode*>(top_level_node.get());
                for (auto& child_node : top_level_element->children_) {
                     if (child_node->getType() == NodeType::Element) {
                        auto* child_element = static_cast<ElementNode*>(child_node.get());
                        if (child_element->tagName_ == tagName.lexeme) {
                            if (tag_index == -1 || current_tag_count == tag_index) {
                                matched_nodes.push_back(child_element);
                            }
                            current_tag_count++;
                        }
                    }
                }
            }
        }

        if (matched_nodes.empty()) {
            throw std::runtime_error("No element found for selector '" + tagName.lexeme + "' to specialize.");
        }

        auto* target_node = matched_nodes[0];
        while (!check(TokenType::CloseBrace) && !isAtEnd()) {
            if (match({TokenType::Style})) {
                auto newStyleBlock = parseStyleBlock();

                StyleBlockNode* existingStyleBlock = nullptr;
                for (auto& child : target_node->children_) {
                    if (child->getType() == NodeType::StyleBlock) {
                        existingStyleBlock = static_cast<StyleBlockNode*>(child.get());
                        break;
                    }
                }

                if (existingStyleBlock) {
                    existingStyleBlock->inline_properties_.insert(
                        existingStyleBlock->inline_properties_.end(),
                        std::make_move_iterator(newStyleBlock->inline_properties_.begin()),
                        std::make_move_iterator(newStyleBlock->inline_properties_.end())
                    );
                    existingStyleBlock->rules_.insert(
                        existingStyleBlock->rules_.end(),
                        std::make_move_iterator(newStyleBlock->rules_.begin()),
                        std::make_move_iterator(newStyleBlock->rules_.end())
                    );
                } else {
                    target_node->children_.push_back(std::move(newStyleBlock));
                }
            } else if (peek().type == TokenType::Identifier && (peekNext().type == TokenType::Colon || peekNext().type == TokenType::Equals)) {
                const Token& key = consume(TokenType::Identifier, "Expected attribute name.");
                consumeColonOrEquals();
                std::string value;
                if (match({TokenType::StringLiteral, TokenType::UnquotedLiteral, TokenType::Identifier})) {
                    value = previous().lexeme;
                } else {
                    throw std::runtime_error("Expected attribute value.");
                }
                consume(TokenType::Semicolon, "Expected ';' after attribute value.");
                target_node->attributes_.push_back(std::make_unique<AttributeNode>(key.lexeme, value));
            }
            else {
                 auto nodes = parseDeclaration();
                 for (auto& node : nodes) {
                    target_node->children_.push_back(std::move(node));
                 }
            }
        }

        consume(TokenType::CloseBrace, "Expected '}' to close specialization body.");
    }
}

std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    const Token& name = previous();
    auto element = std::make_unique<ElementNode>(name.lexeme);

    consume(TokenType::OpenBrace, "Expected '{' after element name.");
    parseElementBody(*element);
    consume(TokenType::CloseBrace, "Expected '}' after element body.");

    return element;
}

void CHTLParser::parseElementBody(ElementNode& element) {
    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        if (match({TokenType::Text})) {
             consumeColonOrEquals();
             std::string value;
             if (match({TokenType::StringLiteral, TokenType::UnquotedLiteral, TokenType::Identifier})) {
                value = previous().lexeme;
             } else {
                throw std::runtime_error("Expected value for text attribute.");
             }
             consume(TokenType::Semicolon, "Expected ';' after text attribute value.");
             element.children_.push_back(std::make_unique<TextNode>(value));
        }
        else if (peek().type == TokenType::Identifier && (peekNext().type == TokenType::Colon || peekNext().type == TokenType::Equals)) {
            const Token& key = consume(TokenType::Identifier, "Expected attribute name.");
            consumeColonOrEquals();

            std::string value;
            if (match({TokenType::StringLiteral, TokenType::UnquotedLiteral, TokenType::Identifier})) {
                value = previous().lexeme;
            } else {
                throw std::runtime_error("Expected attribute value.");
            }

            consume(TokenType::Semicolon, "Expected ';' after attribute value.");
            element.attributes_.push_back(std::make_unique<AttributeNode>(key.lexeme, value));
        }
        else {
            if (match({TokenType::Except})) {
                do {
                    if (match({TokenType::Template})) {
                        if (match({TokenType::AtVar})) {
                             element.constraints_.emplace_back(TypeConstraint{BannedNodeType::VarTemplate});
                        } else {
                             element.constraints_.emplace_back(TypeConstraint{BannedNodeType::Template});
                        }
                    } else if (match({TokenType::Custom})){
                        element.constraints_.emplace_back(TypeConstraint{BannedNodeType::Custom});
                    } else if (match({TokenType::AtHtml})) {
                        element.constraints_.emplace_back(TypeConstraint{BannedNodeType::Html});
                    }
                    else {
                        element.constraints_.emplace_back(PreciseConstraint{consume(TokenType::Identifier, "Expected constraint identifier.").lexeme});
                    }
                } while(match({TokenType::Comma}));
                consume(TokenType::Semicolon, "Expected ';' after except clause.");
            }

            auto nodes = parseDeclaration();
            for (auto& node : nodes) {
                checkConstraints(element, *node);
                element.children_.push_back(std::move(node));
            }
        }
    }
}

std::unique_ptr<TextNode> CHTLParser::parseText() {
    consume(TokenType::OpenBrace, "Expected '{' after 'text' keyword.");

    std::string text_content;
    if (match({TokenType::StringLiteral})) {
        text_content = previous().lexeme;
    } else {
        std::stringstream ss;
        bool first = true;
        while (!check(TokenType::CloseBrace) && !isAtEnd()) {
            if (!first) {
                ss << " ";
            }
            ss << advance().lexeme;
            first = false;
        }
        text_content = ss.str();
    }

    consume(TokenType::CloseBrace, "Expected '}' after text content.");
    return std::make_unique<TextNode>(text_content);
}

std::unique_ptr<CommentNode> CHTLParser::parseGeneratorComment() {
    return std::make_unique<CommentNode>(previous().lexeme);
}

void CHTLParser::parseConfigurationBlock() {
    consume(TokenType::OpenBrace, "Expected '{' after [Configuration] keyword.");
    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        const Token& key = consume(TokenType::Identifier, "Expected configuration key.");
        consumeColonOrEquals();

        if (key.lexeme == "INDEX_INITIAL_COUNT") {
            const Token& value = consume(TokenType::Number, "Expected number for INDEX_INITIAL_COUNT.");
            context_->config_.INDEX_INITIAL_COUNT = std::stoul(value.lexeme);
        } else if (key.lexeme == "DEBUG_MODE") {
            const Token& value = consume(TokenType::Identifier, "Expected 'true' or 'false' for DEBUG_MODE.");
            if (value.lexeme == "true") context_->config_.DEBUG_MODE = true;
            else if (value.lexeme == "false") context_->config_.DEBUG_MODE = false;
            else throw std::runtime_error("Expected 'true' or 'false' for DEBUG_MODE value.");
        }
        else {
            while(peek().type != TokenType::Semicolon && !isAtEnd()) {
                advance();
            }
        }

        consume(TokenType::Semicolon, "Expected ';' after configuration value.");
    }
    consume(TokenType::CloseBrace, "Expected '}' after configuration block.");
}

void CHTLParser::parseTemplateDefinition(bool is_custom) {
    if (match({TokenType::AtStyle})) {
        const Token& name = consume(TokenType::Identifier, "Expected template name.");
        std::string qualified_name = current_namespace_.empty() ? name.lexeme : current_namespace_ + "::" + name.lexeme;
        auto templateNode = std::make_shared<StyleTemplateNode>(qualified_name, is_custom);

        consume(TokenType::OpenBrace, "Expected '{' after template name.");
        while (!check(TokenType::CloseBrace) && !isAtEnd()) {
            if (match({TokenType::AtStyle})) {
                const Token& inheritedName = consume(TokenType::Identifier, "Expected inherited template name.");
                templateNode->inherits_.push_back(inheritedName.lexeme);

                if (is_custom && match({TokenType::OpenBrace})) {
                    std::unordered_set<std::string> deleted_properties;
                    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
                        if (match({TokenType::Delete})) {
                            do {
                                deleted_properties.insert(consume(TokenType::Identifier, "Expected property name after 'delete'.").lexeme);
                            } while (match({TokenType::Comma}));
                            consume(TokenType::Semicolon, "Expected ';' after delete statement.");
                        } else {
                            throw std::runtime_error("Only 'delete' is allowed in definition-time template specialization.");
                        }
                    }
                    consume(TokenType::CloseBrace, "Expected '}' after specialization block.");
                    templateNode->inheritance_specializations_[inheritedName.lexeme] = deleted_properties;
                } else {
                    consume(TokenType::Semicolon, "Expected ';' or '{' after inherited template usage.");
                }
            } else {
                const Token& key = consume(TokenType::Identifier, "Expected CSS property name in template.");
                if (check(TokenType::Comma) || check(TokenType::Semicolon)) {
                     if (!match({TokenType::Comma, TokenType::Semicolon})) {
                            throw std::runtime_error("Expected ',' or ';' after placeholder property.");
                        }
                         templateNode->properties_.emplace_back(key.lexeme, std::vector<PropertyValue>{});
                } else {
                    consumeColonOrEquals();
                    auto value_parts = parsePropertyValue();
                    consume(TokenType::Semicolon, "Expected ';' after property value.");
                    templateNode->properties_.emplace_back(key.lexeme, std::move(value_parts));
                }
            }
        }
        consume(TokenType::CloseBrace, "Expected '}' after template body.");
        context_->style_templates_[qualified_name] = templateNode;

    } else if (match({TokenType::AtElement})) {
        const Token& name = consume(TokenType::Identifier, "Expected template name.");
        std::string qualified_name = current_namespace_.empty() ? name.lexeme : current_namespace_ + "::" + name.lexeme;
        auto templateNode = std::make_shared<ElementTemplateNode>(qualified_name);

        consume(TokenType::OpenBrace, "Expected '{' after template name.");
        while (!check(TokenType::CloseBrace) && !isAtEnd()) {
            auto nodes = parseDeclaration();
            for (auto& node : nodes) {
                templateNode->children_.push_back(std::move(node));
            }
        }
        consume(TokenType::CloseBrace, "Expected '}' after template body.");
        context_->element_templates_[qualified_name] = templateNode;
    } else if (match({TokenType::AtVar})) {
        const Token& name = consume(TokenType::Identifier, "Expected template name.");
        std::string qualified_name = current_namespace_.empty() ? name.lexeme : current_namespace_ + "::" + name.lexeme;
        auto templateNode = std::make_shared<VarTemplateNode>(qualified_name);

        consume(TokenType::OpenBrace, "Expected '{' after template name.");
        while (!check(TokenType::CloseBrace) && !isAtEnd()) {
            const Token& key = consume(TokenType::Identifier, "Expected variable name in template.");
            consumeColonOrEquals();
            auto value_parts = parsePropertyValue();
            consume(TokenType::Semicolon, "Expected ';' after variable value.");
            templateNode->variables_[key.lexeme] = std::move(value_parts);
        }
        consume(TokenType::CloseBrace, "Expected '}' after template body.");
        context_->var_templates_[qualified_name] = templateNode;
    }
    else {
        throw std::runtime_error("Expected '@Style', '@Element', or '@Var' after '[Template]'.");
    }
}

void CHTLParser::applyStyleTemplate(
    StyleBlockNode& styleNode,
    const std::string& template_name,
    const std::unordered_map<std::string, std::vector<PropertyValue>>& provided_values,
    const std::unordered_set<std::string>& deleted_properties,
    const std::unordered_set<std::string>& deleted_templates,
    std::set<std::string>& visited_templates
) {
    if (visited_templates.count(template_name)) {
        return;
    }
    visited_templates.insert(template_name);

    if (!context_->style_templates_.count(template_name)) {
        throw std::runtime_error("Use of undefined style template '" + template_name + "'.");
    }
    const auto& templateNode = context_->style_templates_.at(template_name);

    for (const auto& inherited_name : templateNode->inherits_) {
        if (deleted_templates.count(inherited_name)) {
            continue;
        }
        applyStyleTemplate(styleNode, inherited_name, provided_values, deleted_properties, deleted_templates, visited_templates);
    }

    for (const auto& prop : templateNode->properties_) {
        const std::string& prop_name = prop.first;
        if (deleted_properties.count(prop_name)) {
            continue;
        }

        const auto& prop_value = prop.second;
        if (prop_value.empty()) {
            if (provided_values.count(prop_name)) {
                styleNode.inline_properties_.emplace_back(prop_name, provided_values.at(prop_name));
            } else {
                 throw std::runtime_error("Value for placeholder '" + prop_name + "' not provided for '" + prop_name + "'.");
            }
        } else {
            styleNode.inline_properties_.push_back(prop);
        }
    }
}


std::unique_ptr<StyleBlockNode> CHTLParser::parseStyleBlock() {
    auto styleNode = std::make_unique<StyleBlockNode>();
    consume(TokenType::OpenBrace, "Expected '{' after 'style' keyword.");

    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        if (match({TokenType::AtStyle})) {
            const Token& name = consume(TokenType::Identifier, "Expected template name after '@Style'.");
            std::string qualified_name;

            if (match({TokenType::From})) {
                 const Token& ns = consume(TokenType::Identifier, "Expected namespace name after 'from'.");
                 qualified_name = ns.lexeme + "::" + name.lexeme;
            } else {
                qualified_name = resolveUnqualifiedName(name.lexeme, TemplateType::Style);
            }

            if (!context_->style_templates_.count(qualified_name)) {
                 throw std::runtime_error("Use of undefined style template '" + qualified_name + "'.");
            }
            const auto& templateNode = context_->style_templates_.at(qualified_name);

            if (templateNode->is_custom_) {
                if (match({TokenType::OpenBrace})) {
                    std::unordered_map<std::string, std::vector<PropertyValue>> provided_values;
                    std::unordered_set<std::string> deleted_properties;
                    std::unordered_set<std::string> deleted_templates;

                    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
                        if (match({TokenType::Delete})) {
                            if (match({TokenType::AtStyle})) {
                                deleted_templates.insert(consume(TokenType::Identifier, "Expected template name after '@Style'.").lexeme);
                            } else {
                                do {
                                    deleted_properties.insert(consume(TokenType::Identifier, "Expected property name after 'delete'.").lexeme);
                                } while (match({TokenType::Comma}));
                            }
                            consume(TokenType::Semicolon, "Expected ';' after delete statement.");
                        } else {
                            const Token& key = consume(TokenType::Identifier, "Expected property name in custom style usage.");
                            consumeColonOrEquals();
                            provided_values[key.lexeme] = parsePropertyValue();
                            consume(TokenType::Semicolon, "Expected ';' after property value.");
                        }
                    }
                    consume(TokenType::CloseBrace, "Expected '}' after custom style block.");

                    std::set<std::string> visited;
                    applyStyleTemplate(*styleNode, qualified_name, provided_values, deleted_properties, deleted_templates, visited);
                } else {
                    consume(TokenType::Semicolon, "Expected ';' after custom template usage.");
                    std::set<std::string> visited;
                    applyStyleTemplate(*styleNode, qualified_name, {}, {}, {}, visited);
                }
            } else {
                consume(TokenType::Semicolon, "Expected ';' after template usage.");
                 std::set<std::string> visited;
                applyStyleTemplate(*styleNode, qualified_name, {}, {}, {}, visited);
            }
        }
        else if (peek().type == TokenType::Identifier && (peekNext().type == TokenType::Colon || peekNext().type == TokenType::Equals)) {
            const Token& key = consume(TokenType::Identifier, "Expected CSS property name.");
            consumeColonOrEquals();
            auto value_parts = parsePropertyValue();
            consume(TokenType::Semicolon, "Expected ';' after CSS property value.");
            styleNode->inline_properties_.emplace_back(key.lexeme, std::move(value_parts));
        }
        else if (peek().type == TokenType::Identifier || peek().type == TokenType::Ampersand || peek().type == TokenType::Dot) {
            std::string selector_str;
            if (peek().type == TokenType::Dot) {
                selector_str += advance().lexeme;
                selector_str += consume(TokenType::Identifier, "Expected class name after '.'").lexeme;
            } else {
                 selector_str = advance().lexeme;
            }
            auto ruleNode = std::make_unique<CssRuleNode>(selector_str);

            consume(TokenType::OpenBrace, "Expected '{' after selector.");
            while (!check(TokenType::CloseBrace) && !isAtEnd()) {
                const Token& key = consume(TokenType::Identifier, "Expected CSS property name inside rule.");
                consumeColonOrEquals();
                auto value_parts = parsePropertyValue();
                consume(TokenType::Semicolon, "Expected ';' after CSS property value.");
                ruleNode->properties_.emplace_back(key.lexeme, std::move(value_parts));
            }
            consume(TokenType::CloseBrace, "Expected '}' after CSS rule block.");
            styleNode->rules_.push_back(std::move(ruleNode));
        }
        else {
            throw std::runtime_error("Unexpected token inside style block.");
        }
    }

    consume(TokenType::CloseBrace, "Expected '}' after style block.");
    return styleNode;
}

std::vector<PropertyValue> CHTLParser::parsePropertyValue() {
    std::vector<PropertyValue> parts;
    while (peek().type != TokenType::Semicolon && !isAtEnd()) {
        Token first = peek();
        Token second = peekNext();

        if (first.type == TokenType::Dot && second.type == TokenType::Identifier && tokens_[current_ + 2].type == TokenType::Dot) {
            advance();
            Token class_name = advance();
            Token selector = {TokenType::Identifier, "." + class_name.lexeme, first.line, first.column};
            advance();
            Token prop = consume(TokenType::Identifier, "Expected property name after '.'.");
            parts.emplace_back(PropertyReferenceNode(selector, prop));
        } else if (first.type == TokenType::Identifier && second.type == TokenType::Dot) {
            Token selector = advance();
            advance();
            Token prop = consume(TokenType::Identifier, "Expected property name after '.'.");
            parts.emplace_back(PropertyReferenceNode(selector, prop));
        } else if (first.type == TokenType::Identifier && second.type == TokenType::OpenParen) {
            // This could be a variable group like `MyVars(var_name)`
            if (tokens_[current_ + 2].type == TokenType::Identifier && tokens_[current_ + 3].type == TokenType::CloseParen) {
                const Token& groupNameToken = advance();
                consume(TokenType::OpenParen, "Expected '(' after variable group name.");
                const Token& varNameToken = consume(TokenType::Identifier, "Expected variable name inside parentheses.");
                consume(TokenType::CloseParen, "Expected ')' after variable name.");

                // Here we resolve the variable group name
                std::string qualified_group_name = resolveUnqualifiedName(groupNameToken.lexeme, TemplateType::Var);

                if (context_->var_templates_.count(qualified_group_name)) {
                    const auto& templateNode = context_->var_templates_.at(qualified_group_name);
                    if (templateNode->variables_.count(varNameToken.lexeme)) {
                        const auto& substituted_parts = templateNode->variables_.at(varNameToken.lexeme);
                        parts.insert(parts.end(), substituted_parts.begin(), substituted_parts.end());
                    } else {
                        throw std::runtime_error("Undefined variable '" + varNameToken.lexeme + "' in group '" + groupNameToken.lexeme + "'.");
                    }
                } else {
                    // This should not be reached if the resolver works.
                    throw std::runtime_error("Use of undefined variable group '" + groupNameToken.lexeme + "'.");
                }
            } else {
                // Not a var group, just a regular token.
                parts.emplace_back(advance());
            }
        }
        else {
            parts.emplace_back(advance());
        }
    }

    if (parts.empty()) {
        throw std::runtime_error("Expected CSS property value.");
    }
    return parts;
}

const Token& CHTLParser::peekNext() const {
    if (current_ + 1 >= tokens_.size()) {
        return tokens_.back();
    }
    return tokens_[current_ + 1];
}

const Token& CHTLParser::peek() const {
    return tokens_[current_];
}

const Token& CHTLParser::previous() const {
    return tokens_[current_ - 1];
}

const Token& CHTLParser::advance() {
    if (!isAtEnd()) current_++;
    return previous();
}

bool CHTLParser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
}

bool CHTLParser::check(TokenType type) const {
    if (isAtEnd()) return false;
    return peek().type == type;
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

const Token& CHTLParser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message + " at line " + std::to_string(peek().line));
}

void CHTLParser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::Semicolon) return;
        switch (peek().type) {
            case TokenType::Text:
            case TokenType::Style:
            case TokenType::Script:
            case TokenType::Import:
            case TokenType::Namespace:
                return;
            default:
                break;
        }
        advance();
    }
}

void CHTLParser::consumeColonOrEquals() {
    if (!match({TokenType::Colon, TokenType::Equals})) {
        throw std::runtime_error("Expected ':' or '=' at line " + std::to_string(peek().line));
    }
}

void CHTLParser::checkConstraints(const ElementNode& parent, const Node& child) {
    for (const auto& constraint : parent.constraints_) {
        if (std::holds_alternative<TypeConstraint>(constraint)) {
            const auto& type_constraint = std::get<TypeConstraint>(constraint);
            if (type_constraint.node_type == BannedNodeType::Html && child.getType() == NodeType::Element) {
                throw std::runtime_error("Constraint violation: HTML elements are not allowed in <" + parent.tagName_ + ">.");
            }
            if (type_constraint.node_type == BannedNodeType::Template && child.getType() == NodeType::Template) {
                 throw std::runtime_error("Constraint violation: [Template] nodes are not allowed in <" + parent.tagName_ + ">.");
            }
             if (type_constraint.node_type == BannedNodeType::Custom && child.getType() == NodeType::Custom) {
                 throw std::runtime_error("Constraint violation: [Custom] nodes are not allowed in <" + parent.tagName_ + ">.");
            }
        } else if (std::holds_alternative<PreciseConstraint>(constraint)) {
            const auto& precise_constraint = std::get<PreciseConstraint>(constraint);
            if (child.getType() == NodeType::Element) {
                const auto& element_child = static_cast<const ElementNode&>(child);
                if (element_child.tagName_ == precise_constraint.name) {
                    throw std::runtime_error("Constraint violation: element <" + precise_constraint.name + "> is not allowed in <" + parent.tagName_ + ">.");
                }
            }
        }
    }
}

std::string CHTLParser::resolveUnqualifiedName(const std::string& name, TemplateType type) {
    // 1. Check global namespace first
    switch (type) {
        case TemplateType::Style:
            if (context_->style_templates_.count(name)) return name;
            break;
        case TemplateType::Element:
            if (context_->element_templates_.count(name)) return name;
            break;
        case TemplateType::Var:
            if (context_->var_templates_.count(name)) return name;
            break;
    }

    // 2. Search imported namespaces
    std::vector<std::string> found_in_namespaces;
    for (const auto& ns : context_->imported_namespaces_) {
        std::string qualified_name = ns + "::" + name;
        bool found = false;
        switch (type) {
            case TemplateType::Style:
                if (context_->style_templates_.count(qualified_name)) found = true;
                break;
            case TemplateType::Element:
                if (context_->element_templates_.count(qualified_name)) found = true;
                break;
            case TemplateType::Var:
                if (context_->var_templates_.count(qualified_name)) found = true;
                break;
        }
        if (found) {
            found_in_namespaces.push_back(ns);
        }
    }

    // 3. Analyze results
    if (found_in_namespaces.empty()) {
        throw std::runtime_error("Use of undefined template '" + name + "'. It was not found in the global scope or any imported namespace.");
    }

    if (found_in_namespaces.size() > 1) {
        std::string error_msg = "Ambiguous reference to template '" + name + "'. It was found in multiple namespaces: ";
        for (const auto& ns : found_in_namespaces) {
            error_msg += ns + " ";
        }
        error_msg += ". Use 'from <namespace>' to disambiguate.";
        throw std::runtime_error(error_msg);
    }

    // Exactly one match found
    return found_in_namespaces[0] + "::" + name;
}

} // namespace CHTL
