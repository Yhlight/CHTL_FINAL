#pragma once

#include "../CHTLLexer/Lexer.h"
#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <vector>
#include <memory>
#include <stdexcept>

namespace CHTL {

class Parser {
public:
    Parser(Lexer& lexer) : l(lexer), currentToken({TokenType::ILLEGAL, ""}), peekToken({TokenType::ILLEGAL, ""}) {
        // Prime the token stream by calling nextToken twice
        nextToken();
        nextToken();
    }

    // Main entry point for parsing. Returns a list of top-level nodes.
    std::vector<std::unique_ptr<BaseNode>> ParseProgram() {
        std::vector<std::unique_ptr<BaseNode>> statements;
        while (currentToken.type != TokenType::END_OF_FILE) {
            auto stmt = parseStatement();
            if (stmt) {
                statements.push_back(std::move(stmt));
            }
        }
        return statements;
    }

private:
    void nextToken() {
        currentToken = peekToken;
        peekToken = l.NextToken();
    }

    // Dispatches to the correct parsing function based on the current token.
    std::unique_ptr<BaseNode> parseStatement() {
        if (currentToken.type == TokenType::IDENTIFIER) {
            if (currentToken.value == "text") {
                return parseTextNode();
            } else {
                // Any other identifier is assumed to be an element tag.
                return parseElementNode();
            }
        }
        // If we encounter a token we don't know how to handle, we advance and return null.
        nextToken();
        return nullptr;
    }

    // Parses an element, e.g., `div { ... }`
    std::unique_ptr<ElementNode> parseElementNode() {
        auto node = std::make_unique<ElementNode>(currentToken.value);

        if (peekToken.type != TokenType::LBRACE) {
            throw std::runtime_error("Syntax Error: Expected '{' after element tag '" + currentToken.value + "'");
        }
        nextToken(); // Consume identifier, currentToken is now '{'

        nextToken(); // Consume '{', currentToken is now the first token of the body

        while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
            auto child = parseStatement();
            if (child) {
                node->children.push_back(std::move(child));
            }
        }

        if (currentToken.type != TokenType::RBRACE) {
            throw std::runtime_error("Syntax Error: Expected '}' to close element '" + node->tagName + "'");
        }

        nextToken(); // Consume '}'
        return node;
    }

    // Parses a text block, e.g., `text { "some text" }` or `text { some text }`
    std::unique_ptr<TextNode> parseTextNode() {
        // currentToken is 'text'
        if (peekToken.type != TokenType::LBRACE) {
            throw std::runtime_error("Syntax Error: Expected '{' after 'text' keyword");
        }
        nextToken(); // Consume 'text', currentToken is now '{'
        nextToken(); // Consume '{', currentToken is now the content

        std::string textValue;
        if (currentToken.type == TokenType::STRING) {
            textValue = currentToken.value;
            nextToken(); // Consume the string token
        } else {
            // Handle unquoted literal by concatenating identifiers until '}'
            std::string unquoted_literal;
            while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
                if (!unquoted_literal.empty()) {
                    unquoted_literal += " ";
                }
                unquoted_literal += currentToken.value;
                nextToken();
            }
            textValue = unquoted_literal;
        }

        if (currentToken.type != TokenType::RBRACE) {
            throw std::runtime_error("Syntax Error: Expected '}' to close text block");
        }

        nextToken(); // Consume '}'
        return std::make_unique<TextNode>(textValue);
    }

    Lexer& l;
    Token currentToken;
    Token peekToken;
};

} // namespace CHTL
