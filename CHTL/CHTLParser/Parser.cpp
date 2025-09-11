#include "Parser.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/PropertyNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/ScriptNode.h"
#include "CHTLNode/ExprNode.h"
#include "CHTLNode/TemplateNode.h"
#include "CHTLNode/CustomNode.h"
#include "CHTLNode/ImportNode.h"
#include "CHTLNode/ConfigurationNode.h"
#include "CHTLNode/NamespaceNode.h"
#include "CHTLNode/UseNode.h"
#include "CHTLNode/ConstraintNode.h"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens(tokens) {}

NodeList Parser::parse() {
    NodeList nodes;
    // `use` statements must come first.
    while (peek().type == TokenType::UseKeyword) {
        nodes.push_back(parseUseStatement());
        skipComments();
    }

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
    if (match({TokenType::NamespaceKeyword})) {
        return parseNamespace();
    }
    throw error(peek(), "Expect a declaration (e.g., an element, template, custom, import, or namespace).");
}

NodePtr Parser::parseConfiguration() {
    auto node = std::make_unique<ConfigurationNode>();
    consume(TokenType::OpenBrace, "Expect '{' after [Configuration].");
    while(!check(TokenType::CloseBrace) && !isAtEnd()) {
        skipComments();
        if(check(TokenType::CloseBrace)) break;
        const Token& key = consume(TokenType::Identifier, "Expect configuration key.");
        consume(TokenType::Equals, "Expect '=' in configuration setting.");
        auto value_expr = std::make_unique<LiteralExprNode>(advance());
        auto prop_node = std::make_unique<PropertyNode>(key.value, std::move(value_expr));
        node->settings.push_back(std::move(prop_node));
        consume(TokenType::Semicolon, "Expect ';' after configuration setting.");
    }
    consume(TokenType::CloseBrace, "Expect '}' after configuration block.");
    return node;
}

NodePtr Parser::parseImport() {
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
    const Token& name = consume(TokenType::Identifier, "Expect custom name.");

    if (type.value == "Style") {
        auto node = std::make_unique<CustomStyleDefinitionNode>(name.value);
        consume(TokenType::OpenBrace, "Expect '{' after custom name.");
        while(!check(TokenType::CloseBrace) && !isAtEnd()) {
            skipComments();
            if(check(TokenType::CloseBrace)) break;
            node->properties.push_back(std::unique_ptr<PropertyNode>(static_cast<PropertyNode*>(parseProperty().release())));
        }
        consume(TokenType::CloseBrace, "Expect '}' after custom block.");
        return node;
    }
    // Other custom types...
    throw error(type, "Unknown or unimplemented custom type.");
}

NodePtr Parser::parseTemplateDefinition() {
    consume(TokenType::At, "Expect '@' after [Template].");
    const Token& type = consume(TokenType::Identifier, "Expect template type (Style, Element, Var).");
    if (type.value == "Style") return parseStyleTemplateDefinition();
    if (type.value == "Element") return parseElementTemplateDefinition();
    if (type.value == "Var") return parseVarTemplateDefinition();
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

    std::string namespace_name;
    if (match({TokenType::FromKeyword})) {
        namespace_name = consume(TokenType::Identifier, "Expect namespace name after 'from'.").value;
        // TODO: Handle nested namespaces like `space.room`
    }

    if (type.value == "Style") {
        consume(TokenType::Semicolon, "Expect ';' after @Style usage.");
        return std::make_unique<StyleUsageNode>(name.value, namespace_name);
    }
    if (type.value == "Element") {
        auto node = std::make_unique<ElementUsageNode>(name.value, namespace_name);
        if (match({TokenType::OpenBrace})) {
            while(!check(TokenType::CloseBrace) && !isAtEnd()) {
                skipComments();
                if(check(TokenType::CloseBrace)) break;
                if (peek().type == TokenType::DeleteKeyword) node->specializations.push_back(parseDelete());
                else if (peek().type == TokenType::InsertKeyword) node->specializations.push_back(parseInsert());
                else throw error(peek(), "Only 'delete' or 'insert' allowed in specialization block.");
            }
            consume(TokenType::CloseBrace, "Expect '}' after specialization block.");
        } else {
            consume(TokenType::Semicolon, "Expect ';' after @Element usage.");
        }
        return node;
    }
    throw error(type, "Unknown template usage type.");
}

