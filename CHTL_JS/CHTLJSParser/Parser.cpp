#include "Parser.h"
#include <iostream>

CHTLJSParser::CHTLJSParser(const std::vector<CHTLJSToken>& tokens) : tokens(tokens) {}

CHTLJSNodeList CHTLJSParser::parse() {
    CHTLJSNodeList statements;
    while (!isAtEnd()) {
        statements.push_back(parseStatement());
    }
    return statements;
}

CHTLJSNodePtr CHTLJSParser::parseStatement() {
    if (peek().type == CHTLJSTokenType::Vir) {
        return parseVirDeclaration();
    }
    if (peek().type == CHTLJSTokenType::FileLoader) {
        return parseFileLoaderBlock();
    }
    // Default to an expression statement
    return parseExpressionStatement();
}

CHTLJSNodePtr CHTLJSParser::parseExpressionStatement() {
    CHTLJSNodePtr expr = parseExpression();
    consume(CHTLJSTokenType::Semicolon, "Expect ';' after expression.");
    return expr;
}

CHTLJSNodePtr CHTLJSParser::parseExpression() {
    CHTLJSNodePtr expr = parsePrimary();

    while (match({CHTLJSTokenType::Dot, CHTLJSTokenType::Arrow})) {
        expr = parseMethodCall(std::move(expr));
    }

    return expr;
}

CHTLJSNodePtr CHTLJSParser::parsePrimary() {
    if (match({CHTLJSTokenType::Animate})) {
        return parseAnimateBlock();
    }
    if (match({CHTLJSTokenType::DoubleCurlyOpen})) {
        std::string selector_str;
        while (peek().type != CHTLJSTokenType::DoubleCurlyClose && !isAtEnd()) {
            selector_str += advance().value;
        }
        consume(CHTLJSTokenType::DoubleCurlyClose, "Expect '}}' after selector.");
        return std::make_unique<EnhancedSelectorNode>(selector_str);
    }
    throw error(peek(), "Expect expression.");
}

CHTLJSNodePtr CHTLJSParser::parseMethodCall(CHTLJSNodePtr callee) {
    const CHTLJSToken& methodName = consume(CHTLJSTokenType::Identifier, "Expect method name after '->'.");

    CHTLJSNodePtr arguments;
    if (methodName.value == "listen") {
        arguments = parseListenBlock();
    } else if (methodName.value == "delegate") {
        arguments = parseDelegateBlock();
    }
    else {
        throw error(methodName, "Unsupported method call.");
    }

    return std::make_unique<MethodCallNode>(std::move(callee), methodName.value, std::move(arguments));
}


CHTLJSNodePtr CHTLJSParser::parseVirDeclaration() {
    consume(CHTLJSTokenType::Vir, "Expect 'vir' keyword.");
    const CHTLJSToken& name = consume(CHTLJSTokenType::Identifier, "Expect variable name.");
    consume(CHTLJSTokenType::Equals, "Expect '=' after variable name.");

    CHTLJSNodePtr value = parseExpression();

    consume(CHTLJSTokenType::Semicolon, "Expect ';' after variable declaration.");
    return std::make_unique<VirDeclNode>(name.value, std::move(value));
}

CHTLJSNodePtr CHTLJSParser::parseListenBlock() {
    consume(CHTLJSTokenType::Listen, "Expect 'listen' keyword.");
    consume(CHTLJSTokenType::OpenBrace, "Expect '{' after 'listen'.");

    auto listenNode = std::make_unique<ListenNode>();

    while (peek().type != CHTLJSTokenType::CloseBrace && !isAtEnd()) {
        const CHTLJSToken& key = consume(CHTLJSTokenType::Identifier, "Expect event name (e.g., 'click').");
        consume(CHTLJSTokenType::Colon, "Expect ':' after event name.");

        std::string valueStr = consumeValue();
        listenNode->eventHandlers.push_back(std::make_unique<CHTLJSPropertyNode>(key.value, valueStr));

        if (peek().type == CHTLJSTokenType::Comma) {
            advance();
        }
    }

    consume(CHTLJSTokenType::CloseBrace, "Expect '}' after listen block.");
    return listenNode;
}

CHTLJSNodePtr CHTLJSParser::parseDelegateBlock() {
    consume(CHTLJSTokenType::Delegate, "Expect 'delegate' keyword.");
    consume(CHTLJSTokenType::OpenBrace, "Expect '{' after 'delegate'.");

    auto delegateNode = std::make_unique<DelegateNode>();

    while (peek().type != CHTLJSTokenType::CloseBrace && !isAtEnd()) {
        const CHTLJSToken& key = consume(CHTLJSTokenType::Identifier, "Expect property name (e.g., 'target', 'click').");
        consume(CHTLJSTokenType::Colon, "Expect ':' after property name.");

        std::string valueStr = consumeValue();
        delegateNode->properties.push_back(std::make_unique<CHTLJSPropertyNode>(key.value, valueStr));

        if (peek().type == CHTLJSTokenType::Comma) {
            advance();
        }
    }

    consume(CHTLJSTokenType::CloseBrace, "Expect '}' after delegate block.");
    return delegateNode;
}

