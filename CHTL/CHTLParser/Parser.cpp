#include "Parser.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/PropertyNode.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

NodeList Parser::parse() {
    NodeList nodes;
    while (!isAtEnd()) {
        skipComments();
        if (isAtEnd()) break;
        nodes.push_back(parseDeclaration());
    }
    return nodes;
}

NodePtr Parser::parseDeclaration() {
    // For now, we only parse element nodes at the top level.
    // This will be expanded to handle templates, imports, etc.
    if (peek().type == TokenType::Identifier) {
        return parseElement();
    }
    throw error(peek(), "Expect a declaration (e.g., an element).");
}

NodePtr Parser::parseElement() {
    const Token& nameToken = consume(TokenType::Identifier, "Expect element name.");
    auto element = std::make_unique<ElementNode>(nameToken.value);

    consume(TokenType::OpenBrace, "Expect '{' after element name.");

    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        skipComments();
        if (check(TokenType::CloseBrace)) break;

        // Inside an element, we can have properties or other nodes.
        if (peek().type == TokenType::Identifier) {
            // Look ahead to see if it's a property (identifier: value)
            // or a nested element (identifier { ... })
            if (tokens[current + 1].type == TokenType::Colon || tokens[current + 1].type == TokenType::Equals) {
                element->addChild(parseProperty());
            } else if (tokens[current + 1].type == TokenType::OpenBrace) {
                 if (peek().value == "text") {
                    element->addChild(parseTextNode());
                } else {
                    // For now, we assume any other identifier followed by a brace is an element.
                    // This will be expanded for style, script, etc.
                    element->addChild(parseElement());
                }
            } else {
                // This could be an unquoted literal inside a text block, but we are not handling that yet.
                // For now, it's an error.
                throw error(peek(), "Unexpected token inside element.");
            }
        } else {
            throw error(peek(), "Unexpected token inside element.");
        }
    }

    consume(TokenType::CloseBrace, "Expect '}' after element block.");
    return element;
}

NodePtr Parser::parseProperty() {
    const Token& nameToken = consume(TokenType::Identifier, "Expect property name.");

    if (!match({TokenType::Colon, TokenType::Equals})) {
        throw error(peek(), "Expect ':' or '=' after property name.");
    }

    // For now, we handle simple literal values.
    // This will need to be expanded for expressions.
    const Token& valueToken = advance(); // Consume the value token.
    if (valueToken.type != TokenType::Identifier && valueToken.type != TokenType::StringLiteral) {
        throw error(valueToken, "Property value must be an identifier or a string literal.");
    }

    consume(TokenType::Semicolon, "Expect ';' after property value.");

    return std::make_unique<PropertyNode>(nameToken.value, valueToken.value);
}

NodePtr Parser::parseTextNode() {
    consume(TokenType::Identifier, "Expect 'text' keyword.");
    consume(TokenType::OpenBrace, "Expect '{' after 'text' keyword.");

    std::string textContent;
    // CHTL allows `text { "string" }` or `text { unquoted text }`
    if (peek().type == TokenType::StringLiteral) {
        textContent = consume(TokenType::StringLiteral, "Expect string literal in text block.").value;
    } else {
        // Handle unquoted text. The lexer splits it into multiple identifier tokens.
        // We'll concatenate them until we see the closing brace.
        while (!check(TokenType::CloseBrace) && !isAtEnd()) {
            if (!textContent.empty()) {
                textContent += " ";
            }
            textContent += consume(TokenType::Identifier, "Expect unquoted text.").value;
        }
    }

    consume(TokenType::CloseBrace, "Expect '}' after text block.");
    return std::make_unique<TextNode>(textContent);
}


NodePtr Parser::parseComment(const Token& token) {
    bool isGenerator = token.type == TokenType::GeneratorComment;
    // The lexer gives us the full comment line/block, so we just store it.
    return std::make_unique<CommentNode>(token.value, isGenerator);
}


// --- Helper Methods ---

void Parser::skipComments() {
    while (peek().type == TokenType::SingleLineComment ||
           peek().type == TokenType::MultiLineComment ||
           peek().type == TokenType::GeneratorComment) {
        // For now, the parser ignores comments. The generator might need them later.
        advance();
    }
}

const Token& Parser::peek() const {
    return tokens[current];
}

const Token& Parser::previous() const {
    return tokens[current - 1];
}

const Token& Parser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return previous();
}

bool Parser::isAtEnd() const {
    return peek().type == TokenType::EndOfFile;
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

const Token& Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        return advance();
    }
    throw error(peek(), message);
}

Parser::ParseError Parser::error(const Token& token, const std::string& message) {
    std::string error_message = "Parse Error at line " + std::to_string(token.line) +
                               " column " + std::to_string(token.column) + ": " + message;
    if (token.type == TokenType::EndOfFile) {
        error_message = "Parse Error at end of file: " + message;
    } else {
         error_message = "Parse Error at '" + token.value + "': " + message;
    }
    return ParseError(error_message);
}
