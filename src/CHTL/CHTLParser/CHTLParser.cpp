#include "CHTLParser.h"
#include "../CHTLLexer/CHTLLexer.h" // For recursive parsing
#include <iostream>
#include <stdexcept>
#include <unordered_set>

namespace CHTL {

CHTLParser::CHTLParser(std::vector<Token>& tokens, CHTLLoader& loader, const std::string& initial_path, std::shared_ptr<ParserContext> context)
    : tokens_(tokens), loader_(loader), current_path_(initial_path), context_(context) {
    // Check for a namespace declaration at the beginning of the file.
    if (tokens_.size() > 2 && tokens_[0].type == TokenType::Namespace && tokens_[1].type == TokenType::Identifier) {
        current_namespace_ = tokens_[1].lexeme;
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
        // Namespaces don't produce a node, so we just continue parsing declarations.
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
            CHTLParser imported_parser(imported_tokens, loader_, imported_file_canonical_path, context_);

            // Handle default namespace
            if (imported_parser.current_namespace_.empty()) {
                imported_parser.current_namespace_ = default_namespace;
            }

            imported_parser.parse();
        }

        return nodes; // Imports don't produce direct nodes
    }
    if (match({TokenType::Template, TokenType::Custom})) {
        bool is_custom = previous().type == TokenType::Custom;
        parseTemplateDefinition(is_custom);
        return nodes; // Return empty vector for definitions
    }
    if (match({TokenType::AtElement})) {
        const Token& name = consume(TokenType::Identifier, "Expected template name after '@Element'.");
        std::string qualified_name = name.lexeme;
        if (match({TokenType::From})) {
            const Token& ns = consume(TokenType::Identifier, "Expected namespace name after 'from'.");
            qualified_name = ns.lexeme + "::" + name.lexeme;
        }
        consume(TokenType::Semicolon, "Expected ';' after element template usage.");

        if (context_->element_templates_.count(qualified_name)) {
            const auto& templateNode = context_->element_templates_.at(qualified_name);
            for (const auto& child : templateNode->children_) {
                nodes.push_back(child->clone());
            }
            return nodes;
        } else {
            throw std::runtime_error("Use of undefined element template '" + name.lexeme + "'.");
        }
    }

    // For single-node declarations, wrap them in a vector
    std::unique_ptr<Node> singleNode = nullptr;
    if (match({TokenType::Style})) {
        singleNode = parseStyleBlock();
    } else if (match({TokenType::Identifier})) {
        if (peek().type == TokenType::OpenBrace) {
            singleNode = parseElement();
        }
    } else if (match({TokenType::Text})) {
        singleNode = parseText();
    } else if (match({TokenType::GeneratorComment})) {
        singleNode = parseGeneratorComment();
    }

    if(singleNode) {
        nodes.push_back(std::move(singleNode));
        return nodes;
    }

    // If no declaration matches, advance and report error.
    advance();
    throw std::runtime_error("Expected a declaration (element, text, etc.).");
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
        // If it's an identifier followed by a colon, it's an attribute.
        if (peek().type == TokenType::Identifier && peekNext().type == TokenType::Colon) {
            const Token& key = consume(TokenType::Identifier, "Expected attribute name.");
            consume(TokenType::Colon, "Expected ':' after attribute name.");

            // This logic for parsing a single value is now incorrect, as values can be expressions.
            // However, for attributes, they are typically single values. This might need a revisit.
            std::string value;
            if (match({TokenType::StringLiteral, TokenType::UnquotedLiteral, TokenType::Identifier})) {
                value = previous().lexeme;
            } else {
                throw std::runtime_error("Expected attribute value.");
            }

            consume(TokenType::Semicolon, "Expected ';' after attribute value.");
            element.attributes_.push_back(std::make_unique<AttributeNode>(key.lexeme, value));
        }
        // Otherwise, it must be a declaration (like a child element or style block).
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
    // For now, we only support a single string literal inside text {}
    const Token& content = consume(TokenType::StringLiteral, "Expected a string literal inside text block.");
    consume(TokenType::CloseBrace, "Expected '}' after text content.");
    return std::make_unique<TextNode>(content.lexeme);
}

std::unique_ptr<CommentNode> CHTLParser::parseGeneratorComment() {
    // The lexeme of the GeneratorComment token now holds the content.
    return std::make_unique<CommentNode>(previous().lexeme);
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
            // If it's a placeholder (no colon), the value is empty.
            if (!match({TokenType::Colon})) {
                // It's a placeholder, just consume the comma or semicolon
                if (!match({TokenType::Comma, TokenType::Semicolon})) {
                    throw std::runtime_error("Expected ',' or ';' after placeholder property.");
                }
            } else {
                // It has a value, parse it.
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
        // Check for template usage
        if (match({TokenType::AtStyle})) {
            const Token& name = consume(TokenType::Identifier, "Expected template name after '@Style'.");
            std::string qualified_name = name.lexeme;
            bool is_custom_usage = check(TokenType::OpenBrace);

            // Only look for 'from' if it's NOT a custom style usage with a block
            if (!is_custom_usage && match({TokenType::From})) {
                 const Token& ns = consume(TokenType::Identifier, "Expected namespace name after 'from'.");
                 qualified_name = ns.lexeme + "::" + name.lexeme;
            }

            if (!context_->style_templates_.count(qualified_name)) {
                throw std::runtime_error("Use of undefined style template '" + qualified_name + "'.");
            }
            const auto& templateNode = context_->style_templates_.at(qualified_name);

            // If it's a custom style, it must be used with a block
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

                // Expand the template with the provided values
                for (const auto& prop : templateNode->properties_) {
                    const std::string& prop_name = prop.first;
                    // Skip deleted properties
                    if (deleted_properties.count(prop_name)) {
                        continue;
                    }

                    const auto& prop_value = prop.second;
                    if (prop_value.empty()) { // It's a placeholder
                        if (provided_values.count(prop_name)) {
                            styleNode->inline_properties_.emplace_back(prop_name, provided_values.at(prop_name));
                        } else {
                            throw std::runtime_error("Value for placeholder '" + prop_name + "' not provided for '" + prop_name + "'.");
                        }
                    } else { // It's a regular property from the template
                        styleNode->inline_properties_.push_back(prop);
                    }
                }

            } else { // It's a regular template
                consume(TokenType::Semicolon, "Expected ';' after template usage.");
                for (const auto& prop : templateNode->properties_) {
                    styleNode->inline_properties_.push_back(prop);
                }
            }
        }
        // If it's an identifier followed by a colon, it's an inline property.
        else if (peek().type == TokenType::Identifier && peekNext().type == TokenType::Colon) {
            const Token& key = consume(TokenType::Identifier, "Expected CSS property name.");
            consume(TokenType::Colon, "Expected ':' after CSS property name.");

            std::vector<Token> value_tokens = parsePropertyValue();

            consume(TokenType::Semicolon, "Expected ';' after CSS property value.");
            styleNode->inline_properties_.emplace_back(key.lexeme, value_tokens);
        }
        // Otherwise, assume it's a CSS rule (e.g., .class, #id, &)
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
        // Check for variable usage: GroupName(VarName)
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
            // It's a regular token, just add it to the list.
            final_tokens.push_back(advance());
        }
    }

    if (final_tokens.empty()) {
        throw std::runtime_error("Expected CSS property value.");
    }
    return final_tokens;
}

// --- Helper Methods ---

const Token& CHTLParser::peekNext() const {
    if (current_ + 1 >= tokens_.size()) {
        // Return the EOF token if we are at the end
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
