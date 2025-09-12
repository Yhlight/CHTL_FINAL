#include "CHTLParser.h"
#include "../CHTLLexer/CHTLLexer.h" // For recursive parsing
#include <iostream>
#include <stdexcept>
#include <unordered_set>

namespace CHTL {

CHTLParser::CHTLParser(const std::string& source, std::vector<Token>& tokens, CHTLLoader& loader, const std::string& initial_path, std::shared_ptr<ParserContext> context)
    : source_(source), tokens_(tokens), loader_(loader), current_path_(initial_path), context_(context) {
    // Check for a namespace declaration at the beginning of the file.
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
        std::string qualified_name = name.lexeme;
        if (match({TokenType::From})) {
            const Token& ns = consume(TokenType::Identifier, "Expected namespace name after 'from'.");
            qualified_name = ns.lexeme + "::" + name.lexeme;
        }

        if (!context_->element_templates_.count(qualified_name)) {
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
            const Token& tagName = consume(TokenType::Identifier, "Expected tag name for delete target.");
            size_t index = 0;
            if (match({TokenType::OpenBracket})) {
                const Token& indexToken = consume(TokenType::Number, "Expected index number in brackets.");
                index = std::stoul(indexToken.lexeme);
                consume(TokenType::CloseBracket, "Expected ']' after index.");
            }
            consume(TokenType::Semicolon, "Expected ';' after delete statement.");

            auto target_it = target_nodes.begin();
            bool found = false;
            size_t current_index = context_->config_.INDEX_INITIAL_COUNT;
            for (auto it = target_nodes.begin(); it != target_nodes.end(); ++it) {
                if ((*it)->getType() == NodeType::Element) {
                    ElementNode* target_element = static_cast<ElementNode*>((*it).get());
                    if (target_element->tagName_ == tagName.lexeme) {
                        if (current_index == index) {
                            target_it = it;
                            found = true;
                            break;
                        }
                        current_index++;
                    }
                }
            }

            if (found) {
                target_nodes.erase(target_it);
            } else {
                throw std::runtime_error("Could not find element '" + tagName.lexeme + "' at index " + std::to_string(index) + " to delete.");
            }
        } else if (match({TokenType::Insert})) {
            if (match({TokenType::After, TokenType::Before, TokenType::Replace, TokenType::AtTop, TokenType::AtBottom})) {
                const Token& position = previous();
                const Token& tagName = consume(TokenType::Identifier, "Expected tag name for insert target.");
                size_t index = 0;
                if (match({TokenType::OpenBracket})) {
                    const Token& indexToken = consume(TokenType::Number, "Expected index number in brackets.");
                    index = std::stoul(indexToken.lexeme);
                    consume(TokenType::CloseBracket, "Expected ']' after index.");
                }

                consume(TokenType::OpenBrace, "Expected '{' for insert body.");
                std::vector<std::unique_ptr<Node>> new_nodes;
                while (!check(TokenType::CloseBrace) && !isAtEnd()) {
                    auto parsed_nodes = parseDeclaration();
                    for(auto& node : parsed_nodes) {
                        new_nodes.push_back(std::move(node));
                    }
                }
                consume(TokenType::CloseBrace, "Expected '}' to close insert body.");

                auto target_it = target_nodes.begin();
                bool found = false;
                if (position.type != TokenType::AtTop && position.type != TokenType::AtBottom) {
                    size_t current_index = context_->config_.INDEX_INITIAL_COUNT;
                    for (auto it = target_nodes.begin(); it != target_nodes.end(); ++it) {
                        if ((*it)->getType() == NodeType::Element) {
                            ElementNode* target_element = static_cast<ElementNode*>((*it).get());
                            if (target_element->tagName_ == tagName.lexeme) {
                                if (current_index == index) {
                                    target_it = it;
                                    found = true;
                                    break;
                                }
                                current_index++;
                            }
                        }
                    }
                    if (!found) {
                        throw std::runtime_error("Could not find element '" + tagName.lexeme + "' at index " + std::to_string(index) + " for insert operation.");
                    }
                }

                switch (position.type) {
                    case TokenType::Before:
                        target_nodes.insert(target_it, std::make_move_iterator(new_nodes.begin()), std::make_move_iterator(new_nodes.end()));
                        break;
                    case TokenType::After:
                        target_nodes.insert(std::next(target_it), std::make_move_iterator(new_nodes.begin()), std::make_move_iterator(new_nodes.end()));
                        break;
                    case TokenType::Replace:
                        target_it = target_nodes.erase(target_it);
                        target_nodes.insert(target_it, std::make_move_iterator(new_nodes.begin()), std::make_move_iterator(new_nodes.end()));
                        break;
                    case TokenType::AtTop:
                        target_nodes.insert(target_nodes.begin(), std::make_move_iterator(new_nodes.begin()), std::make_move_iterator(new_nodes.end()));
                        break;
                    case TokenType::AtBottom:
                        target_nodes.insert(target_nodes.end(), std::make_move_iterator(new_nodes.begin()), std::make_move_iterator(new_nodes.end()));
                        break;
                    default:
                        break;
                }
            } else {
                throw std::runtime_error("Expected a position keyword (after, before, etc.) after 'insert'.");
            }
        } else {
            const Token& tagName = consume(TokenType::Identifier, "Expected tag name for specialization.");
            size_t index = 0;
            if (match({TokenType::OpenBracket})) {
                const Token& indexToken = consume(TokenType::Number, "Expected index number in brackets.");
                index = std::stoul(indexToken.lexeme);
                consume(TokenType::CloseBracket, "Expected ']' after index.");
            }

            auto modifier_element = std::make_unique<ElementNode>(tagName.lexeme);
            consume(TokenType::OpenBrace, "Expected '{' for specialization body.");
            parseElementBody(*modifier_element);
            consume(TokenType::CloseBrace, "Expected '}' to close specialization body.");

            Node* target_node = nullptr;
            size_t current_index = context_->config_.INDEX_INITIAL_COUNT;
            for (auto& target : target_nodes) {
                if (target->getType() == NodeType::Element) {
                    ElementNode* target_element = static_cast<ElementNode*>(target.get());
                    if (target_element->tagName_ == tagName.lexeme) {
                        if (current_index == index) {
                            target_node = target_element;
                            break;
                        }
                        current_index++;
                    }
                }
            }

            if (target_node) {
                StyleBlockNode* modifier_style_block = nullptr;
                for (const auto& child : modifier_element->children_) {
                    if (child->getType() == NodeType::StyleBlock) {
                        modifier_style_block = static_cast<StyleBlockNode*>(child.get());
                        break;
                    }
                }

                if (modifier_style_block) {
                    StyleBlockNode* target_style_block = nullptr;
                    ElementNode* target_element_node = static_cast<ElementNode*>(target_node);
                    for (const auto& child : target_element_node->children_) {
                        if (child->getType() == NodeType::StyleBlock) {
                            target_style_block = static_cast<StyleBlockNode*>(child.get());
                            break;
                        }
                    }
                    if (!target_style_block) {
                        auto new_style_block = std::make_unique<StyleBlockNode>();
                        target_style_block = new_style_block.get();
                        target_element_node->children_.push_back(std::move(new_style_block));
                    }

                    target_style_block->inline_properties_.insert(
                        target_style_block->inline_properties_.end(),
                        modifier_style_block->inline_properties_.begin(),
                        modifier_style_block->inline_properties_.end()
                    );
                    target_style_block->rules_.insert(
                        target_style_block->rules_.end(),
                        std::make_move_iterator(modifier_style_block->rules_.begin()),
                        std::make_move_iterator(modifier_style_block->rules_.end())
                    );
                }
            } else {
                throw std::runtime_error("Could not find element '" + tagName.lexeme + "' at index " + std::to_string(index) + " to specialize.");
            }
        }
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
        if (peek().type == TokenType::Identifier && peekNext().type == TokenType::Colon) {
            const Token& key = consume(TokenType::Identifier, "Expected attribute name.");
            consume(TokenType::Colon, "Expected ':' after attribute name.");

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
            auto nodes = parseDeclaration();
            for (auto& node : nodes) {
                element.children_.push_back(std::move(node));
            }
        }
    }
}