NodePtr Parser::parseNamespace() {
    const Token& name = consume(TokenType::Identifier, "Expect namespace name.");
    auto node = std::make_unique<NamespaceNode>(name.value);
    consume(TokenType::OpenBrace, "Expect '{' after namespace name.");
    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        skipComments();
        if (check(TokenType::CloseBrace)) break;
        node->addChild(parseDeclaration());
    }
    consume(TokenType::CloseBrace, "Expect '}' after namespace block.");
    return node;
}

NodePtr Parser::parseUseStatement() {
    consume(TokenType::UseKeyword, "Expect 'use' keyword.");
    std::vector<Token> target;
    while(!check(TokenType::Semicolon) && !isAtEnd()) {
        target.push_back(advance());
    }
    consume(TokenType::Semicolon, "Expect ';' after use statement.");
    return std::make_unique<UseNode>(std::move(target));
}

NodePtr Parser::parseConstraint() {
    std::vector<Token> targets;
    do {
        targets.push_back(advance());
    } while (match({TokenType::Comma}));
    consume(TokenType::Semicolon, "Expect ';' after except statement.");
    return std::make_unique<ConstraintNode>(std::move(targets));
}

NodePtr Parser::parseDelete() {
    consume(TokenType::DeleteKeyword, "Expect 'delete' keyword.");
    const Token& target = consume(TokenType::Identifier, "Expect target for delete.");

    int index = -1;
    if (match({TokenType::OpenBracket})) {
        const Token& indexToken = consume(TokenType::Identifier, "Expect index inside [].");
        try {
            index = std::stoi(indexToken.value);
        } catch (const std::invalid_argument&) {
            throw error(indexToken, "Invalid index in delete statement.");
        }
        consume(TokenType::CloseBracket, "Expect ']' after index.");
    }

    consume(TokenType::Semicolon, "Expect ';' after delete statement.");
    return std::make_unique<DeleteNode>(target.value, index);
}

NodePtr Parser::parseInsert() {
    consume(TokenType::InsertKeyword, "Expect 'insert' keyword.");

    // Parse position
    const Token& posToken = consume(TokenType::Identifier, "Expect position keyword (after, before, etc.).");
    InsertNode::Position position;
    if (posToken.value == "after") position = InsertNode::Position::After;
    else if (posToken.value == "before") position = InsertNode::Position::Before;
    else if (posToken.value == "replace") position = InsertNode::Position::Replace;
    else if (posToken.value == "top") position = InsertNode::Position::AtTop;
    else if (posToken.value == "bottom") position = InsertNode::Position::AtBottom;
    else throw error(posToken, "Invalid insert position.");

    // Parse selector
    const Token& selector = consume(TokenType::Identifier, "Expect selector for insert.");
    // TODO: Handle complex selectors

    // Parse block
    NodeList children;
    consume(TokenType::OpenBrace, "Expect '{' for insert block.");
    while (!check(TokenType::CloseBrace) && !isAtEnd()) {
        skipComments();
        if (check(TokenType::CloseBrace)) break;
        children.push_back(parseDeclaration());
    }
    consume(TokenType::CloseBrace, "Expect '}' after insert block.");

    return std::make_unique<InsertNode>(position, selector.value, std::move(children));
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
                 if (peek().value == "text") element->addChild(parseTextNode());
                 else if (peek().value == "style") element->addChild(parseStyleNode());
                 else if (peek().value == "script") element->addChild(parseScriptNode());
                 else element->addChild(parseElement());
            } else {
                throw error(peek(), "Unexpected token inside element.");
            }
        } else if (peek().type == TokenType::At) {
            element->addChild(parseTemplateUsage());
        } else if (match({TokenType::ExceptKeyword})) {
            element->addChild(parseConstraint());
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
            if (!textContent.empty()) textContent += " ";
            textContent += advance().value;
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
        if (peek().type == TokenType::At) styleNode->addChild(parseTemplateUsage());
        else styleNode->addChild(parseProperty());
    }
    consume(TokenType::CloseBrace, "Expect '}' after style block.");
    return styleNode;
}

