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
    if (check(TokenType::TOKEN_IDENTIFIER) && peek().type == TokenType::TOKEN_LPAREN) {
        // This is a variable access, e.g. MyGroup(myVar)
        Token group = currentToken;
        advance(); // consume group name
        advance(); // consume '('
        Token var = currentToken;
        consume(TokenType::TOKEN_IDENTIFIER, "Expect variable name in parentheses.");
        consume(TokenType::TOKEN_RPAREN, "Expect ')' after variable name.");
        return std::make_unique<VarAccessNode>(group, var);
    }

    if (match(TokenType::TOKEN_STRING) || match(TokenType::TOKEN_IDENTIFIER)) {
        return std::make_unique<LiteralNode>(previousToken);
    }
    if (match(TokenType::TOKEN_NUMBER)) {
        Token numberToken = previousToken;
        if (check(TokenType::TOKEN_IDENTIFIER)) {
            // It's a number with a unit, combine them.
            numberToken.lexeme += currentToken.lexeme;
            advance();
        }
        return std::make_unique<LiteralNode>(numberToken);
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

AstNodePtr Parser::parseCustomNode() {
    consume(TokenType::KEYWORD_CUSTOM, "Expect '[Custom]' keyword.");
    consume(TokenType::TOKEN_AT, "Expect '@' after '[Custom]'.");
    consume(TokenType::TOKEN_IDENTIFIER, "Expect custom type (Style, Element, Var).");
    std::string typeStr = previousToken.lexeme;

    TemplateType type;
    if (typeStr == "Style") type = TemplateType::STYLE;
    else if (typeStr == "Element") type = TemplateType::ELEMENT;
    else if (typeStr == "Var") type = TemplateType::VAR;
    else throw std::runtime_error("Unknown custom type: " + typeStr);

    consume(TokenType::TOKEN_IDENTIFIER, "Expect custom name.");
    auto node = std::make_unique<CustomNode>(previousToken.lexeme, type);

    consume(TokenType::TOKEN_LBRACE, "Expect '{' to start custom block.");

    // For now, parsing the body is the same as for templates.
    if (type == TemplateType::STYLE) {
        while(check(TokenType::TOKEN_IDENTIFIER) || check(TokenType::TOKEN_AT)) {
            if (check(TokenType::TOKEN_IDENTIFIER)) {
                // Check for valueless property vs. property with value
                if (peek().type == TokenType::TOKEN_SEMICOLON) {
                    node->valueless_style_properties.push_back(currentToken.lexeme);
                    advance(); // consume identifier
                    advance(); // consume semicolon
                } else {
                    parseProperties(node->style_properties);
                }
            } else if (check(TokenType::TOKEN_AT)) {
                node->inherited_templates.push_back(
                    std::unique_ptr<TemplateUsageNode>(dynamic_cast<TemplateUsageNode*>(parseTemplateUsageNode().release()))
                );
            }
        }
    } else if (type == TemplateType::ELEMENT) {
        while (!check(TokenType::TOKEN_RBRACE) && !check(TokenType::TOKEN_EOF)) {
            node->element_children.push_back(parseDeclaration());
        }
    } else if (type == TemplateType::VAR) {
        while (check(TokenType::TOKEN_IDENTIFIER)) {
            std::string key = currentToken.lexeme;
            advance();
            consume(TokenType::TOKEN_COLON, "Expect ':' after var name.");
            if (match(TokenType::TOKEN_STRING)) {
                node->variables[key] = previousToken.lexeme.substr(1, previousToken.lexeme.length() - 2);
            } else {
                throw std::runtime_error("Expect string literal for var value.");
            }
            consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after var value.");
        }
    }

    consume(TokenType::TOKEN_RBRACE, "Expect '}' to end custom block.");
    return node;
}


void Parser::parseProperties(std::vector<std::pair<std::string, std::unique_ptr<ExprNode>>>& properties) {
     while (check(TokenType::TOKEN_IDENTIFIER) && peek().type == TokenType::TOKEN_COLON) {
        std::string key = currentToken.lexeme;
        advance(); // consume identifier
        advance(); // consume ':'

        auto value = parseExpression();
        properties.push_back(std::make_pair(key, std::move(value)));

        consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after property value.");
    }
}

AstNodePtr Parser::parseTemplateNode() {
    consume(TokenType::KEYWORD_TEMPLATE, "Expect '[Template]' keyword.");
    consume(TokenType::TOKEN_AT, "Expect '@' after '[Template]'.");
    consume(TokenType::TOKEN_IDENTIFIER, "Expect template type (Style, Element, Var).");
    std::string typeStr = previousToken.lexeme;

    TemplateType type;
    if (typeStr == "Style") type = TemplateType::STYLE;
    else if (typeStr == "Element") type = TemplateType::ELEMENT;
    else if (typeStr == "Var") type = TemplateType::VAR;
    else throw std::runtime_error("Unknown template type: " + typeStr);

    consume(TokenType::TOKEN_IDENTIFIER, "Expect template name.");
    auto node = std::make_unique<TemplateNode>(previousToken.lexeme, type);

    consume(TokenType::TOKEN_LBRACE, "Expect '{' to start template block.");

    if (type == TemplateType::STYLE) {
        // A style template can contain properties or inherit from another style template
        while(check(TokenType::TOKEN_IDENTIFIER) || check(TokenType::TOKEN_AT)) {
            if (check(TokenType::TOKEN_IDENTIFIER)) {
                parseProperties(node->style_properties);
            } else if (check(TokenType::TOKEN_AT)) {
                node->inherited_templates.push_back(
                    std::unique_ptr<TemplateUsageNode>(dynamic_cast<TemplateUsageNode*>(parseTemplateUsageNode().release()))
                );
            }
        }
    } else if (type == TemplateType::ELEMENT) {
        // An element template contains child nodes
        while (!check(TokenType::TOKEN_RBRACE) && !check(TokenType::TOKEN_EOF)) {
            node->element_children.push_back(parseDeclaration());
        }
    } else if (type == TemplateType::VAR) {
        // A var template contains key-value string pairs
        while (check(TokenType::TOKEN_IDENTIFIER)) {
            std::string key = currentToken.lexeme;
            advance();
            consume(TokenType::TOKEN_COLON, "Expect ':' after var name.");
            if (match(TokenType::TOKEN_STRING)) {
                node->variables[key] = previousToken.lexeme.substr(1, previousToken.lexeme.length() - 2);
            } else {
                throw std::runtime_error("Expect string literal for var value.");
            }
            consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after var value.");
        }
    }

    consume(TokenType::TOKEN_RBRACE, "Expect '}' to end template block.");
    return node;
}

AstNodePtr Parser::parseTemplateUsageNode() {
    consume(TokenType::TOKEN_AT, "Expect '@' keyword for template usage.");
    consume(TokenType::TOKEN_IDENTIFIER, "Expect template type.");
    std::string typeStr = previousToken.lexeme;

    TemplateType type;
    if (typeStr == "Style") type = TemplateType::STYLE;
    else if (typeStr == "Element") type = TemplateType::ELEMENT;
    else if (typeStr == "Var") type = TemplateType::VAR;
    else throw std::runtime_error("Unknown template usage type: " + typeStr);

    consume(TokenType::TOKEN_IDENTIFIER, "Expect template name.");
    auto node = std::make_unique<TemplateUsageNode>(previousToken.lexeme, type);

    // Check for optional specialization block
    if (match(TokenType::TOKEN_LBRACE)) {
        while (!check(TokenType::TOKEN_RBRACE) && !check(TokenType::TOKEN_EOF)) {
            if (match(TokenType::KEYWORD_DELETE)) {
                // Handle `delete prop;` or `delete @Style Other;`
                if (match(TokenType::TOKEN_AT)) {
                    consume(TokenType::TOKEN_IDENTIFIER, "Expect template type after @delete.");
                    consume(TokenType::TOKEN_IDENTIFIER, "Expect template name after @delete type.");
                    node->deleted_inherited_templates.push_back(previousToken.lexeme);
                } else {
                    consume(TokenType::TOKEN_IDENTIFIER, "Expect property name after delete.");
                    node->deleted_properties.push_back(previousToken.lexeme);
                }
                consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after delete statement.");
            } else {
                // Handle property overrides
                parseProperties(node->property_overrides);
            }
        }
        consume(TokenType::TOKEN_RBRACE, "Expect '}' to end specialization block.");
    } else {
        consume(TokenType::TOKEN_SEMICOLON, "Expect ';' after template usage.");
    }

    return node;
}


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
    if (check(TokenType::KEYWORD_TEMPLATE)) {
        return parseTemplateNode();
    }
    if (check(TokenType::KEYWORD_CUSTOM)) {
        return parseCustomNode();
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
        } else if (check(TokenType::TOKEN_AT)) {
            element->children.push_back(parseTemplateUsageNode());
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
