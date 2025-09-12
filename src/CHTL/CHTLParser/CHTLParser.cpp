#include "CHTLParser.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/CommentNode.h"
#include <stdexcept>

namespace CHTL {

CHTLParser::CHTLParser(std::vector<Token>&& tokens, CHTLContext& context)
    : tokens(std::move(tokens)), context(context) {}

std::unique_ptr<BaseNode> CHTLParser::parse() {
    if (isAtEnd()) return nullptr;
    return parseElement();
}

std::unique_ptr<ElementNode> CHTLParser::parseElement() {
    Token tagToken = consume(TokenType::UNQUOTED_LITERAL, "Expect element tag name.");
    auto element = std::make_unique<ElementNode>(tagToken.lexeme);

    consume(TokenType::LEFT_BRACE, "Expect '{' after element tag name.");
    parseBlock(element.get());
    consume(TokenType::RIGHT_BRACE, "Expect '}' after element block.");

    return element;
}

void CHTLParser::parseBlock(ElementNode* element) {
    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (peek().type == TokenType::CHTL_COMMENT) {
            Token commentToken = advance();
            element->addChild(std::make_unique<CommentNode>(commentToken.lexeme));
        } else if (peek().type == TokenType::KEYWORD_STYLE) {
            parseStyleBlock(element);
        }
        else if (peek().type == TokenType::UNQUOTED_LITERAL || peek().type == TokenType::KEYWORD_TEXT) {
            if (peekNext().type == TokenType::COLON) {
                parseProperty(element);
            } else {
                element->addChild(parseElement());
            }
        } else {
            element->addChild(parseElement());
        }
    }
}

void CHTLParser::parseProperty(ElementNode* element) {
    Token key = advance();

    consume(TokenType::COLON, "Expect ':' after property key.");

    Token value = advance();
    if (value.type != TokenType::STRING && value.type != TokenType::UNQUOTED_LITERAL && value.type != TokenType::NUMBER) {
        throw std::runtime_error("Property value must be a string, number, or unquoted literal.");
    }

    consume(TokenType::SEMICOLON, "Expect ';' after property value.");

    if (key.type == TokenType::KEYWORD_TEXT) {
        element->addChild(std::make_unique<TextNode>(value.lexeme));
    } else {
        element->addAttribute(key.lexeme, value.lexeme);
    }
}

void CHTLParser::parseStyleBlock(ElementNode* element) {
    consume(TokenType::KEYWORD_STYLE, "Expect 'style' keyword.");
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'style'.");

    while (!check(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        if (peek().type == TokenType::CHTL_COMMENT) {
            Token commentToken = advance();
            element->addChild(std::make_unique<CommentNode>(commentToken.lexeme));
        } else if (peek().type == TokenType::DOT) { // Class selector
            consume(TokenType::DOT, "Expect '.' for class selector.");
            Token name = consume(TokenType::UNQUOTED_LITERAL, "Expect class name.");
            element->addAttribute("class", name.lexeme);
            element->setPrimarySelector("." + name.lexeme);

            consume(TokenType::LEFT_BRACE, "Expect '{' for style rule block.");
            std::string ruleBody = parseCssBlock();
            context.addCssRule("." + name.lexeme, ruleBody);
            consume(TokenType::RIGHT_BRACE, "Expect '}' for style rule block.");

        } else if (peek().type == TokenType::HASH) { // ID selector
            consume(TokenType::HASH, "Expect '#' for id selector.");
            Token name = consume(TokenType::UNQUOTED_LITERAL, "Expect id name.");
            element->addAttribute("id", name.lexeme);
            element->setPrimarySelector("#" + name.lexeme);

            consume(TokenType::LEFT_BRACE, "Expect '{' for style rule block.");
            std::string ruleBody = parseCssBlock();
            context.addCssRule("#" + name.lexeme, ruleBody);
            consume(TokenType::RIGHT_BRACE, "Expect '}' for style rule block.");
        } else if (peek().type == TokenType::AMPERSAND) { // Context selector
            consume(TokenType::AMPERSAND, "Expect '&'.");
            // The rest of the selector (e.g., :hover) is tokenized as other things.
            // We'll consume until the `{`. This is a simplification.
            std::string restOfSelector = "";
            while(peek().type != TokenType::LEFT_BRACE && !isAtEnd()) {
                restOfSelector += advance().lexeme;
            }
            std::string fullSelector = element->getPrimarySelector() + restOfSelector;

            consume(TokenType::LEFT_BRACE, "Expect '{' for style rule block.");
            std::string ruleBody = parseCssBlock();
            context.addCssRule(fullSelector, ruleBody);
            consume(TokenType::RIGHT_BRACE, "Expect '}' for style rule block.");
        }
        else { // Inline style property
            Token key = consume(TokenType::UNQUOTED_LITERAL, "Expect style property name.");
            consume(TokenType::COLON, "Expect ':' after style property name.");

            Token value = advance();
            if (value.type != TokenType::STRING && value.type != TokenType::UNQUOTED_LITERAL && value.type != TokenType::NUMBER) {
                throw std::runtime_error("Style value must be a string, number, or unquoted literal.");
            }

            element->addInlineStyle(key.lexeme, value.lexeme);

            consume(TokenType::SEMICOLON, "Expect ';' after style value.");
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after style block.");
}

std::string CHTLParser::parseCssBlock() {
    // This is a simple approach that captures the raw string content inside the braces.
    // It assumes the lexer doesn't tokenize anything inside the CSS block, which is not
    // currently true. A better approach is needed.
    // For now, let's just consume tokens until the matching brace.
    int start_pos = current;
    int brace_level = 1;
    while (brace_level > 0 && !isAtEnd()) {
        if (peek().type == TokenType::LEFT_BRACE) brace_level++;
        else if (peek().type == TokenType::RIGHT_BRACE) brace_level--;

        if (brace_level == 0) break; // found matching brace
        advance();
    }
    int end_pos = current;

    // This part is tricky because the lexemes don't contain whitespace.
    // Reconstructing from source is better.
    // Let's find the character position of the start and end tokens.
    // This requires more info on the Token or a different approach.

    // Let's stick to the simple token-by-token concatenation for now, it's a known limitation.
    std::string body = "";
    for (int i = start_pos; i < end_pos; ++i) {
        body += tokens[i].lexeme;
        if (tokens[i].type == TokenType::COLON || tokens[i].type == TokenType::SEMICOLON) {
            body += " ";
        }
    }
    return body;
}

// --- Parser Helpers ---

bool CHTLParser::isAtEnd() const {
    return peek().type == TokenType::END_OF_FILE;
}

Token CHTLParser::peek() const {
    if (current >= tokens.size()) return tokens.back(); // EOF
    return tokens[current];
}

Token CHTLParser::peekNext() const {
    if (current + 1 >= tokens.size()) return tokens.back(); // EOF
    return tokens[current + 1];
}

Token CHTLParser::advance() {
    if (!isAtEnd()) {
        current++;
    }
    return tokens[current - 1];
}

bool CHTLParser::check(TokenType type) const {
    if (isAtEnd()) return type == TokenType::END_OF_FILE;
    return peek().type == type;
}

Token CHTLParser::consume(TokenType type, const std::string& message) {
    if (check(type)) return advance();
    throw std::runtime_error(message + " Got " + peek().toString());
}

} // namespace CHTL
