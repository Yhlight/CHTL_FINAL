#include "CHTLParser.h"
#include <memory>

namespace CHTL {

CHTLParser::CHTLParser(CHTLLexer& lexer) : m_lexer(lexer) {
    // Prime the parser with the first two tokens
    nextToken();
    nextToken();
}

void CHTLParser::nextToken() {
    m_curToken = m_peekToken;
    m_peekToken = m_lexer.NextToken();
}

void CHTLParser::peekError(TokenType expected) {
    std::string msg = "Error: Expected next token to be " + tokenTypeToString(expected) +
                      ", got " + tokenTypeToString(m_peekToken.type) + " instead.";
    m_errors.push_back(msg);
}

bool CHTLParser::expectPeek(TokenType expected) {
    if (m_peekToken.type == expected) {
        nextToken();
        return true;
    } else {
        peekError(expected);
        return false;
    }
}

std::unique_ptr<Program> CHTLParser::ParseProgram() {
    auto program = std::make_unique<Program>();

    while (m_curToken.type != TokenType::TOKEN_EOF) {
        auto stmt = parseStatement();
        if (stmt) {
            program->statements.push_back(stmt);
        }
        nextToken();
    }
    return program;
}

std::shared_ptr<Statement> CHTLParser::parseStatement() {
    switch (m_curToken.type) {
        case TokenType::TOKEN_IDENTIFIER:
            return parseElementStatement();
        case TokenType::TOKEN_KEYWORD_TEXT:
            return parseTextStatement();
        default:
            // Handle other statement types or return null for now
            return nullptr;
    }
}

std::shared_ptr<ElementNode> CHTLParser::parseElementStatement() {
    auto elementNode = std::make_shared<ElementNode>(m_curToken);

    if (!expectPeek(TokenType::TOKEN_LBRACE)) {
        return nullptr;
    }
    nextToken(); // Consume '{'

    // Parse child statements
    while (m_curToken.type != TokenType::TOKEN_RBRACE && m_curToken.type != TokenType::TOKEN_EOF) {
        auto stmt = parseStatement();
        if (stmt) {
            elementNode->children.push_back(stmt);
        }
        nextToken();
    }

    if (m_curToken.type != TokenType::TOKEN_RBRACE) {
        // Error: unclosed element
        m_errors.push_back("Error: Unterminated element '" + elementNode->GetTokenLiteral() + "'. Expected '}'.");
        return nullptr;
    }

    return elementNode;
}

std::shared_ptr<TextNode> CHTLParser::parseTextStatement() {
    if (!expectPeek(TokenType::TOKEN_LBRACE)) {
        return nullptr;
    }

    if (!expectPeek(TokenType::TOKEN_STRING)) {
        return nullptr;
    }

    auto textNode = std::make_shared<TextNode>(m_curToken);

    if (!expectPeek(TokenType::TOKEN_RBRACE)) {
        return nullptr;
    }

    return textNode;
}

} // namespace CHTL