CHTLJSNodePtr CHTLJSParser::parseAnimateBlock() {
    consume(CHTLJSTokenType::Animate, "Expect 'animate' keyword.");
    consume(CHTLJSTokenType::OpenBrace, "Expect '{' after 'animate'.");

    auto animateNode = std::make_unique<AnimateNode>();

    while (peek().type != CHTLJSTokenType::CloseBrace && !isAtEnd()) {
        const CHTLJSToken& key = consume(CHTLJSTokenType::Identifier, "Expect property name.");
        consume(CHTLJSTokenType::Colon, "Expect ':' after property name.");

        std::string valueStr = consumeValue();
        animateNode->properties.push_back(std::make_unique<CHTLJSPropertyNode>(key.value, valueStr));

        if (peek().type == CHTLJSTokenType::Comma) {
            advance();
        }
    }

    consume(CHTLJSTokenType::CloseBrace, "Expect '}' after animate block.");
    return animateNode;
}

CHTLJSNodePtr CHTLJSParser::parseFileLoaderBlock() {
    consume(CHTLJSTokenType::FileLoader, "Expect 'fileloader' keyword.");
    consume(CHTLJSTokenType::OpenBrace, "Expect '{' after 'fileloader'.");

    auto fileLoaderNode = std::make_unique<FileLoaderNode>();

    while (peek().type != CHTLJSTokenType::CloseBrace && !isAtEnd()) {
        const CHTLJSToken& key = consume(CHTLJSTokenType::Identifier, "Expect property name.");
        if (key.value != "load") {
            throw error(key, "Only 'load' property is allowed in fileloader block.");
        }
        consume(CHTLJSTokenType::Colon, "Expect ':' after 'load' property.");

        do {
            const CHTLJSToken& path = consume(CHTLJSTokenType::Identifier, "Expect file path.");
            fileLoaderNode->files.push_back(path.value);
        } while (match({CHTLJSTokenType::Comma}));

        if (peek().type != CHTLJSTokenType::CloseBrace) {
             consume(CHTLJSTokenType::Semicolon, "Expect ';' or '}' after file path(s).");
        }
    }

    consume(CHTLJSTokenType::CloseBrace, "Expect '}' after fileloader block.");
    return fileLoaderNode;
}


// --- Helper Methods ---

std::string CHTLJSParser::consumeValue() {
    std::string valueStr;
    int brace_level = 0;
    int bracket_level = 0;

    if (peek().type == CHTLJSTokenType::OpenBrace || peek().type == CHTLJSTokenType::OpenBracket) {
        do {
            if (peek().type == CHTLJSTokenType::OpenBrace) brace_level++;
            else if (peek().type == CHTLJSTokenType::CloseBrace) brace_level--;
            else if (peek().type == CHTLJSTokenType::OpenBracket) bracket_level++;
            else if (peek().type == CHTLJSTokenType::CloseBracket) bracket_level--;

            valueStr += advance().value + " ";

        } while ((brace_level > 0 || bracket_level > 0) && !isAtEnd());
    } else {
        while (!isAtEnd()) {
            if (peek().type == CHTLJSTokenType::Comma) break;
            if (peek().type == CHTLJSTokenType::CloseBrace) break;
            valueStr += advance().value + " ";
        }
    }
    return valueStr;
}


const CHTLJSToken& CHTLJSParser::peek() const {
    return tokens[current];
}

const CHTLJSToken& CHTLJSParser::advance() {
    if (!isAtEnd()) current++;
    return tokens[current - 1];
}

bool CHTLJSParser::isAtEnd() const {
    return peek().type == CHTLJSTokenType::EndOfFile;
}

const CHTLJSToken& CHTLJSParser::consume(CHTLJSTokenType type, const std::string& message) {
    if (peek().type == type) return advance();
    throw error(peek(), message);
}

bool CHTLJSParser::match(const std::vector<CHTLJSTokenType>& types) {
    for (const auto& type : types) {
        if (peek().type == type) {
            advance();
            return true;
        }
    }
    return false;
}

CHTLJSParser::ParseError CHTLJSParser::error(const CHTLJSToken& token, const std::string& message) {
    return ParseError("CHTL JS Parse Error: " + message);
}
