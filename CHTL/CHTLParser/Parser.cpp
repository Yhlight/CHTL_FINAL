#include "Parser.h"
#include <stdexcept>

namespace CHTL {

Parser::Parser(std::vector<Token> tokens)
    : m_tokens(std::move(tokens)), m_position(0) {}

const Token& Parser::peek(size_t offset) const {
    if (m_position + offset >= m_tokens.size()) {
        // Return EOF token
        return m_tokens.back();
    }
    return m_tokens[m_position + offset];
}

Token Parser::consume() {
    if (m_position >= m_tokens.size()) {
        return m_tokens.back(); // EOF
    }
    return m_tokens[m_position++];
}

bool Parser::match(TokenType type) {
    skipComments();
    if (peek().type == type) {
        consume();
        return true;
    }
    return false;
}

Token Parser::expect(TokenType type) {
    skipComments();
    if (peek().type == type) {
        return consume();
    }
    const auto& p = peek();
    throw std::runtime_error("Unexpected token: " + TokenTypeToString(p.type) + " '" + p.value +
                             "', expected " + TokenTypeToString(type) +
                             " at line " + std::to_string(p.line) + ", column " + std::to_string(p.column));
}

void Parser::skipComments() {
    while (peek().type == TokenType::LineComment || peek().type == TokenType::BlockComment || peek().type == TokenType::GeneratorComment) {
        consume();
    }
}

std::vector<std::unique_ptr<Node>> Parser::parse() {
    std::vector<std::unique_ptr<Node>> roots;
    while (peek().type != TokenType::EndOfFile) {
        roots.push_back(parseStatement());
        skipComments();
    }
    return roots;
}

std::unique_ptr<Node> Parser::parseStatement() {
    skipComments();
    const auto& token = peek();
    if (token.type == TokenType::Identifier) {
        return parseElement(consume());
    }
    throw std::runtime_error("Unexpected token at root level: " + TokenTypeToString(token.type) + " '" + token.value + "'");
}

std::unique_ptr<ElementNode> Parser::parseElement(Token identifier) {
    auto element = std::make_unique<ElementNode>(identifier.value);

    // Check for element body
    skipComments();
    if (peek().type == TokenType::OpenBrace) {
        expect(TokenType::OpenBrace);
        parseElementBody(element.get());
        expect(TokenType::CloseBrace);
    }

    return element;
}

void Parser::parseElementBody(ElementNode* element) {
    while (peek().type != TokenType::CloseBrace && peek().type != TokenType::EndOfFile) {
        skipComments();

        const auto& current_token = peek();
        if (current_token.type != TokenType::Identifier) {
            throw std::runtime_error("Expected identifier for attribute or child element, but got " + TokenTypeToString(current_token.type));
        }

        const auto& next_token = peek(1);

        if (next_token.type == TokenType::Colon || next_token.type == TokenType::Equals) {
            // It's an attribute
            Token key = consume(); // Identifier
            consume(); // : or =
            Token value = expect(TokenType::Identifier); // CHTL allows unquoted literals
            element->addAttribute(std::make_unique<AttributeNode>(key.value, value.value));
            expect(TokenType::Semicolon);
} else if (current_token.value == "style" && next_token.type == TokenType::OpenBrace) {
            consume(); // 'style'
            expect(TokenType::OpenBrace);
            element->setStyleBlock(parseStyleBlock());
            expect(TokenType::CloseBrace);
        } else if (current_token.value == "text" && next_token.type == TokenType::OpenBrace) {
            // It's a text node `text { ... }`
            consume(); // text identifier
            expect(TokenType::OpenBrace);
            // The spec allows for unquoted, single quoted, or double quoted strings.
            skipComments();
            Token text_content = peek();
            if (text_content.type == TokenType::Identifier || text_content.type == TokenType::StringLiteral) {
                consume(); // consume the content token
                element->addChild(std::make_unique<TextNode>(text_content.value));
            } else {
                 throw std::runtime_error("Expected a string or unquoted literal inside text block at line " + std::to_string(text_content.line));
            }
            expect(TokenType::CloseBrace);
        } else if (next_token.type == TokenType::OpenBrace || next_token.type == TokenType::Semicolon || peek().type == TokenType::Semicolon || next_token.type == TokenType::EndOfFile) {
            // It's a child element
             element->addChild(parseElement(consume()));
             // Optional semicolon for single-tag elements, e.g. `br;`
             if (peek().type == TokenType::Semicolon) {
                 consume();
             }
        } else {
            throw std::runtime_error("Invalid syntax inside element body at line " + std::to_string(current_token.line));
        }
    }
}

std::unique_ptr<StyleBlockNode> Parser::parseStyleBlock() {
    auto styleNode = std::make_unique<StyleBlockNode>();
    while (peek().type != TokenType::CloseBrace && peek().type != TokenType::EndOfFile) {
        skipComments();
        const auto& current = peek();
        const auto& next = peek(1);

        // If next token is a colon, it's an inline property like `width: 100px;`
        if (current.type == TokenType::Identifier && next.type == TokenType::Colon) {
            Token key = consume();
            consume(); // ':'
            std::string value_str;
            while(peek().type != TokenType::Semicolon && peek().type != TokenType::EndOfFile) {
                if (!value_str.empty()) value_str += " ";
                value_str += consume().value;
            }
            expect(TokenType::Semicolon);
            styleNode->addInlineProperty(std::make_unique<CssPropertyNode>(key.value, value_str));
        }
        // Otherwise, assume it's a selector rule, e.g., `.box { ... }`
        else {
            styleNode->addCssRule(parseCssRule());
        }
    }
    return styleNode;
}

std::unique_ptr<CssRuleNode> Parser::parseCssRule() {
    skipComments();
    std::string selector_str;

    // A selector can be simple (.box) or complex (&:hover).
    // For now, let's handle simple selectors: .box, #id, &
    const auto& first_token = peek();
    if (first_token.type == TokenType::Dot || first_token.type == TokenType::Hash || first_token.type == TokenType::Ampersand) {
        selector_str += consume().value;
    }

    // The selector name itself
    Token selector_name = expect(TokenType::Identifier);
    selector_str += selector_name.value;

    // A more complex selector parser would go here to handle pseudo-classes etc.
    // For now, we assume simple selectors.

    auto ruleNode = std::make_unique<CssRuleNode>(selector_str);

    expect(TokenType::OpenBrace);
    while (peek().type != TokenType::CloseBrace && peek().type != TokenType::EndOfFile) {
        skipComments();
        Token key = expect(TokenType::Identifier);
        expect(TokenType::Colon);
        std::string value_str;
        while(peek().type != TokenType::Semicolon && peek().type != TokenType::EndOfFile) {
            if (!value_str.empty()) value_str += " ";
            value_str += consume().value;
        }
        expect(TokenType::Semicolon);
        ruleNode->addProperty(std::make_unique<CssPropertyNode>(key.value, value_str));
    }
    expect(TokenType::CloseBrace);

    return ruleNode;
}


} // namespace CHTL