std::unique_ptr<TextNode> CHTLParser::parseText() {
    consume(TokenType::OpenBrace, "Expected '{' after 'text' keyword.");
    const Token& content = consume(TokenType::StringLiteral, "Expected a string literal inside text block.");
    consume(TokenType::CloseBrace, "Expected '}' after text content.");
    return std::make_unique<TextNode>(content.lexeme);
}

std::unique_ptr<CommentNode> CHTLParser::parseGeneratorComment() {
    return std::make_unique<CommentNode>(previous().lexeme);
}

void CHTLParser::parseConfigurationBlock() {
    consume(TokenType::OpenBrace, "Expected '{' after [Configuration] keyword.");
    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        const Token& key = consume(TokenType::Identifier, "Expected configuration key.");
        consume(TokenType::Equals, "Expected '=' after configuration key.");

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
            const Token& key = consume(TokenType::Identifier, "Expected CSS property name in template.");

            std::vector<Token> value_tokens;
            if (!match({TokenType::Colon})) {
                if (!match({TokenType::Comma, TokenType::Semicolon})) {
                    throw std::runtime_error("Expected ',' or ';' after placeholder property.");
                }
            } else {
                while(peek().type != TokenType::Semicolon && !isAtEnd()) {
                    value_tokens.push_back(advance());
                }
                if (value_tokens.empty()) {
                    throw std::runtime_error("Expected CSS property value in template.");
                }
                consume(TokenType::Semicolon, "Expected ';' after property value.");
            }

            templateNode->properties_.emplace_back(key.lexeme, value_tokens);
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
            consume(TokenType::Colon, "Expected ':' after variable name.");

            std::vector<Token> value_tokens;
            while(peek().type != TokenType::Semicolon && !isAtEnd()) {
                value_tokens.push_back(advance());
            }
            if (value_tokens.empty()) {
                throw std::runtime_error("Expected variable value in template.");
            }

            consume(TokenType::Semicolon, "Expected ';' after variable value.");
            templateNode->variables_[key.lexeme] = value_tokens;
        }
        consume(TokenType::CloseBrace, "Expected '}' after template body.");
        context_->var_templates_[qualified_name] = templateNode;
    }
    else {
        throw std::runtime_error("Expected '@Style', '@Element', or '@Var' after '[Template]'.");
    }
}

