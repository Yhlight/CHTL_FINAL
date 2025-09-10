#include "Parser.h"
#include <iostream>
#include <stdexcept>
#include <vector>

// Constructor and core token handling
Parser::Parser(Lexer& lexer) : lexer(lexer) {
    advance();
    advance();
}

void Parser::advance() {
    previousToken = currentToken;
    currentToken = peekToken;
    peekToken = lexer.nextToken();
}

Token Parser::peek() {
    return peekToken;
}

bool Parser::check(TokenType type) {
    return currentToken.type == type;
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

void Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        advance();
        return;
    }
    throw std::runtime_error("Parser Error: " + message + " (at line " + std::to_string(currentToken.line) + ", got '" + currentToken.lexeme + "')");
}

// --- Expression Parsing (Recursive Descent) ---
std::unique_ptr<ExprNode> Parser::parseExpression() { return parseTernary(); }
std::unique_ptr<ExprNode> Parser::parseTernary() {
    auto expr = parseLogical();
    if (match(TokenType::TOKEN_QUESTION)) {
        auto then_branch = parseTernary();
        consume(TokenType::TOKEN_COLON, "Expect ':' for ternary operator.");
        auto else_branch = parseTernary();
        expr = std::make_unique<TernaryNode>(std::move(expr), std::move(then_branch), std::move(else_branch));
    }
    return expr;
}
std::unique_ptr<ExprNode> Parser::parseLogical() {
    auto expr = parseEquality();
    while (match(TokenType::TOKEN_AMPERSAND_AMPERSAND) || match(TokenType::TOKEN_PIPE_PIPE)) {
        Token op = previousToken;
        auto right = parseEquality();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}
std::unique_ptr<ExprNode> Parser::parseEquality() {
    auto expr = parseComparison();
    while (match(TokenType::TOKEN_EQUAL_EQUAL) || match(TokenType::TOKEN_BANG_EQUAL)) {
        Token op = previousToken;
        auto right = parseComparison();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}
std::unique_ptr<ExprNode> Parser::parseComparison() {
    auto expr = parseTerm();
    while (match(TokenType::TOKEN_GREATER) || match(TokenType::TOKEN_GREATER_EQUAL) || match(TokenType::TOKEN_LESS) || match(TokenType::TOKEN_LESS_EQUAL)) {
        Token op = previousToken;
        auto right = parseTerm();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}
std::unique_ptr<ExprNode> Parser::parseTerm() {
    auto expr = parseFactor();
    while (match(TokenType::TOKEN_PLUS) || match(TokenType::TOKEN_MINUS)) {
        Token op = previousToken;
        auto right = parseFactor();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}
std::unique_ptr<ExprNode> Parser::parseFactor() {
    auto expr = parsePower();
    while (match(TokenType::TOKEN_STAR) || match(TokenType::TOKEN_SLASH) || match(TokenType::TOKEN_PERCENT)) {
        Token op = previousToken;
        auto right = parsePower();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}
std::unique_ptr<ExprNode> Parser::parsePower() {
    auto expr = parseUnary();
    while (match(TokenType::TOKEN_STAR_STAR)) {
        Token op = previousToken;
        auto right = parseUnary();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}
std::unique_ptr<ExprNode> Parser::parseUnary() {
    if (match(TokenType::TOKEN_MINUS)) {
        Token op = previousToken;
        auto right = parseUnary();
        return std::make_unique<UnaryOpNode>(op, std::move(right));
    }
    return parseMemberAccess();
}
std::unique_ptr<ExprNode> Parser::parseMemberAccess() {
    auto expr = parsePrimary();
    while (match(TokenType::TOKEN_DOT)) {
        Token prop = currentToken;
        consume(TokenType::TOKEN_IDENTIFIER, "Expect property name after '.'.");
        expr = std::make_unique<MemberAccessNode>(std::move(expr), prop);
    }
    return expr;
}
std::unique_ptr<ExprNode> Parser::parsePrimary() {
    if (match(TokenType::TOKEN_STRING) || match(TokenType::TOKEN_NUMBER) || match(TokenType::TOKEN_IDENTIFIER)) {
        return std::make_unique<LiteralNode>(previousToken);
    }
    if (match(TokenType::TOKEN_LPAREN)) {
        auto expr = parseExpression();
        consume(TokenType::TOKEN_RPAREN, "Expect ')' after expression.");
        return std::make_unique<GroupingNode>(std::move(expr));
    }
    if (check(TokenType::TOKEN_HASH) || check(TokenType::TOKEN_DOT)) {
        Token selectorToken = currentToken;
        advance();
        if (check(TokenType::TOKEN_IDENTIFIER)) {
            selectorToken.lexeme += currentToken.lexeme;
            advance();
            return std::make_unique<LiteralNode>(selectorToken);
        }
    }
    throw std::runtime_error("Expected expression.");
}

// --- Main Parser Logic ---

AstNodePtr Parser::parseImportNode() {
    consume(TokenType::KEYWORD_IMPORT, "Expect '[Import]' keyword.");
    ImportInfo info;

    // For now, only parse the simple form: [Import] @Chtl from "path";
    consume(TokenType::TOKEN_AT, "Expect '@' after '[Import]'.");
    consume(TokenType::TOKEN_IDENTIFIER, "Expect import type (e.g., Chtl).");
    info.import_type = previousToken.lexeme;

    consume(TokenType::KEYWORD_FROM, "Expect 'from' keyword.");

    if (match(TokenType::TOKEN_STRING)) {
        info.path = previousToken.lexeme.substr(1, previousToken.lexeme.length() - 2);
    } else {
        throw std::runtime_error("Expect path string after 'from'.");
    }

    // Optional 'as' part would be parsed here.

    consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after import statement.");
    return std::make_unique<ImportNode>(info);
}

AstNodePtr Parser::parseNamespaceNode() {
    consume(TokenType::KEYWORD_NAMESPACE, "Expect '[Namespace]' keyword.");
    consume(TokenType::TOKEN_IDENTIFIER, "Expect namespace name.");
    auto node = std::make_unique<NamespaceNode>(previousToken.lexeme);

    consume(TokenType::TOKEN_LBRACE, "Expect '{' to start namespace block.");
    while (!check(TokenType::TOKEN_RBRACE) && !check(TokenType::TOKEN_EOF)) {
        node->children.push_back(parseDeclaration());
    }
    consume(TokenType::TOKEN_RBRACE, "Expect '}' to end namespace block.");

    return node;
}

AstNodePtr Parser::parseOriginNode() {
    consume(TokenType::KEYWORD_ORIGIN, "Expect '[Origin]' keyword.");
    consume(TokenType::TOKEN_AT, "Expect '@' after '[Origin]'.");
    consume(TokenType::TOKEN_IDENTIFIER, "Expect origin type identifier.");
    std::string type = previousToken.lexeme;
    consume(TokenType::TOKEN_LBRACE, "Expect '{' to start origin block.");

    size_t start_pos = currentToken.start;

    int brace_level = 1;
    while (brace_level > 0 && !check(TokenType::TOKEN_EOF)) {
        if (check(TokenType::TOKEN_LBRACE)) brace_level++;
        if (check(TokenType::TOKEN_RBRACE)) brace_level--;
        advance();
    }

    size_t end_pos = previousToken.start;
    std::string content = lexer.getSource().substr(start_pos, end_pos - start_pos);

    return std::make_unique<OriginNode>(type, content);
}

NodeList Parser::parse() {
    NodeList nodes;
    while (!check(TokenType::TOKEN_EOF)) {
        nodes.push_back(parseDeclaration());
    }
    return nodes;
}

AstNodePtr Parser::parseDeclaration() {
    if (check(TokenType::KEYWORD_ORIGIN)) {
        return parseOriginNode();
    }
    if (check(TokenType::KEYWORD_NAMESPACE)) {
        return parseNamespaceNode();
    }
    if (check(TokenType::KEYWORD_IMPORT)) {
        return parseImportNode();
    }
    if (check(TokenType::TOKEN_GENERATOR_COMMENT)) {
        return parseCommentNode();
    }
    if (check(TokenType::TOKEN_IDENTIFIER) && peek().type == TokenType::TOKEN_LBRACE) {
        return parseElement();
    }
    throw std::runtime_error("Parser Error: Unexpected token at top level: " + currentToken.lexeme);
}

AstNodePtr Parser::parseElement() {
    auto element = std::make_unique<ElementNode>(currentToken.lexeme);
    advance();
    consume(TokenType::TOKEN_LBRACE, "Expect '{' after element name.");

    while (!check(TokenType::TOKEN_RBRACE) && !check(TokenType::TOKEN_EOF)) {
        if (check(TokenType::KEYWORD_ORIGIN)) {
            element->children.push_back(parseOriginNode());
        } else if (check(TokenType::TOKEN_IDENTIFIER)) {
            if (peek().type == TokenType::TOKEN_COLON) {
                std::string key = currentToken.lexeme;
                advance(); advance();
                auto value = parseExpression();
                element->style_properties.push_back(std::make_pair(key, std::move(value)));
                consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after property value.");
            } else if (peek().type == TokenType::TOKEN_EQUAL) {
                std::string key = currentToken.lexeme;
                advance(); advance();
                if (match(TokenType::TOKEN_STRING)) {
                     element->simple_attributes.push_back({key, previousToken.lexeme.substr(1, previousToken.lexeme.length() - 2)});
                } else {
                    throw std::runtime_error("Expect string for attribute value.");
                }
                consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after attribute value.");
            } else {
                element->children.push_back(parseElement());
            }
        } else if (check(TokenType::TOKEN_KEYWORD_TEXT)) {
            element->children.push_back(parseTextNode());
        } else if (check(TokenType::TOKEN_GENERATOR_COMMENT)) {
            element->children.push_back(parseCommentNode());
        } else {
            throw std::runtime_error("Unexpected token in element body: " + currentToken.lexeme);
        }
    }
    consume(TokenType::TOKEN_RBRACE, "Expect '}' after element block.");
    return element;
}

AstNodePtr Parser::parseTextNode() {
    advance();
    consume(TokenType::TOKEN_LBRACE, "Expect '{' after 'text' keyword.");
    std::string content;
    if (match(TokenType::TOKEN_STRING)) {
        content = previousToken.lexeme.substr(1, previousToken.lexeme.length() - 2);
    } else if (match(TokenType::TOKEN_IDENTIFIER)) {
        content = previousToken.lexeme;
    } else {
        content = "";
    }
    consume(TokenType::TOKEN_RBRACE, "Expect '}' after text content.");
    return std::make_unique<TextNode>(content);
}

AstNodePtr Parser::parseCommentNode() {
    std::string content = currentToken.lexeme.substr(2);
    size_t first = content.find_first_not_of(' ');
    if (std::string::npos != first) {
        content = content.substr(first);
    }
    advance();
    return std::make_unique<CommentNode>(content);
}
