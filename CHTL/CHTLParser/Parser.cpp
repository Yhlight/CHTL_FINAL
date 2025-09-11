#include "Parser.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/PropertyNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/ExprNode.h"
#include "CHTLNode/TemplateNode.h"
#include "CHTLNode/CustomNode.h"
#include "CHTLNode/ImportNode.h"
#include "CHTLNode/ConfigurationNode.h"
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
    if (peek().type == TokenType::Identifier) {
        return parseElement();
    }
    if (match({TokenType::TemplateKeyword})) {
        return parseTemplateDefinition();
    }
    if (match({TokenType::CustomKeyword})) {
        return parseCustomDefinition();
    }
    if (match({TokenType::ImportKeyword})) {
        return parseImport();
    }
    if (match({TokenType::ConfigurationKeyword})) {
        return parseConfiguration();
    }
    throw error(peek(), "Expect a declaration (e.g., an element, template, custom, or import).");
}

NodePtr Parser::parseConfiguration() {
    auto node = std::make_unique<ConfigurationNode>();
    consume(TokenType::OpenBrace, "Expect '{' after [Configuration].");
    while(!check(TokenType::CloseBrace) && !isAtEnd()) {
        skipComments();
        if(check(TokenType::CloseBrace)) break;
        node->settings.push_back(std::unique_ptr<PropertyNode>(static_cast<PropertyNode*>(parseProperty().release())));
    }
    consume(TokenType::CloseBrace, "Expect '}' after configuration block.");
    return node;
}

NodePtr Parser::parseImport() {
    // This is a simplified parser for now. It assumes a simple `@Type from "path"` syntax.
    consume(TokenType::At, "Expect '@' after [Import].");
    const Token& type = consume(TokenType::Identifier, "Expect import type.");

    consume(TokenType::FromKeyword, "Expect 'from' keyword.");

    const Token& path = consume(TokenType::StringLiteral, "Expect path string.");

    std::string alias;
    if (match({TokenType::Identifier}) && previous().value == "as") {
        alias = consume(TokenType::Identifier, "Expect alias name.").value;
    }

    consume(TokenType::Semicolon, "Expect ';' after import statement.");

    return std::make_unique<ImportNode>(type.value, path.value, alias);
}

NodePtr Parser::parseCustomDefinition() {
    consume(TokenType::At, "Expect '@' after [Custom].");
    const Token& type = consume(TokenType::Identifier, "Expect custom type (Style, Element, Var).");

    if (type.value == "Style") {
        const Token& name = consume(TokenType::Identifier, "Expect custom name.");
        auto node = std::make_unique<CustomStyleDefinitionNode>(name.value);
        consume(TokenType::OpenBrace, "Expect '{' after custom name.");
        while(!check(TokenType::CloseBrace) && !isAtEnd()) {
            skipComments();
            if(check(TokenType::CloseBrace)) break;
            node->properties.push_back(std::unique_ptr<PropertyNode>(static_cast<PropertyNode*>(parseProperty().release())));
        }
        consume(TokenType::CloseBrace, "Expect '}' after custom block.");
        return node;
    } else if (type.value == "Element") {
        const Token& name = consume(TokenType::Identifier, "Expect custom name.");
        auto node = std::make_unique<CustomElementDefinitionNode>(name.value);
        consume(TokenType::OpenBrace, "Expect '{' after custom name.");
        while(!check(TokenType::CloseBrace) && !isAtEnd()) {
            skipComments();
            if(check(TokenType::CloseBrace)) break;
            node->children.push_back(parseElement());
        }
        consume(TokenType::CloseBrace, "Expect '}' after custom block.");
        return node;
    } else if (type.value == "Var") {
        const Token& name = consume(TokenType::Identifier, "Expect custom name.");
        auto node = std::make_unique<CustomVarDefinitionNode>(name.value);
        consume(TokenType::OpenBrace, "Expect '{' after custom name.");
        while(!check(TokenType::CloseBrace) && !isAtEnd()) {
            skipComments();
            if(check(TokenType::CloseBrace)) break;
            node->variables.push_back(std::unique_ptr<PropertyNode>(static_cast<PropertyNode*>(parseProperty().release())));
        }
        consume(TokenType::CloseBrace, "Expect '}' after custom block.");
        return node;
    }
    throw error(type, "Unknown custom type.");
}

NodePtr Parser::parseTemplateDefinition() {
    consume(TokenType::At, "Expect '@' after [Template].");
    const Token& type = consume(TokenType::Identifier, "Expect template type (Style, Element, Var).");

    if (type.value == "Style") {
        return parseStyleTemplateDefinition();
    } else if (type.value == "Element") {
        return parseElementTemplateDefinition();
    } else if (type.value == "Var") {
        return parseVarTemplateDefinition();
    }
    throw error(type, "Unknown template type.");
}