std::unique_ptr<StyleBlockNode> CHTLParser::parseStyleBlock() {
    auto styleNode = std::make_unique<StyleBlockNode>();
    consume(TokenType::OpenBrace, "Expected '{' after 'style' keyword.");

    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        if (match({TokenType::AtStyle})) {
            const Token& name = consume(TokenType::Identifier, "Expected template name after '@Style'.");
            std::string qualified_name = name.lexeme;
            bool is_custom_usage = check(TokenType::OpenBrace);

            if (!is_custom_usage && match({TokenType::From})) {
                 const Token& ns = consume(TokenType::Identifier, "Expected namespace name after 'from'.");
                 qualified_name = ns.lexeme + "::" + name.lexeme;
            }

            if (!context_->style_templates_.count(qualified_name)) {
                throw std::runtime_error("Use of undefined style template '" + qualified_name + "'.");
            }
            const auto& templateNode = context_->style_templates_.at(qualified_name);

            if (templateNode->is_custom_) {
                consume(TokenType::OpenBrace, "Expected '{' for custom style usage.");
                std::unordered_map<std::string, std::vector<Token>> provided_values;
                std::unordered_set<std::string> deleted_properties;

                while (!check(TokenType::CloseBrace) && !isAtEnd()) {
                    if (match({TokenType::Delete})) {
                        do {
                            deleted_properties.insert(consume(TokenType::Identifier, "Expected property name after 'delete'.").lexeme);
                        } while (match({TokenType::Comma}));
                        consume(TokenType::Semicolon, "Expected ';' after delete statement.");
                    } else {
                        const Token& key = consume(TokenType::Identifier, "Expected property name in custom style usage.");
                        consume(TokenType::Colon, "Expected ':' after property name.");
                        provided_values[key.lexeme] = parsePropertyValue();
                        consume(TokenType::Semicolon, "Expected ';' after property value.");
                    }
                }
                consume(TokenType::CloseBrace, "Expected '}' after custom style block.");

                for (const auto& prop : templateNode->properties_) {
                    const std::string& prop_name = prop.first;
                    if (deleted_properties.count(prop_name)) {
                        continue;
                    }

                    const auto& prop_value = prop.second;
                    if (prop_value.empty()) {
                        if (provided_values.count(prop_name)) {
                            styleNode->inline_properties_.emplace_back(prop_name, provided_values.at(prop_name));
                        } else {
                            throw std::runtime_error("Value for placeholder '" + prop_name + "' not provided for '" + prop_name + "'.");
                        }
                    } else {
                        styleNode->inline_properties_.push_back(prop);
                    }
                }

            } else {
                consume(TokenType::Semicolon, "Expected ';' after template usage.");
                for (const auto& prop : templateNode->properties_) {
                    styleNode->inline_properties_.push_back(prop);
                }
            }
        }
        else if (peek().type == TokenType::Identifier && peekNext().type == TokenType::Colon) {
            const Token& key = consume(TokenType::Identifier, "Expected CSS property name.");
            consume(TokenType::Colon, "Expected ':' after CSS property name.");

            std::vector<Token> value_tokens = parsePropertyValue();

            consume(TokenType::Semicolon, "Expected ';' after CSS property value.");
            styleNode->inline_properties_.emplace_back(key.lexeme, value_tokens);
        }
        else if (peek().type == TokenType::Identifier || peek().type == TokenType::Ampersand) {
            const Token& selector = consume(TokenType::Identifier, "Expected a CSS selector.");
            auto ruleNode = std::make_unique<CssRuleNode>(selector.lexeme);

            consume(TokenType::OpenBrace, "Expected '{' after selector.");

            while (!check(TokenType::CloseBrace) && !isAtEnd()) {
                const Token& key = consume(TokenType::Identifier, "Expected CSS property name inside rule.");
                consume(TokenType::Colon, "Expected ':' after CSS property name.");

                std::vector<Token> value_tokens = parsePropertyValue();

                consume(TokenType::Semicolon, "Expected ';' after CSS property value.");
                ruleNode->properties_.emplace_back(key.lexeme, value_tokens);
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

std::vector<Token> CHTLParser::parsePropertyValue() {
    std::vector<Token> final_tokens;
    while (peek().type != TokenType::Semicolon && !isAtEnd()) {
        if (peek().type == TokenType::Identifier && peekNext().type == TokenType::OpenParen) {
            const Token& groupName = advance();
            consume(TokenType::OpenParen, "Expected '(' after variable group name.");
            const Token& varName = consume(TokenType::Identifier, "Expected variable name inside parentheses.");
            consume(TokenType::CloseParen, "Expected ')' after variable name.");

            if (context_->var_templates_.count(groupName.lexeme)) {
                const auto& templateNode = context_->var_templates_.at(groupName.lexeme);
                if (templateNode->variables_.count(varName.lexeme)) {
                    const auto& substituted_tokens = templateNode->variables_.at(varName.lexeme);
                    final_tokens.insert(final_tokens.end(), substituted_tokens.begin(), substituted_tokens.end());
                } else {
                    throw std::runtime_error("Undefined variable '" + varName.lexeme + "' in group '" + groupName.lexeme + "'.");
                }
            } else {
                throw std::runtime_error("Use of undefined variable group '" + groupName.lexeme + "'.");
            }
        } else {
            final_tokens.push_back(advance());
        }
    }

    if (final_tokens.empty()) {
        throw std::runtime_error("Expected CSS property value.");
    }
    return final_tokens;
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

} // namespace CHTL
