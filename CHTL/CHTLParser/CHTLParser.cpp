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
    if (peekTokenIs(expected)) {
        nextToken();
        return true;
    } else {
        peekError(expected);
        return false;
    }
}

bool CHTLParser::curTokenIs(TokenType type) const {
    return m_curToken.type == type;
}

bool CHTLParser::peekTokenIs(TokenType type) const {
    return m_peekToken.type == type;
}

std::unique_ptr<Program> CHTLParser::ParseProgram() {
    auto program = std::make_unique<Program>();

    while (!curTokenIs(TokenType::TOKEN_EOF)) {
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
            if (peekTokenIs(TokenType::TOKEN_COLON)) {
                return parseAttributeStatement();
            } else if (peekTokenIs(TokenType::TOKEN_LBRACE)) {
                return parseElementStatement();
            }
            break;
        case TokenType::TOKEN_KEYWORD_TEXT:
            return parseTextStatement();
        case TokenType::TOKEN_KEYWORD_STYLE:
            return parseStyleStatement();
        default:
            return nullptr;
    }
    return nullptr;
}

std::shared_ptr<Expression> CHTLParser::parseExpression() {
    switch (m_curToken.type) {
        case TokenType::TOKEN_IDENTIFIER:
            return std::make_shared<Identifier>(m_curToken);
        case TokenType::TOKEN_STRING:
            return std::make_shared<StringLiteral>(m_curToken);
        // Add other expression types here (numbers, etc.)
        default:
            return nullptr;
    }
}


std::shared_ptr<AttributeNode> CHTLParser::parseAttributeStatement() {
    Token keyToken = m_curToken;

    if (!expectPeek(TokenType::TOKEN_COLON)) {
        return nullptr;
    }
    nextToken(); // consume ':'

    auto value = parseExpression();
    if (!value) {
        return nullptr;
    }

    auto attrNode = std::make_shared<AttributeNode>(keyToken, value);

    // Optional semicolon
    if (peekTokenIs(TokenType::TOKEN_SEMICOLON)) {
        nextToken();
    }

    return attrNode;
}

std::shared_ptr<StyleNode> CHTLParser::parseStyleStatement() {
    auto styleNode = std::make_shared<StyleNode>(m_curToken);

    if (!expectPeek(TokenType::TOKEN_LBRACE)) {
        return nullptr;
    }
    nextToken(); // consume '{'

    while (!curTokenIs(TokenType::TOKEN_RBRACE) && !curTokenIs(TokenType::TOKEN_EOF)) {
        if (m_curToken.type == TokenType::TOKEN_IDENTIFIER) {
            auto prop = parseAttributeStatement();
            if (prop) {
                styleNode->properties.push_back(prop);
            }
        }
        nextToken();
    }

    if (!curTokenIs(TokenType::TOKEN_RBRACE)) {
        m_errors.push_back("Error: Unterminated style block. Expected '}'.");
        return nullptr;
    }

    return styleNode;
}


std::shared_ptr<ElementNode> CHTLParser::parseElementStatement() {
    auto elementNode = std::make_shared<ElementNode>(m_curToken);

    if (!expectPeek(TokenType::TOKEN_LBRACE)) {
        return nullptr;
    }
    nextToken(); // Consume '{'

    // Parse child statements
    while (!curTokenIs(TokenType::TOKEN_RBRACE) && !curTokenIs(TokenType::TOKEN_EOF)) {
        auto stmt = parseStatement();
        if (stmt) {
            elementNode->children.push_back(stmt);
        }
        nextToken();
    }

    if (!curTokenIs(TokenType::TOKEN_RBRACE)) {
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
