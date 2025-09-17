#include "Parser.h"
#include <stdexcept>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::shared_ptr<RootNode> Parser::parse() {
    auto root = std::make_shared<RootNode>();
    while (!isAtEnd()) {
        root->addChild(parseStatement());
    }
    return root;
}

// Helper methods
Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

Token Parser::peek() const {
    return tokens[current];
}

Token Parser::previous() const {
    return tokens[current - 1];
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

bool Parser::check(TokenType type) const {
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
    throw std::runtime_error(message + " at line " + std::to_string(peek().line) + " but got " + tokenTypeToString(peek().type));
}


// Parsing methods
std::shared_ptr<BaseNode> Parser::parseStatement() {
    if (peek().type == TokenType::IDENTIFIER && tokens[current + 1].type == TokenType::LBRACE) {
        return parseElement();
    }
    if (peek().type == TokenType::KEYWORD_TEXT && tokens[current + 1].type == TokenType::LBRACE) {
        return parseTextObject();
    }

    throw std::runtime_error("Unexpected token at line " + std::to_string(peek().line) + ". Expected element or text block.");
}

std::shared_ptr<ElementNode> Parser::parseElement() {
    Token tagName = consume(TokenType::IDENTIFIER, "Expected element name.");
    auto element = std::make_shared<ElementNode>(tagName.value);

    consume(TokenType::LBRACE, "Expected '{' after element name.");
    parseBlock(element);
    consume(TokenType::RBRACE, "Expected '}' to close element block.");

    return element;
}

void Parser::parseBlock(std::shared_ptr<ElementNode> element) {
    while (!check(TokenType::RBRACE) && !isAtEnd()) {
        // Is it an attribute? (IDENTIFIER or KEYWORD_TEXT followed by : or =)
        if ((check(TokenType::IDENTIFIER) || check(TokenType::KEYWORD_TEXT)) && (tokens[current + 1].type == TokenType::COLON || tokens[current + 1].type == TokenType::EQUALS)) {
            parseAttribute(element);
        }
        // Is it a nested text block?
        else if (check(TokenType::KEYWORD_TEXT)) {
            element->addChild(parseTextObject());
        }
        // Is it a nested element?
        else if (check(TokenType::IDENTIFIER)) {
            element->addChild(parseElement());
        }
        else {
            throw std::runtime_error("Unexpected token in block at line " + std::to_string(peek().line) + ": " + peek().value);
        }
    }
}

void Parser::parseAttribute(std::shared_ptr<ElementNode> element) {
    Token key = advance();
    if (key.type != TokenType::IDENTIFIER && key.type != TokenType::KEYWORD_TEXT) {
        throw std::runtime_error("Expected attribute name or 'text' keyword at line " + std::to_string(key.line));
    }

    // Correctly check for and consume ':' or '='
    if (!match({TokenType::COLON, TokenType::EQUALS})) {
        throw std::runtime_error("Expected ':' or '=' after attribute name at line " + std::to_string(peek().line));
    }

    // The value can be a string, an identifier (unquoted literal), or a number
    Token valueToken = advance();
    std::string value;

    if(valueToken.type == TokenType::STRING_LITERAL || valueToken.type == TokenType::IDENTIFIER || valueToken.type == TokenType::UNQUOTED_LITERAL || valueToken.type == TokenType::NUMBER_LITERAL) {
        value = valueToken.value;
    } else {
        throw std::runtime_error("Expected attribute value at line " + std::to_string(valueToken.line));
    }

    // If the key was 'text', this is a shorthand for a text node child.
    if (key.type == TokenType::KEYWORD_TEXT) {
        element->addChild(std::make_shared<TextNode>(value));
    } else {
        element->addAttribute(std::make_shared<AttributeNode>(key.value, value));
    }

    consume(TokenType::SEMICOLON, "Expected ';' after attribute value.");
}

std::shared_ptr<TextNode> Parser::parseTextObject() {
    consume(TokenType::KEYWORD_TEXT, "Expected 'text' keyword.");
    consume(TokenType::LBRACE, "Expected '{' after 'text' keyword.");

    Token value = advance();
    if (value.type != TokenType::STRING_LITERAL && value.type != TokenType::UNQUOTED_LITERAL && value.type != TokenType::IDENTIFIER) {
         throw std::runtime_error("Expected a string or unquoted literal inside text block at line " + std::to_string(value.line));
    }

    consume(TokenType::RBRACE, "Expected '}' to close text block.");
    return std::make_shared<TextNode>(value.value);
}
