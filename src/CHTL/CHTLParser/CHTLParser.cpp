#include "CHTLParser.h"
#include <stdexcept>

CHTLParser::CHTLParser(const std::vector<Token>& tokens) : tokens(tokens) {}

// ... token management methods ...
const Token& CHTLParser::peek() const {
    return tokens[current];
}

const Token& CHTLParser::previous() const {
    return tokens[current - 1];
}

bool CHTLParser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
}

const Token& CHTLParser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return previous();
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
    if (check(type)) {
        return advance();
    }
    throw std::runtime_error(message + " at line " + std::to_string(peek().line) + ":" + std::to_string(peek().column));
}


std::unique_ptr<ElementNode> CHTLParser::parse() {
    if (peek().type == TokenType::Identifier) {
        return parseElement();
    }
    throw std::runtime_error("CHTML document must start with a root element.");
}

void CHTLParser::parseBlock(ElementNode* element) {
    consume(TokenType::LeftBrace, "Expect '{' after element name.");

    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        if (peek().type == TokenType::Identifier && tokens[current + 1].type == TokenType::Colon) {
            parseAttribute(element);
        } else if (peek().type == TokenType::StringLiteral) {
            Token textToken = advance();
            element->addChild(std::make_unique<TextNode>(textToken.lexeme));
        }
        else {
            auto statement = parseStatement();
            if(statement) {
                 element->addChild(std::move(statement));
            }
        }
    }

    consume(TokenType::RightBrace, "Expect '}' after block.");
}

void CHTLParser::parseAttribute(ElementNode* element) {
    const Token& key = consume(TokenType::Identifier, "Expect attribute name.");
    consume(TokenType::Colon, "Expect ':' after attribute name.");

    std::string value;
    if (match({TokenType::Identifier, TokenType::UnquotedLiteral, TokenType::StringLiteral})) {
        value = previous().lexeme;
    } else {
        throw std::runtime_error("Unsupported attribute value type.");
    }

    consume(TokenType::Semicolon, "Expect ';' after attribute value.");
    element->addAttribute({key.lexeme, value});
}

std::unique_ptr<BaseNode> CHTLParser::parseStatement() {
    if (peek().type == TokenType::Text) {
        return parseText();
    }
    if (peek().type == TokenType::Identifier) {
        return parseElement();
    }
    advance();
    return nullptr;
}

std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    const Token& nameToken = consume(TokenType::Identifier, "Expect element name.");
    auto element = std::make_unique<ElementNode>(nameToken.lexeme);
    parseBlock(element.get());
    return element;
}

std::unique_ptr<TextNode> CHTLParser::parseText() {
    consume(TokenType::Text, "Expect 'text' keyword.");
    consume(TokenType::LeftBrace, "Expect '{' after 'text'.");

    std::string textContent;
    while (!check(TokenType::RightBrace) && !isAtEnd()) {
        if (peek().type == TokenType::Identifier || peek().type == TokenType::UnquotedLiteral) {
            if (!textContent.empty()) textContent += " ";
            textContent += advance().lexeme;
        } else if (peek().type == TokenType::StringLiteral) {
            if (!textContent.empty()) textContent += " ";
            textContent += advance().lexeme;
        }
        else {
            break;
        }
    }

    consume(TokenType::RightBrace, "Expect '}' after text block.");
    return std::make_unique<TextNode>(textContent);
}
