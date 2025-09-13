#include "CHTLParser.h"
#include "../CHTLLexer/CHTLLexer.h"
#include <iostream>
#include <stdexcept>
#include <unordered_set>
#include <set>
#include <algorithm>
#include "../CHTLNode/PropertyReferenceNode.h"
#include "../CHTLNode/CssRuleNode.h"

namespace CHTL {

CHTLParser::CHTLParser(const std::string& source, std::vector<Token>& tokens, CHTLLoader& loader, const std::string& initial_path, std::shared_ptr<ParserContext> context)
    : source_(source), tokens_(tokens), loader_(loader), current_path_(initial_path), context_(context) {}

std::unique_ptr<RootNode> CHTLParser::parse() {
    auto root = std::make_unique<RootNode>();
    while (!isAtEnd()) {
        try {
            auto nodes = parseDeclaration();
            for (auto& node : nodes) {
                root->children_.push_back(std::move(node));
            }
        } catch (const std::runtime_error& e) {
            std::cerr << "Parse Error: " << e.what() << " on line " << (peek().line) << std::endl;
            synchronize();
            hadError_ = true;
        }
    }
    return root;
}

std::vector<std::unique_ptr<Node>> CHTLParser::parseDeclaration() {
    std::vector<std::unique_ptr<Node>> nodes;
    if (match({TokenType::Template, TokenType::Custom})) {
        bool is_custom = previous().type == TokenType::Custom;
        parseTemplateDefinition(is_custom);
        return nodes;
    }
    // ... (other top-level declarations)

    std::unique_ptr<Node> singleNode;
    if (match({TokenType::Identifier})) {
        singleNode = parseElement();
    } else if (match({TokenType::Style})) {
        singleNode = parseStyleBlock();
    }
    // ... (other node types)

    if(singleNode) {
        nodes.push_back(std::move(singleNode));
        return nodes;
    }

    if (isAtEnd()) return nodes;

    throw std::runtime_error("Expected a declaration.");
}

// --- Expression Parsing ---

PropertyDeclaration CHTLParser::parsePropertyDeclaration() {
    const Token& key = consume(TokenType::Identifier, "Expected CSS property name.");
    consumeColonOrEquals();
    auto value_expr = parseExpression();
    consume(TokenType::Semicolon, "Expected ';' after CSS property value.");
    return PropertyDeclaration(key.lexeme, std::move(value_expr));
}

std::unique_ptr<ExpressionNode> CHTLParser::parseExpression() {
    return parseConditional();
}

std::unique_ptr<ExpressionNode> CHTLParser::parseConditional() {
    auto expr = parseTerm();
    if (match({TokenType::QuestionMark})) {
        auto thenBranch = parseExpression();
        consume(TokenType::Colon, "Expected ':' in conditional expression.");
        auto elseBranch = parseExpression();
        return std::make_unique<ConditionalExpression>(std::move(expr), std::move(thenBranch), std::move(elseBranch));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseTerm() {
    auto expr = parseFactor();
    while (match({TokenType::Plus, TokenType::Minus})) {
        Token op = previous();
        auto right = parseFactor();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parseFactor() {
    auto expr = parsePrimary();
    while (match({TokenType::Asterisk, TokenType::Slash, TokenType::Percent, TokenType::DoubleAsterisk})) {
        Token op = previous();
        auto right = parsePrimary();
        expr = std::make_unique<BinaryExpression>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> CHTLParser::parsePrimary() {
    if (match({TokenType::UnquotedLiteral, TokenType::Number, TokenType::StringLiteral})) {
        Token first_token = previous();
        std::string combined_lexeme = first_token.lexeme;
        while (peek().type == TokenType::UnquotedLiteral || peek().type == TokenType::Number || peek().type == TokenType::Identifier) {
             combined_lexeme += " " + advance().lexeme;
        }
        return std::make_unique<LiteralExpression>(Token{first_token.type, combined_lexeme, first_token.line, first_token.column});
    }
    if (match({TokenType::Identifier})) {
        Token first = previous();
        if (match({TokenType::Dot})) {
            Token prop = consume(TokenType::Identifier, "Expected property name after '.'.");
            return std::make_unique<ReferenceExpression>(PropertyReferenceNode(first, prop));
        }
        if (match({TokenType::OpenParen})) {
            const Token& varName = consume(TokenType::Identifier, "Expected variable name.");
            consume(TokenType::CloseParen, "Expected ')' after variable name.");
            if (context_->var_templates_.count(first.lexeme)) {
                const auto& templateNode = context_->var_templates_.at(first.lexeme);
                if (templateNode->variables_.count(varName.lexeme)) {
                    return templateNode->variables_.at(varName.lexeme)->clone();
                }
            }
            throw std::runtime_error("Use of undefined variable '" + varName.lexeme + "' in group '" + first.lexeme + "'.");
        }
        return std::make_unique<LiteralExpression>(first);
    }
    if (match({TokenType::OpenParen})) {
        auto expr = parseExpression();
        consume(TokenType::CloseParen, "Expected ')' after expression.");
        return expr;
    }
    throw std::runtime_error("Expected primary expression.");
}

// --- Template and Style Parsing ---

void CHTLParser::parseTemplateDefinition(bool is_custom) {
    if (match({TokenType::AtStyle})) {
        const Token& name = consume(TokenType::Identifier, "Expected template name.");
        auto templateNode = std::make_shared<StyleTemplateNode>(name.lexeme, is_custom);
        consume(TokenType::OpenBrace, "Expected '{' after template name.");
        while (!check(TokenType::CloseBrace) && !isAtEnd()) {
            if (match({TokenType::AtStyle})) {
                const Token& inheritedName = consume(TokenType::Identifier, "Expected inherited template name.");
                templateNode->inherits_.push_back(inheritedName.lexeme);
                if (is_custom && match({TokenType::OpenBrace})) {
                    SpecializationInfo spec_info;
                    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
                        consume(TokenType::Delete, "Expected 'delete' keyword.");
                        if (match({TokenType::AtStyle})) {
                            spec_info.deleted_templates.insert(consume(TokenType::Identifier, "Expected template name.").lexeme);
                        } else {
                            do {
                                spec_info.deleted_properties.insert(consume(TokenType::Identifier, "Expected property name.").lexeme);
                            } while (match({TokenType::Comma}));
                        }
                        consume(TokenType::Semicolon, "Expected ';' after delete.");
                    }
                    consume(TokenType::CloseBrace, "Expected '}' after specialization.");
                    templateNode->inheritance_specializations_[inheritedName.lexeme] = spec_info;
                } else {
                    consume(TokenType::Semicolon, "Expected ';' after inherited template.");
                }
            } else {
                const Token& key = consume(TokenType::Identifier, "Expected CSS property name.");
                if (match({TokenType::Semicolon})) {
                    templateNode->properties_.emplace_back(key.lexeme, nullptr);
                } else {
                    consumeColonOrEquals();
                    templateNode->properties_.emplace_back(key.lexeme, parseExpression());
                    consume(TokenType::Semicolon, "Expected ';' after property value.");
                }
            }
        }
        consume(TokenType::CloseBrace, "Expected '}' after template body.");
        context_->style_templates_[name.lexeme] = templateNode;
    } else if (match({TokenType::AtVar})) {
        const Token& name = consume(TokenType::Identifier, "Expected template name.");
        auto templateNode = std::make_shared<VarTemplateNode>(name.lexeme);
        consume(TokenType::OpenBrace, "Expected '{' after template name.");
        while (!check(TokenType::CloseBrace) && !isAtEnd()) {
            const Token& key = consume(TokenType::Identifier, "Expected variable name.");
            consumeColonOrEquals();
            templateNode->variables_[key.lexeme] = parseExpression();
            consume(TokenType::Semicolon, "Expected ';' after variable value.");
        }
        consume(TokenType::CloseBrace, "Expected '}' after template body.");
        context_->var_templates_[name.lexeme] = templateNode;
    }
    // ... other template types
}

void CHTLParser::applyStyleTemplate(
    StyleBlockNode& styleNode,
    const std::string& template_name,
    const std::unordered_map<std::string, std::unique_ptr<ExpressionNode>>& provided_values,
    const std::unordered_set<std::string>& deleted_properties,
    const std::unordered_set<std::string>& deleted_templates,
    std::set<std::string>& visited_templates
) {
    if (visited_templates.count(template_name)) return;
    visited_templates.insert(template_name);

    if (!context_->style_templates_.count(template_name)) {
        throw std::runtime_error("Use of undefined style template '" + template_name + "'.");
    }
    const auto& templateNode = context_->style_templates_.at(template_name);

    for (const auto& inherited_name : templateNode->inherits_) {
        if (deleted_templates.count(inherited_name)) continue;

        std::unordered_set<std::string> prop_deletions = deleted_properties;
        std::unordered_set<std::string> template_deletions = deleted_templates;
        if (templateNode->inheritance_specializations_.count(inherited_name)) {
            const auto& spec_info = templateNode->inheritance_specializations_.at(inherited_name);
            prop_deletions.insert(spec_info.deleted_properties.begin(), spec_info.deleted_properties.end());
            template_deletions.insert(spec_info.deleted_templates.begin(), spec_info.deleted_templates.end());
        }
        applyStyleTemplate(styleNode, inherited_name, provided_values, prop_deletions, template_deletions, visited_templates);
    }

    for (const auto& prop_pair : templateNode->properties_) {
        const std::string& prop_name = prop_pair.first;
        if (deleted_properties.count(prop_name)) continue;

        styleNode.inline_properties_.erase(
            std::remove_if(styleNode.inline_properties_.begin(), styleNode.inline_properties_.end(),
                [&](const PropertyDeclaration& p) { return p.name == prop_name; }),
            styleNode.inline_properties_.end()
        );

        if (const auto& prop_value_expr = prop_pair.second) {
            styleNode.inline_properties_.emplace_back(prop_name, prop_value_expr->clone());
        } else {
            if (provided_values.count(prop_name)) {
                styleNode.inline_properties_.emplace_back(prop_name, provided_values.at(prop_name)->clone());
            } else {
                 throw std::runtime_error("Value for placeholder '" + prop_name + "' not provided.");
            }
        }
    }
}

std::unique_ptr<StyleBlockNode> CHTLParser::parseStyleBlock() {
    auto styleNode = std::make_unique<StyleBlockNode>();
    consume(TokenType::OpenBrace, "Expected '{' after 'style' keyword.");
    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        if (match({TokenType::AtStyle})) {
            const Token& name = consume(TokenType::Identifier, "Expected template name.");
            std::set<std::string> visited;
            applyStyleTemplate(*styleNode, name.lexeme, {}, {}, {}, visited);
            consume(TokenType::Semicolon, "Expected ';' after template usage.");
        }
        else if (peek().type == TokenType::Identifier && (peekNext().type == TokenType::Colon || peekNext().type == TokenType::Equals)) {
            styleNode->inline_properties_.push_back(parsePropertyDeclaration());
        }
        else if (peek().type == TokenType::Identifier || peek().type == TokenType::Dot) {
            std::string selector_str;
            if(match({TokenType::Dot})) selector_str += ".";
            selector_str += consume(TokenType::Identifier, "Expected selector name.").lexeme;

            auto ruleNode = std::make_unique<CssRuleNode>(selector_str);
            consume(TokenType::OpenBrace, "Expected '{' after selector.");
            while (!check(TokenType::CloseBrace) && !isAtEnd()) {
                 ruleNode->properties_.push_back(parsePropertyDeclaration());
            }
            consume(TokenType::CloseBrace, "Expected '}' after CSS rule block.");
            styleNode->rules_.push_back(std::move(ruleNode));
        } else {
            throw std::runtime_error("Unexpected token in style block.");
        }
    }
    consume(TokenType::CloseBrace, "Expected '}' after style block.");
    return styleNode;
}

// ... (rest of the file, including consume, match, etc.)
// Note: This is a partial file for brevity. The unchanged functions are omitted.
void CHTLParser::consumeColonOrEquals() {
    if (!match({TokenType::Colon, TokenType::Equals})) {
        throw std::runtime_error("Expected ':' or '=' at line " + std::to_string(peek().line));
    }
}
const Token& CHTLParser::peekNext() const {
    if (current_ + 1 >= tokens_.size()) {
        return tokens_.back();
    }
    return tokens_[current_ + 1];
}
const Token& CHTLParser::peek() const { return tokens_[current_]; }
const Token& CHTLParser::previous() const { return tokens_[current_ - 1]; }
const Token& CHTLParser::advance() { if (!isAtEnd()) current_++; return previous(); }
bool CHTLParser::isAtEnd() const { return peek().type == TokenType::EndOfFile; }
bool CHTLParser::check(TokenType type) const { if (isAtEnd()) return false; return peek().type == type; }
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
            case TokenType::Text: case TokenType::Style: case TokenType::Script:
            case TokenType::Import: case TokenType::Namespace:
                return;
            default: break;
        }
        advance();
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
    while(!check(TokenType::CloseBrace) && !isAtEnd()) {
        auto nodes = parseDeclaration();
        for(auto& node : nodes) {
            element.children_.push_back(std::move(node));
        }
    }
}

}
