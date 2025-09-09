#include "Parser.h"
#include <stdexcept>

namespace CHTL {

Parser::Parser(std::vector<Token> tokens)
    : m_tokens(std::move(tokens)), m_position(0) {}

const Token& Parser::peek(size_t offset) const {
    if (m_position + offset >= m_tokens.size()) {
        return m_tokens.back();
    }
    return m_tokens[m_position + offset];
}

Token Parser::consume() {
    if (m_position >= m_tokens.size()) {
        return m_tokens.back();
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

        if (current_token.value == "text" && next_token.type == TokenType::Colon) {
            consume(); // text
            consume(); // :
            const auto& value_token = consume();
            if (value_token.type != TokenType::Identifier && value_token.type != TokenType::StringLiteral) {
                throw std::runtime_error("Expected a string or literal for text attribute at line " + std::to_string(value_token.line));
            }
            element->addChild(std::make_unique<TextNode>(value_token.value));
            expect(TokenType::Semicolon);
        } else if (next_token.type == TokenType::Colon || next_token.type == TokenType::Equals) {
            Token key = consume();
            consume(); // : or =
            Token value = expect(TokenType::Identifier);
            element->addAttribute(std::make_unique<AttributeNode>(key.value, value.value));
            expect(TokenType::Semicolon);
        } else if (current_token.value == "style" && next_token.type == TokenType::OpenBrace) {
            consume(); // 'style'
            expect(TokenType::OpenBrace);
            element->setStyleBlock(parseStyleBlock());
            expect(TokenType::CloseBrace);
        } else if (current_token.value == "text" && next_token.type == TokenType::OpenBrace) {
            consume(); // text identifier
            expect(TokenType::OpenBrace);
            skipComments();
            std::string text_value;
            while(peek().type != TokenType::CloseBrace && peek().type != TokenType::EndOfFile) {
                if (!text_value.empty()) {
                    text_value += " ";
                }
                text_value += consume().value;
            }
            element->addChild(std::make_unique<TextNode>(text_value));
            expect(TokenType::CloseBrace);
        } else if (next_token.type == TokenType::OpenBrace || next_token.type == TokenType::Semicolon) {
             element->addChild(parseElement(consume()));
             if (peek().type == TokenType::Semicolon) {
                 consume();
             }
        } else {
            throw std::runtime_error("Invalid syntax inside element body at line " + std::to_string(current_token.line));
        }
    }
}

std::unique_ptr<CssPropertyNode> Parser::parseCssProperty() {
    Token key = expect(TokenType::Identifier);
    expect(TokenType::Colon);
    auto value = parseExpression();
    expect(TokenType::Semicolon);
    return std::make_unique<CssPropertyNode>(key.value, std::move(value));
}

std::unique_ptr<StyleBlockNode> Parser::parseStyleBlock() {
    auto styleNode = std::make_unique<StyleBlockNode>();
    while (peek().type != TokenType::CloseBrace && peek().type != TokenType::EndOfFile) {
        skipComments();
        const auto& current = peek();
        const auto& next = peek(1);

        if (current.type == TokenType::Identifier && next.type == TokenType::Colon) {
            styleNode->addInlineProperty(parseCssProperty());
        }
        else {
            styleNode->addCssRule(parseCssRule());
        }
    }
    return styleNode;
}

std::unique_ptr<CssRuleNode> Parser::parseCssRule() {
    skipComments();
    std::string selector_str;

    while (peek().type != TokenType::OpenBrace && peek().type != TokenType::EndOfFile) {
        selector_str += consume().value;
    }

    if (selector_str.empty()) {
        throw std::runtime_error("Found CSS rule with empty selector at line " + std::to_string(peek().line));
    }

    auto ruleNode = std::make_unique<CssRuleNode>(selector_str);

    expect(TokenType::OpenBrace);
    while (peek().type != TokenType::CloseBrace && peek().type != TokenType::EndOfFile) {
        skipComments();
        ruleNode->addProperty(parseCssProperty());
    }
    expect(TokenType::CloseBrace);

    return ruleNode;
}

int Parser::getOperatorPrecedence(TokenType type) {
    switch (type) {
        case TokenType::QuestionMark:
            return 1;
        case TokenType::LogicalOr:
            return 2;
        case TokenType::LogicalAnd:
            return 3;
        case TokenType::GreaterThan:
        case TokenType::LessThan:
            return 4;
        default:
            return 0;
    }
}

std::unique_ptr<ExpressionNode> Parser::parsePrimaryExpression() {
    if (peek().type == TokenType::Identifier || peek().type == TokenType::StringLiteral) {
        return std::make_unique<LiteralNode>(consume().value);
    }

    if (match(TokenType::OpenParen)) {
        auto expr = parseExpression();
        expect(TokenType::CloseParen);
        return expr;
    }

    throw std::runtime_error("Unexpected token in expression: " + peek().value);
}

std::unique_ptr<ExpressionNode> Parser::parseExpression(int precedence) {
    auto left = parsePrimaryExpression();

    while (precedence < getOperatorPrecedence(peek().type)) {
        Token op = consume();

        if (op.type == TokenType::QuestionMark) {
            auto true_expr = parseExpression();
            expect(TokenType::Colon);
            auto false_expr = parseExpression(getOperatorPrecedence(op.type) - 1);
            left = std::make_unique<TernaryOperationNode>(std::move(left), std::move(true_expr), std::move(false_expr));
        } else {
            auto right = parseExpression(getOperatorPrecedence(op.type));
            left = std::make_unique<BinaryOperationNode>(std::move(left), op, std::move(right));
        }
    }

    return left;
}

} // namespace CHTL
