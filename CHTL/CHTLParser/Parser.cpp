#include "Parser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"
#include <iostream>

Parser::Parser(const std::string& source, const std::vector<Token>& tokens) : source(source), tokens(tokens) {}

// The main entry point for parsing.
// A CHTL fragment can be a list of declarations, so we'll wrap them in a root node.
std::shared_ptr<BaseNode> Parser::parse() {
    auto root = std::make_shared<ElementNode>();
    root->tagName = "fragment_root"; // A virtual root for the fragment

    while (!isAtEnd()) {
        auto decl = declaration();
        if (decl) {
            root->children.push_back(decl);
        } else if (hadError) {
            // If declaration returned null due to an error, stop.
            return nullptr;
        }
    }
    return root;
}

std::shared_ptr<BaseNode> Parser::declaration() {
    if (peek().type == TokenType::TOKEN_IDENTIFIER && (tokens[current + 1].type == TokenType::TOKEN_LBRACE || tokens[current + 1].type == TokenType::TOKEN_COLON || tokens[current + 1].type == TokenType::TOKEN_EQUAL)) {
        // This is likely an element or a top-level attribute which is not valid CHTL.
        // For now, we assume it's an element.
        if (match({TokenType::TOKEN_IDENTIFIER})) {
             return element();
        }
    }
    if (match({TokenType::TOKEN_TEXT})) {
        return textBlock();
    }
    if (match({TokenType::TOKEN_STYLE})) {
        return styleBlock();
    }
    if (match({TokenType::TOKEN_GENERATOR_COMMENT})) {
        auto node = std::make_shared<CommentNode>();
        node->comment = previous().lexeme;
        return node;
    }

    // If we don't know what this is, advance and report error to avoid loops.
    if (!isAtEnd()) {
        std::cerr << "Parse Error: Unexpected token '" << peek().lexeme << "' of type " << tokenTypeToString(peek().type) << std::endl;
        hadError = true;
        advance();
    }
    return nullptr;
}

std::shared_ptr<BaseNode> Parser::element() {
    Token identifier = previous(); // Assumes IDENTIFIER was matched before calling
    auto node = std::make_shared<ElementNode>();
    node->tagName = identifier.lexeme;

    consume(TokenType::TOKEN_LBRACE, "Expect '{' after element name.");

    while (!check(TokenType::TOKEN_RBRACE) && !isAtEnd()) {
        // Is it an attribute? (IDENTIFIER followed by : or =)
        if (peek().type == TokenType::TOKEN_IDENTIFIER && (tokens[current + 1].type == TokenType::TOKEN_COLON || tokens[current + 1].type == TokenType::TOKEN_EQUAL)) {
            attributes(node);
        } else {
            // It must be a child node
            auto child = declaration();
            if (child) {
                node->children.push_back(child);
            }
        }
    }

    consume(TokenType::TOKEN_RBRACE, "Expect '}' after element block.");
    return node;
}

void Parser::attributes(std::shared_ptr<ElementNode> node) {
    Token name = advance(); // Consume attribute name

    // FIX: Match either colon or equals
    if (!match({TokenType::TOKEN_COLON, TokenType::TOKEN_EQUAL})) {
        std::cerr << "Parse Error on line " << peek().line << ": Expect ':' or '=' after attribute name." << std::endl;
        hadError = true;
        synchronize(); // Attempt to recover
        return;
    }

    if (match({TokenType::TOKEN_STRING_LITERAL, TokenType::TOKEN_IDENTIFIER})) {
        node->attributes[name.lexeme] = previous().lexeme;
    } else {
        std::cerr << "Parse Error on line " << peek().line << ": Expect attribute value." << std::endl;
        hadError = true;
    }

    if (!match({TokenType::TOKEN_SEMICOLON})) {
        std::cerr << "Parse Error on line " << peek().line << ": Expect ';' after attribute." << std::endl;
        hadError = true;
        synchronize(); // Attempt to recover
    }
}

std::shared_ptr<BaseNode> Parser::textBlock() {
    consume(TokenType::TOKEN_LBRACE, "Expect '{' after 'text' keyword.");

    auto node = std::make_shared<TextNode>();

    if (match({TokenType::TOKEN_STRING_LITERAL, TokenType::TOKEN_IDENTIFIER})) {
        node->text = previous().lexeme;
    } else {
        std::cerr << "Parse Error on line " << peek().line << ": Expect text content inside text block." << std::endl;
        hadError = true;
    }

    consume(TokenType::TOKEN_RBRACE, "Expect '}' after text block.");
    return node;
}

std::shared_ptr<BaseNode> Parser::styleBlock() {
    Token lbrace = consume(TokenType::TOKEN_LBRACE, "Expect '{' after 'style' keyword.");

    auto node = std::make_shared<StyleNode>();

    // Find the matching RBRACE to determine the end of the block
    int brace_count = 1;
    size_t content_end_token_idx = current;
    while(content_end_token_idx < tokens.size() && brace_count > 0) {
        if(tokens[content_end_token_idx].type == TokenType::TOKEN_LBRACE) brace_count++;
        if(tokens[content_end_token_idx].type == TokenType::TOKEN_RBRACE) brace_count--;
        if (brace_count > 0) content_end_token_idx++;
    }

    if (brace_count > 0) {
        std::cerr << "Parse Error on line " << peek().line << ": Unterminated style block." << std::endl;
        hadError = true;
        return nullptr;
    }

    // Capture the raw content using offsets from the original source string
    size_t content_start_offset = lbrace.offset + 1;
    size_t content_end_offset = tokens[content_end_token_idx].offset;

    if (content_end_offset > content_start_offset) {
        node->rawContent = source.substr(content_start_offset, content_end_offset - content_start_offset);
    }

    // Advance the parser state past the style block content
    current = content_end_token_idx;

    consume(TokenType::TOKEN_RBRACE, "Expect '}' after style block.");
    return node;
}

// --- Helper Methods ---

bool Parser::isAtEnd() {
    return peek().type == TokenType::TOKEN_EOF;
}

Token Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}

Token Parser::peek() {
    return tokens[current];
}

Token Parser::previous() {
    return tokens[current - 1];
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
    std::cerr << "Parse Error on line " << peek().line << ": " << message << std::endl;
    hadError = true;
    return peek(); // Return the bogus token but don't throw
}

// Basic error recovery: advance until we find something that can start a new declaration.
void Parser::synchronize() {
    advance();
    while (!isAtEnd()) {
        if (previous().type == TokenType::TOKEN_SEMICOLON) return; // Previous declaration likely ended.
        switch (peek().type) {
            case TokenType::TOKEN_TEXT:
            case TokenType::TOKEN_IDENTIFIER: // Could be a new element
                return;
            default:
                break;
        }
        advance();
    }
}
