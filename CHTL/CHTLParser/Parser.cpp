#include "Parser.h"
#include "../CHTLNode/TextNode.h" // Still need this for text: shorthand
#include "../CHTLNode/OriginUsageNode.h"
#include <stdexcept>
#include <fstream>
#include <iostream>
#include <sstream>

Parser::Parser(const std::vector<Token>& tokens, CHTLContext& context) : tokens(tokens), context(context) {}

// --- Main Parsing Method ---
std::vector<std::unique_ptr<BaseNode>> Parser::parse() {
    std::vector<std::unique_ptr<BaseNode>> topLevelNodes;
    while (!isAtEnd()) {
        if (peek().type == TokenType::KEYWORD_ORIGIN) {
            // Look ahead to see if it's a block or a usage
            if (tokens[current + 2].type == TokenType::LEFT_BRACE) {
                topLevelNodes.push_back(parseOriginBlock());
            } else {
                topLevelNodes.push_back(parseOriginUsage());
            }
        } else if (peek().type == TokenType::KEYWORD_IMPORT) {
            parseImportStatement(); // Imports modify the context, don't return a node
        } else if (peek().type == TokenType::IDENTIFIER) {
            topLevelNodes.push_back(parseElement());
        } else {
            advance(); // Skip unknown tokens at top level
        }
    }
    return topLevelNodes;
}

// --- Grammar Parsing Methods ---

std::unique_ptr<OriginNode> Parser::parseOriginBlock() {
    Token originToken = advance(); // consume the single KEYWORD_ORIGIN token
    // The lexeme holds the type (e.g., "Html") and the content holds the raw string
    return std::make_unique<OriginNode>(originToken.lexeme, originToken.content);
}

std::unique_ptr<OriginUsageNode> Parser::parseOriginUsage() {
    advance(); // consume [Origin]
    if (!match(TokenType::AT)) throw std::runtime_error("Expected '@' after [Origin]");

    Token typeToken = advance();
    if (typeToken.type != TokenType::IDENTIFIER) throw std::runtime_error("Expected origin type identifier");

    Token nameToken = advance();
    if (nameToken.type != TokenType::IDENTIFIER) throw std::runtime_error("Expected name identifier for origin usage");

    match(TokenType::SEMICOLON);
    return std::make_unique<OriginUsageNode>(typeToken.lexeme, nameToken.lexeme);
}

void Parser::parseImportStatement() {
    advance(); // consume [Import]
    if (!match(TokenType::AT)) throw std::runtime_error("Expected '@' after [Import]");

    Token typeToken = advance();
    if (typeToken.type != TokenType::IDENTIFIER) throw std::runtime_error("Expected import type identifier");

    if (!match(TokenType::KEYWORD_FROM)) throw std::runtime_error("Expected 'from' in import statement");

    Token pathToken = advance();
    if (pathToken.type != TokenType::STRING) throw std::runtime_error("Expected string literal for import path");
    std::string path = pathToken.lexeme.substr(1, pathToken.lexeme.length() - 2); // remove quotes

    if (!match(TokenType::KEYWORD_AS)) throw std::runtime_error("Expected 'as' in import statement");

    Token nameToken = advance();
    if (nameToken.type != TokenType::IDENTIFIER) throw std::runtime_error("Expected name identifier for import");

    // Read the file content
    std::ifstream file(path);
    if (!file.is_open()) throw std::runtime_error("Could not open imported file: " + path);
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();

    // Create an OriginNode and store it in the context
    auto originNode = std::make_unique<OriginNode>(typeToken.lexeme, content);
    context.namedOriginBlocks[nameToken.lexeme] = std::move(originNode);

    match(TokenType::SEMICOLON); // Optional semicolon
}


std::unique_ptr<ElementNode> Parser::parseElement() {
    Token tagToken = advance();
    auto element = std::make_unique<ElementNode>(tagToken.lexeme);

    if (!match(TokenType::LEFT_BRACE)) return element; // Allow elements without braces

    while (!isAtEnd() && !check(TokenType::RIGHT_BRACE)) {
        if (peek().type == TokenType::KEYWORD_ORIGIN) {
            if (tokens[current + 2].type == TokenType::LEFT_BRACE) {
                element->addChild(parseOriginBlock());
            } else {
                element->addChild(parseOriginUsage());
            }
        }
        else if (peek().type == TokenType::IDENTIFIER && peekNext().type == TokenType::COLON) {
            // Attribute or text shorthand
            Token key = advance();
            advance(); // consume ':'

            // Collect value tokens until semicolon
            std::string valueStr;
            while(!isAtEnd() && !check(TokenType::SEMICOLON)) {
                if (!valueStr.empty()) valueStr += " ";
                valueStr += advance().lexeme;
            }

            if (key.lexeme == "text") {
                element->addChild(std::make_unique<TextNode>(valueStr));
            } else {
                element->addAttribute(key.lexeme, valueStr);
            }
            match(TokenType::SEMICOLON);
        } else if (peek().type == TokenType::IDENTIFIER) {
            // Child element
            element->addChild(parseElement());
        } else {
            advance(); // Skip unknown tokens
        }
    }

    match(TokenType::RIGHT_BRACE);
    return element;
}


// --- Helper Methods ---
bool Parser::isAtEnd() { return peek().type == TokenType::END_OF_FILE; }
Token Parser::peek() { return tokens[current]; }
Token Parser::advance() { if (!isAtEnd()) current++; return tokens[current - 1]; }
bool Parser::match(TokenType type) {
    if (isAtEnd() || peek().type != type) return false;
    advance();
    return true;
}
bool Parser::check(TokenType type) {
    if (isAtEnd()) return false;
    return peek().type == type;
}
Token Parser::peekNext() {
    if (current + 1 >= tokens.size()) return tokens.back();
    return tokens[current+1];
}
