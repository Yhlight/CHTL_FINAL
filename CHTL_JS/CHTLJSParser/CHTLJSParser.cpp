#include "CHTLJSParser.h"
#include "../CHTLJSNode/CHTLJSNode.h" // For ListenNode, SelectorNode
#include <stdexcept>

CHTLJSParser::CHTLJSParser(CHTLJSLexer& lexer) : lexer(lexer) {
    advance();
}

void CHTLJSParser::advance() {
    previousToken = currentToken;
    currentToken = lexer.nextToken();
}

bool CHTLJSParser::match(CHTLJSTokenType type) {
    if (currentToken.type == type) {
        advance();
        return true;
    }
    return false;
}

void CHTLJSParser::consume(CHTLJSTokenType type, const std::string& message) {
    if (currentToken.type == type) {
        advance();
        return;
    }
    throw std::runtime_error("CHTLJS Parser Error: " + message);
}

std::vector<CHTLJSNodePtr> CHTLJSParser::parse() {
    std::vector<CHTLJSNodePtr> statements;
    while (currentToken.type != CHTLJSTokenType::TOKEN_EOF) {
        statements.push_back(parseStatement());
    }
    return statements;
}

CHTLJSNodePtr CHTLJSParser::parseStatement() {
    if (currentToken.type == CHTLJSTokenType::TOKEN_DOUBLE_LBRACE) {
        return parseListenStatement();
    }
    // For now, only listen statements are supported.
    throw std::runtime_error("Unexpected token at start of CHTL JS statement.");
}

CHTLJSNodePtr CHTLJSParser::parseSelector() {
    consume(CHTLJSTokenType::TOKEN_DOUBLE_LBRACE, "Expect '{{' to start selector.");
    consume(CHTLJSTokenType::TOKEN_IDENTIFIER, "Expect selector identifier.");
    auto node = std::make_unique<SelectorNode>(previousToken.lexeme);
    consume(CHTLJSTokenType::TOKEN_DOUBLE_RBRACE, "Expect '}}' to end selector.");
    return node;
}

CHTLJSNodePtr CHTLJSParser::parseListenStatement() {
    auto node = std::make_unique<ListenNode>();
    node->target = parseSelector();

    consume(CHTLJSTokenType::TOKEN_ARROW, "Expect '->' after selector.");
    consume(CHTLJSTokenType::TOKEN_IDENTIFIER, "Expect 'listen' keyword."); // Should check if lexeme is "listen"

    consume(CHTLJSTokenType::TOKEN_LBRACE, "Expect '{' to start listen block.");

    // For now, parse the body as a raw string until the matching brace.
    // This is a simplification.
    std::string body_content;
    int brace_level = 1;
    while(brace_level > 0 && currentToken.type != CHTLJSTokenType::TOKEN_EOF) {
        if (currentToken.type == CHTLJSTokenType::TOKEN_LBRACE) brace_level++;
        if (currentToken.type == CHTLJSTokenType::TOKEN_RBRACE) brace_level--;

        if (brace_level > 0) {
            body_content += currentToken.lexeme + " ";
        }
        advance();
    }

    node->body = body_content;
    return node;
}