NodePtr Parser::parseScriptNode() {
    consume(TokenType::Identifier, "Expect 'script' keyword.");
    consume(TokenType::OpenBrace, "Expect '{' after 'script' keyword.");
    const Token& placeholder = consume(TokenType::Identifier, "Expect placeholder inside script block.");
    consume(TokenType::CloseBrace, "Expect '}' after script block.");
    return std::make_unique<ScriptNode>(placeholder.value);
}

ExprNodePtr Parser::parseExpression() { return parseTernary(); }

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
    while (match({TokenType::GreaterThan, TokenType::LessThan})) { // Incomplete, should include >=, <=, ==, !=
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
    ExprNodePtr expr = parsePower();
    while (match({TokenType::Asterisk, TokenType::Slash, TokenType::Percent})) {
        Token op = previous();
        ExprNodePtr right = parsePower();
        expr = std::make_unique<BinaryExprNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprNodePtr Parser::parsePower() {
    ExprNodePtr expr = parseUnary();
    if (match({TokenType::DoubleAsterisk})) {
        Token op = previous();
        ExprNodePtr right = parsePower();
        expr = std::make_unique<BinaryExprNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

ExprNodePtr Parser::parseUnary() {
    if (match({TokenType::Minus, TokenType::Plus})) {
        Token op = previous();
        ExprNodePtr right = parseUnary();
        return std::make_unique<UnaryExprNode>(op, std::move(right));
    }
    return parsePrimary();
}

ExprNodePtr Parser::parsePrimary() {
    if (peek().type == TokenType::Identifier && tokens[current + 1].type == TokenType::OpenParen) {
        Token callee = advance();
        consume(TokenType::OpenParen, "Expect '(' after function call callee.");
        std::vector<ExprNodePtr> arguments;
        if (!check(TokenType::CloseParen)) {
            do {
                // An argument can be a simple identifier or a key-value pair for specialization
                ExprNodePtr arg = parseExpression();
                if (auto* lit = dynamic_cast<LiteralExprNode*>(arg.get())) {
                    if (match({TokenType::Equals})) {
                        Token op = previous();
                        ExprNodePtr val = parseExpression();
                        arg = std::make_unique<BinaryExprNode>(std::move(arg), op, std::move(val));
                    }
                }
                arguments.push_back(std::move(arg));
            } while (match({TokenType::Comma}));
        }
        consume(TokenType::CloseParen, "Expect ')' after arguments.");
        return std::make_unique<FunctionCallNode>(callee, std::move(arguments));
    }

    if ((peek().type == TokenType::Hash || peek().type == TokenType::Dot || peek().type == TokenType::Identifier) && tokens[current + 1].type == TokenType::Dot) {
        Token selector = advance();
        consume(TokenType::Dot, "Expect '.' after selector in property reference.");
        Token propertyName = consume(TokenType::Identifier, "Expect property name in property reference.");
        return std::make_unique<PropertyRefNode>(selector, propertyName);
    }
    if (match({TokenType::Identifier, TokenType::StringLiteral, TokenType::UnquotedLiteral})) {
        return std::make_unique<LiteralExprNode>(previous());
    }
    if (match({TokenType::OpenParen})) {
        ExprNodePtr expr = parseExpression();
        consume(TokenType::CloseParen, "Expect ')' after expression.");
        return expr;
    }
    throw error(peek(), "Expect expression, literal, or property reference.");
}

NodePtr Parser::parseComment(const Token& token) {
    bool isGenerator = token.type == TokenType::GeneratorComment;
    return std::make_unique<CommentNode>(token.value, isGenerator);
}

void Parser::skipComments() {
    while (peek().type == TokenType::SingleLineComment ||
           peek().type == TokenType::MultiLineComment ||
           peek().type == TokenType::GeneratorComment) {
        advance();
    }
}

const Token& Parser::peek() const { return tokens[current]; }
const Token& Parser::previous() const { return tokens[current - 1]; }
const Token& Parser::advance() {
    if (!isAtEnd()) current++;
    return previous();
}
bool Parser::isAtEnd() const { return peek().type == TokenType::EndOfFile; }
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
    if (check(type)) return advance();
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
