#include "CHTLParser.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/CommentNode.h"
#include "CHTL/CHTLNode/BinaryExpr.h"
#include "CHTL/CHTLNode/NumberLiteralExpr.h"
#include "CHTL/CHTLNode/GroupingExpr.h"
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
        } else if (peek().type == TokenType::DOT) {
            consume(TokenType::DOT, "Expect '.' for class selector.");
            Token name = consume(TokenType::UNQUOTED_LITERAL, "Expect class name.");
            element->addAttribute("class", name.lexeme);
            element->setPrimarySelector("." + name.lexeme);

            consume(TokenType::LEFT_BRACE, "Expect '{' for style rule block.");
            std::string ruleBody = parseCssBlock();
            context.addCssRule("." + name.lexeme, ruleBody);
            consume(TokenType::RIGHT_BRACE, "Expect '}' for style rule block.");

        } else if (peek().type == TokenType::HASH) {
            consume(TokenType::HASH, "Expect '#' for id selector.");
            Token name = consume(TokenType::UNQUOTED_LITERAL, "Expect id name.");
            element->addAttribute("id", name.lexeme);
            element->setPrimarySelector("#" + name.lexeme);

            consume(TokenType::LEFT_BRACE, "Expect '{' for style rule block.");
            std::string ruleBody = parseCssBlock();
            context.addCssRule("#" + name.lexeme, ruleBody);
            consume(TokenType::RIGHT_BRACE, "Expect '}' for style rule block.");
        } else if (peek().type == TokenType::AMPERSAND) {
            consume(TokenType::AMPERSAND, "Expect '&'.");
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

            element->addInlineStyle(key.lexeme, parseExpression());

            consume(TokenType::SEMICOLON, "Expect ';' after style value.");
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after style block.");
}

std::string CHTLParser::parseCssBlock() {
    int start_pos = current;
    int brace_level = 1;
    while (brace_level > 0 && !isAtEnd()) {
        if (peek().type == TokenType::LEFT_BRACE) brace_level++;
        else if (peek().type == TokenType::RIGHT_BRACE) brace_level--;

        if (brace_level == 0) break;
        advance();
    }
    int end_pos = current;

    std::string body = "";
    for (int i = start_pos; i < end_pos; ++i) {
        body += tokens[i].lexeme;
        if (tokens[i].type == TokenType::COLON || tokens[i].type == TokenType::SEMICOLON) {
            body += " ";
        }
    }
    return body;
}

// --- Expression Parsing (Recursive Descent) ---

// expression -> term
std::unique_ptr<ExprNode> CHTLParser::parseExpression() {
    return parseTerm();
}

// term -> factor ( ( "-" | "+" ) factor )*
std::unique_ptr<ExprNode> CHTLParser::parseTerm() {
    auto expr = parseFactor();

    while (peek().type == TokenType::PLUS || peek().type == TokenType::MINUS) {
        Token op = advance();
        auto right = parseFactor();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// factor -> primary ( ( "/" | "*" ) primary )*
std::unique_ptr<ExprNode> CHTLParser::parseFactor() {
    auto expr = parsePrimary();

    while (peek().type == TokenType::STAR || peek().type == TokenType::SLASH) {
        Token op = advance();
        auto right = parsePrimary();
        expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(right));
    }

    return expr;
}

// primary -> NUMBER | "(" expression ")"
std::unique_ptr<ExprNode> CHTLParser::parsePrimary() {
    if (peek().type == TokenType::NUMBER) {
        return std::make_unique<NumberLiteralExpr>(advance().lexeme);
    }

    if (peek().type == TokenType::LEFT_PAREN) {
        advance(); // consume '('
        auto expr = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return std::make_unique<GroupingExpr>(std::move(expr));
    }

    throw std::runtime_error("Expect expression, but got " + peek().toString());
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
