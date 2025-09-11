#include "Parser.hpp"
#include <stdexcept>
#include "../CHTLNode/StyleNode.hpp" // For StyleNode and StyleRule
#include "../CHTLNode/TemplateDefinitionNode.hpp"

namespace CHTL {

Parser::Parser(const std::vector<Token>& tokens) : m_tokens(tokens) {}

std::vector<NodePtr> Parser::parse() {
    std::vector<NodePtr> statements;
    while (!isAtEnd()) {
        statements.push_back(declaration());
    }
    return statements;
}

// === Parsing Rules ===

NodePtr Parser::declaration() {
    try {
        if (match({TokenType::TEMPLATE})) {
            return templateDefinition();
        }
        if (match({TokenType::GENERATOR_COMMENT})) {
            auto comment = std::make_shared<CommentNode>();
            comment->content = previous().lexeme;
            comment->line = previous().line;
            return comment;
        }
        if (peek().type == TokenType::IDENTIFIER) {
            if (peek().lexeme == "text") {
                return textBlock();
            }
            return element();
        }
    } catch (std::runtime_error& e) {
        // In the future, we can add error recovery here (synchronize())
        throw;
    }

    throw std::runtime_error("Expect a declaration (element, comment, etc). Found " + peek().lexeme);
}

NodePtr Parser::element() {
    auto node = std::make_shared<ElementNode>();
    node->tagName = consume(TokenType::IDENTIFIER, "Expect element name.").lexeme;
    node->line = previous().line;

    consume(TokenType::LEFT_BRACE, "Expect '{' after element name.");

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (peek().type == TokenType::GENERATOR_COMMENT) {
            advance(); // Consume the comment token
            auto comment = std::make_shared<CommentNode>();
            comment->content = previous().lexeme;
            comment->line = previous().line;
            node->children.push_back(comment);
        } else if (peek().type == TokenType::IDENTIFIER) {
            // Look ahead to distinguish between an attribute and a child element.
            if (m_current + 1 < m_tokens.size() &&
                (m_tokens[m_current + 1].type == TokenType::COLON || m_tokens[m_current + 1].type == TokenType::EQUAL)) {
                node->attributes.push_back(attribute());
            } else if (peek().lexeme == "text") {
                node->children.push_back(textBlock());
            } else if (peek().lexeme == "style") {
                // Look ahead to confirm it's a block
                if (m_current + 1 < m_tokens.size() && m_tokens[m_current + 1].type == TokenType::LEFT_BRACE) {
                    node->children.push_back(styleBlock());
                } else {
                    // It's just a normal element named "style"
                    node->children.push_back(element());
                }
            }
            else {
                node->children.push_back(element());
            }
        } else if (peek().type == TokenType::STRING) {
            auto textNode = std::make_shared<TextNode>();
            textNode->line = peek().line;
            textNode->textTokens.push_back(advance()); // Consume only the single STRING token
            node->children.push_back(textNode);
        }
        else {
            throw std::runtime_error("Unexpected token inside element: " + peek().lexeme);
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after element block.");
    return node;
}

NodePtr Parser::textBlock() {
    auto node = std::make_shared<TextNode>();
    consume(TokenType::IDENTIFIER, "Expect 'text' keyword."); // Assumes lexeme is "text"
    node->line = previous().line;
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'text'.");

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        node->textTokens.push_back(advance());
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after text block.");
    return node;
}

std::shared_ptr<AttributeNode> Parser::attribute() {
    auto node = std::make_shared<AttributeNode>();
    node->name = consume(TokenType::IDENTIFIER, "Expect attribute name.").lexeme;
    node->line = previous().line;

    if (!match({TokenType::COLON, TokenType::EQUAL})) {
        throw std::runtime_error("Expect ':' or '=' after attribute name.");
    }

    while (!check(TokenType::SEMICOLON) && !isAtEnd()) {
        node->valueTokens.push_back(advance());
    }

    consume(TokenType::SEMICOLON, "Expect ';' after attribute value.");
    return node;
}

NodePtr Parser::styleBlock() {
    auto node = std::make_shared<StyleNode>();
    consume(TokenType::IDENTIFIER, "Expect 'style' keyword."); // Assumes lexeme is "style"
    node->line = previous().line;
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'style'.");

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        StyleRule rule;
        while (!check(TokenType::SEMICOLON) && !check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            rule.push_back(advance());
        }
        if (match({TokenType::SEMICOLON})) {
            rule.push_back(previous()); // include the semicolon in the rule
        }
        if (!rule.empty()) {
            node->rules.push_back(rule);
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after style block.");
    return node;
}

NodePtr Parser::templateDefinition() {
    auto node = std::make_shared<TemplateDefinitionNode>();
    node->line = previous().line;

    consume(TokenType::AT, "Expect '@' after [Template].");
    Token type = consume(TokenType::IDENTIFIER, "Expect template type (Style, Element, Var).");
    node->name = consume(TokenType::IDENTIFIER, "Expect template name.").lexeme;

    if (type.lexeme == "Style") {
        node->templateType = TemplateType::STYLE;
        consume(TokenType::LEFT_BRACE, "Expect '{' after style template name.");
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            StyleRule rule;
            while (!check(TokenType::SEMICOLON) && !check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                rule.push_back(advance());
            }
            if (match({TokenType::SEMICOLON})) {
                rule.push_back(previous());
            }
            if (!rule.empty()) {
                node->styleRules.push_back(rule);
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after style template body.");
    } else if (type.lexeme == "Element") {
        node->templateType = TemplateType::ELEMENT;
        consume(TokenType::LEFT_BRACE, "Expect '{' after element template name.");
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            node->bodyNodes.push_back(declaration());
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after element template body.");
    } else if (type.lexeme == "Var") {
        node->templateType = TemplateType::VAR;
        // TODO: Implement Var template parsing
        consume(TokenType::LEFT_BRACE, "Expect '{' for Var template.");
        while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
            advance(); // Just consume tokens for now
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after Var template.");
    } else {
        throw std::runtime_error("Unknown template type: " + type.lexeme);
    }

    return node;
}


// === Helper Methods ===

Token Parser::advance() {
    if (!isAtEnd()) m_current++;
    return previous();
}

Token Parser::peek() {
    return m_tokens[m_current];
}

Token Parser::previous() {
    return m_tokens[m_current - 1];
}

bool Parser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}

bool Parser::match(const std::vector<TokenType>& types) {
    for (TokenType type : types) {
        if (check(type)) {
            advance();
            return true;
        }
    }
    return false;
}

Token Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message + " (Found " + peek().lexeme + " instead)");
}

} // namespace CHTL