NodePtr Parser::parseStyleTemplateDefinition() {
    const Token& name = consume(TokenType::Identifier, "Expect template name.");
    auto node = std::make_unique<StyleTemplateDefinitionNode>(name.value);
    consume(TokenType::OpenBrace, "Expect '{' after template name.");
    while(!check(TokenType::CloseBrace) && !isAtEnd()) {
        skipComments();
        if(check(TokenType::CloseBrace)) break;
        node->properties.push_back(std::unique_ptr<PropertyNode>(static_cast<PropertyNode*>(parseProperty().release())));
    }
    consume(TokenType::CloseBrace, "Expect '}' after template block.");
    return node;
}

NodePtr Parser::parseElementTemplateDefinition() {
    const Token& name = consume(TokenType::Identifier, "Expect template name.");
    auto node = std::make_unique<ElementTemplateDefinitionNode>(name.value);
    consume(TokenType::OpenBrace, "Expect '{' after template name.");
    while(!check(TokenType::CloseBrace) && !isAtEnd()) {
        skipComments();
        if(check(TokenType::CloseBrace)) break;
        node->children.push_back(parseElement());
    }
    consume(TokenType::CloseBrace, "Expect '}' after template block.");
    return node;
}

NodePtr Parser::parseVarTemplateDefinition() {
    const Token& name = consume(TokenType::Identifier, "Expect template name.");
    auto node = std::make_unique<VarTemplateDefinitionNode>(name.value);
    consume(TokenType::OpenBrace, "Expect '{' after template name.");
    while(!check(TokenType::CloseBrace) && !isAtEnd()) {
        skipComments();
        if(check(TokenType::CloseBrace)) break;
        node->variables.push_back(std::unique_ptr<PropertyNode>(static_cast<PropertyNode*>(parseProperty().release())));
    }
    consume(TokenType::CloseBrace, "Expect '}' after template block.");
    return node;
}

NodePtr Parser::parseTemplateUsage() {
    consume(TokenType::At, "Expect '@' for template usage.");
    const Token& type = consume(TokenType::Identifier, "Expect template type (Style, Element).");
    const Token& name = consume(TokenType::Identifier, "Expect template name.");

    if (type.value == "Style") {
        consume(TokenType::Semicolon, "Expect ';' after @Style usage.");
        return std::make_unique<StyleUsageNode>(name.value);
    }

    if (type.value == "Element") {
        auto node = std::make_unique<ElementUsageNode>(name.value);
        if (match({TokenType::OpenBrace})) {
            // Specialization block
            while(!check(TokenType::CloseBrace) && !isAtEnd()) {
                skipComments();
                if(check(TokenType::CloseBrace)) break;
                if (peek().type == TokenType::DeleteKeyword) {
                    node->specializations.push_back(parseDelete());
                } else if (peek().type == TokenType::InsertKeyword) {
                    node->specializations.push_back(parseInsert());
                } else {
                    throw error(peek(), "Only 'delete' or 'insert' allowed in specialization block.");
                }
            }
            consume(TokenType::CloseBrace, "Expect '}' after specialization block.");
        } else {
            consume(TokenType::Semicolon, "Expect ';' after @Element usage.");
        }
        return node;
    }

    throw error(type, "Unknown template usage type.");
}

NodePtr Parser::parseDelete() {
    consume(TokenType::DeleteKeyword, "Expect 'delete' keyword.");
    // For now, we assume the target is a simple identifier.
    // The spec mentions selectors like `div[1]`, which will require a more complex parser later.
    const Token& target = consume(TokenType::Identifier, "Expect target for delete.");
    consume(TokenType::Semicolon, "Expect ';' after delete statement.");
    return std::make_unique<DeleteNode>(target.value);
}

NodePtr Parser::parseInsert() {
    consume(TokenType::InsertKeyword, "Expect 'insert' keyword.");
    // This is a placeholder. A full implementation would parse position, selector, and a block.
    throw error(peek(), "Insert statement parsing is not yet implemented.");
}


