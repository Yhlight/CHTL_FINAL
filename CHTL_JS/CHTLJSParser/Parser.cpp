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
    // Add other statement types here later
    throw error(peek(), "Expect a statement (e.g., 'vir').");
}

CHTLJSNodePtr CHTLJSParser::parseVirDeclaration() {
    consume(CHTLJSTokenType::Vir, "Expect 'vir' keyword.");
    const CHTLJSToken& name = consume(CHTLJSTokenType::Identifier, "Expect variable name.");
    consume(CHTLJSTokenType::Equals, "Expect '=' after variable name.");

    CHTLJSNodePtr value;
    if (peek().type == CHTLJSTokenType::Listen) {
        value = parseListenBlock();
    } else {
        throw error(peek(), "Unsupported value for 'vir' declaration.");
    }

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

        // For now, we treat the value (the JS function) as an opaque block.
        // We will consume tokens until we hit a comma or the closing brace.
        // This is a simplification until the Unified Scanner is implemented.
        std::string valueStr;
        int brace_level = 0;
        while (!isAtEnd()) {
            if (peek().type == CHTLJSTokenType::Comma && brace_level == 0) break;
            if (peek().type == CHTLJSTokenType::CloseBrace && brace_level == 0) break;

            if (peek().type == CHTLJSTokenType::OpenBrace) brace_level++;
            if (peek().type == CHTLJSTokenType::CloseBrace) brace_level--;

            valueStr += advance().value + " ";
        }

        listenNode->eventHandlers.push_back(std::make_unique<CHTLJSPropertyNode>(key.value, valueStr));

        if (peek().type == CHTLJSTokenType::Comma) {
            advance(); // Consume the comma
        }
    }

    consume(CHTLJSTokenType::CloseBrace, "Expect '}' after listen block.");
    return listenNode;
}


// --- Helper Methods ---

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
