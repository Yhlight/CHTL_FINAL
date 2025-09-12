#include "CHTLParser.h"
#include <iostream>
#include <sstream>

namespace CHTL {

CHTLParser::CHTLParser(const std::vector<Token>& tokens) : tokens(tokens) {}

std::shared_ptr<ElementNode> CHTLParser::parse() {
    auto root = std::make_shared<ElementNode>("root");
    while (!isAtEnd()) {
        root->addChild(declaration());
    }
    return root;
}

std::shared_ptr<BaseNode> CHTLParser::declaration() {
    if (check(TokenType::KEYWORD_TEXT)) {
        // Lookahead to see if it's text: "..." or text { ... }
        if (tokens[current + 1].type == TokenType::LEFT_BRACE) {
            return textElement();
        }
    }
    // Default to parsing an element
    return element();
}

std::shared_ptr<ElementNode> CHTLParser::element() {
    Token identifier = consume(TokenType::IDENTIFIER, "Expect element name.");
    auto node = std::make_shared<ElementNode>(identifier.lexeme);
    consume(TokenType::LEFT_BRACE, "Expect '{' after element name.");

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        Token currentToken = peek();

        if (currentToken.type == TokenType::KEYWORD_STYLE) {
            node->styles = parseStyleBlock();
        } else if (currentToken.type == TokenType::KEYWORD_TEXT) {
            // text: "..." ;
             advance(); // consume 'text'
             consume(TokenType::COLON, "Expect ':' after 'text' attribute.");
             Token value = consume(TokenType::STRING, "Expect string value for 'text' attribute.");
             consume(TokenType::SEMICOLON, "Expect ';' after text value.");
             node->addChild(std::make_shared<TextNode>(value.lexeme));

        } else if (currentToken.type == TokenType::IDENTIFIER) {
            // Lookahead to see if it's an attribute or a nested element.
            if (tokens[current + 1].type == TokenType::COLON || tokens[current + 1].type == TokenType::EQUAL) {
                // Attribute
                Token key = advance();
                match({TokenType::COLON, TokenType::EQUAL});
                Token value = consume(TokenType::STRING, "Expect attribute value.");
                consume(TokenType::SEMICOLON, "Expect ';' after attribute value.");
                node->addAttribute(std::make_shared<AttributeNode>(key.lexeme, value.lexeme));
            } else {
                // Nested Element
                node->addChild(element());
            }
        } else {
            throw error(peek(), "Unexpected token inside element block.");
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after element block.");
    return node;
}

std::shared_ptr<TextNode> CHTLParser::textElement() {
    consume(TokenType::KEYWORD_TEXT, "Expect 'text' keyword.");
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'text'.");
    Token content = consume(TokenType::STRING, "Expect string literal inside text block.");
    consume(TokenType::RIGHT_BRACE, "Expect '}' after text block.");
    return std::make_shared<TextNode>(content.lexeme);
}

std::map<std::string, std::string> CHTLParser::parseStyleBlock() {
    consume(TokenType::KEYWORD_STYLE, "Expect 'style' keyword.");
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'style'.");

    std::map<std::string, std::string> styles;

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        Token key = consume(TokenType::IDENTIFIER, "Expect style property name.");
        consume(TokenType::COLON, "Expect ':' after property name.");

        std::stringstream value_ss;
        while (!check(TokenType::SEMICOLON) && !isAtEnd()) {
            if (!value_ss.str().empty()) {
                value_ss << " ";
            }
            value_ss << advance().lexeme;
        }

        consume(TokenType::SEMICOLON, "Expect ';' after property value.");

        styles[key.lexeme] = value_ss.str();
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after style block.");
    return styles;
}


// --- Utility and Error Handling Methods ---

bool CHTLParser::isAtEnd() {
    return peek().type == TokenType::END_OF_FILE;
}

Token CHTLParser::peek() {
    return tokens[current];
}

Token CHTLParser::previous() {
    return tokens[current - 1];
}

Token CHTLParser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

bool CHTLParser::check(TokenType type) {
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

Token CHTLParser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw error(peek(), message);
}

CHTLParser::ParseError CHTLParser::error(const Token& token, const std::string& message) {
    // In a real compiler, you might have a more sophisticated error reporter.
    std::cerr << "Parse Error at line " << token.line << " near '" << token.lexeme << "': " << message << std::endl;
    return ParseError(message);
}

} // namespace CHTL