NodePtr Parser::parseElement() {
    const Token& nameToken = consume(TokenType::Identifier, "Expect element name.");
    auto element = std::make_unique<ElementNode>(nameToken.value);

    consume(TokenType::OpenBrace, "Expect '{' after element name.");

    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        skipComments();
        if (check(TokenType::CloseBrace)) break;

        if (peek().type == TokenType::Identifier) {
            if (tokens[current + 1].type == TokenType::Colon || tokens[current + 1].type == TokenType::Equals) {
                element->addChild(parseProperty());
            } else if (tokens[current + 1].type == TokenType::OpenBrace) {
                 if (peek().value == "text") {
                    element->addChild(parseTextNode());
                } else if (peek().value == "style") {
                    element->addChild(parseStyleNode());
                } else {
                    element->addChild(parseElement());
                }
            } else {
                throw error(peek(), "Unexpected token inside element.");
            }
        } else if (peek().type == TokenType::At) {
            element->addChild(parseTemplateUsage());
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

    auto value = parseExpression();

    consume(TokenType::Semicolon, "Expect ';' after property value.");

    return std::make_unique<PropertyNode>(nameToken.value, std::move(value));
}

NodePtr Parser::parseTextNode() {
    consume(TokenType::Identifier, "Expect 'text' keyword.");
    consume(TokenType::OpenBrace, "Expect '{' after 'text' keyword.");

    std::string textContent;
    if (peek().type == TokenType::StringLiteral) {
        textContent = consume(TokenType::StringLiteral, "Expect string literal in text block.").value;
    } else {
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

NodePtr Parser::parseStyleNode() {
    consume(TokenType::Identifier, "Expect 'style' keyword.");
    consume(TokenType::OpenBrace, "Expect '{' after 'style' keyword.");

    auto styleNode = std::make_unique<StyleNode>();

    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        skipComments();
        if (check(TokenType::CloseBrace)) break;

        if (peek().type == TokenType::At) {
            styleNode->addChild(parseTemplateUsage());
        } else {
            styleNode->addChild(parseProperty());
        }
    }

    consume(TokenType::CloseBrace, "Expect '}' after style block.");
    return styleNode;
}

// --- Expression Parsing ---

ExprNodePtr Parser::parseExpression() {
    return parseTernary();
}

ExprNodePtr Parser::parseTernary() {
    ExprNodePtr expr = parseLogicalOr();

    if (match({TokenType::QuestionMark})) {
        ExprNodePtr trueExpr = parseExpression();
        consume(TokenType::Colon, "Expect ':' after true branch of ternary expression.");
        ExprNodePtr falseExpr = parseExpression();
        expr = std::make_unique<TernaryExprNode>(std::move(expr), std::move(trueExpr), std::move(falseExpr));
    }

    return expr;
}

ExprNodePtr Parser::parseLogicalOr() {
    ExprNodePtr expr = parseLogicalAnd();
    while (match({TokenType::LogicalOr})) {
        Token op = previous();
        ExprNodePtr right = parseLogicalAnd();
        expr = std::make_unique<BinaryExprNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprNodePtr Parser::parseLogicalAnd() {
    ExprNodePtr expr = parseComparison();
    while (match({TokenType::LogicalAnd})) {
        Token op = previous();
        ExprNodePtr right = parseComparison();
        expr = std::make_unique<BinaryExprNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprNodePtr Parser::parseComparison() {
    ExprNodePtr expr = parseTerm();
    while (match({TokenType::GreaterThan, TokenType::LessThan})) {
        Token op = previous();
        ExprNodePtr right = parseTerm();
        expr = std::make_unique<BinaryExprNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprNodePtr Parser::parseTerm() {
    ExprNodePtr expr = parseFactor();
    while (match({TokenType::Plus, TokenType::Minus})) {
        Token op = previous();
        ExprNodePtr right = parseFactor();
        expr = std::make_unique<BinaryExprNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprNodePtr Parser::parseFactor() {
    ExprNodePtr expr = parsePrimary();
    while (match({TokenType::Asterisk, TokenType::Slash, TokenType::Percent})) {
        Token op = previous();
        ExprNodePtr right = parsePrimary();
        expr = std::make_unique<BinaryExprNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprNodePtr Parser::parsePrimary() {
    if (match({TokenType::Identifier, TokenType::StringLiteral})) {
        return std::make_unique<LiteralExprNode>(previous());
    }

    if (match({TokenType::OpenParen})) {
        ExprNodePtr expr = parseExpression();
        consume(TokenType::CloseParen, "Expect ')' after expression.");
        return expr;
    }

    throw error(peek(), "Expect expression.");
}


NodePtr Parser::parseComment(const Token& token) {
    bool isGenerator = token.type == TokenType::GeneratorComment;
    return std::make_unique<CommentNode>(token.value, isGenerator);
}


// --- Helper Methods ---

void Parser::skipComments() {
    while (peek().type == TokenType::SingleLineComment ||
           peek().type == TokenType::MultiLineComment ||
           peek().type == TokenType::GeneratorComment) {
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
    std::string error_message;
    if (token.type == TokenType::EndOfFile) {
        error_message = "Parse Error at end of file: " + message;
    } else {
         error_message = "Parse Error at line " + std::to_string(token.line) +
                               " column " + std::to_string(token.column) +
                               " near '" + token.value + "': " + message;
    }
    return ParseError(error_message);
}
