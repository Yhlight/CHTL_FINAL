#include "Parser.h"
#include <sstream>

Parser::Parser(std::vector<Token> tokens) : m_tokens(std::move(tokens)), m_position(0) {
    // Initialize current and peek tokens
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    m_currentToken = m_peekToken;
    if (m_position < m_tokens.size()) {
        m_peekToken = m_tokens[m_position];
    } else {
        // When we run out of tokens, peek token becomes EOF
        m_peekToken = Token{TokenType::END_OF_FILE, "", m_currentToken.line};
    }
    m_position++;
}

bool Parser::currentTokenIs(TokenType t) const {
    return m_currentToken.type == t;
}

bool Parser::peekTokenIs(TokenType t) const {
    return m_peekToken.type == t;
}

void Parser::peekError(TokenType t) {
    std::stringstream ss;
    ss << "Line " << m_peekToken.line << ": Expected next token to be "
       << Token{t, "", 0}.typeToString() << ", got "
       << m_peekToken.typeToString() << " instead.";
    m_errors.push_back(ss.str());
}

bool Parser::expectPeek(TokenType t) {
    if (peekTokenIs(t)) {
        nextToken();
        return true;
    } else {
        peekError(t);
        return false;
    }
}

const std::vector<std::string>& Parser::getErrors() const {
    return m_errors;
}

std::vector<std::unique_ptr<BaseNode>> Parser::parseProgram() {
    std::vector<std::unique_ptr<BaseNode>> program;
    while (!currentTokenIs(TokenType::END_OF_FILE)) {
        auto stmt = parseStatement();
        if (stmt) {
            program.push_back(std::move(stmt));
        }
        nextToken();
    }
    return program;
}

std::unique_ptr<BaseNode> Parser::parseStatement() {
    if (currentTokenIs(TokenType::KEYWORD_TEXT)) {
        return parseTextElement();
    } else if (currentTokenIs(TokenType::IDENTIFIER)) {
        // Could be an element or an unquoted text literal inside a text block.
        // For now, we assume it's an element.
        return parseElement();
    } else if (currentTokenIs(TokenType::SINGLE_LINE_COMMENT) ||
               currentTokenIs(TokenType::MULTI_LINE_COMMENT) ||
               currentTokenIs(TokenType::GENERATOR_COMMENT)) {
        return parseComment();
    }
    // Skip illegal tokens
    return nullptr;
}

std::unique_ptr<AttributeNode> Parser::parseAttribute() {
    Token identToken = m_currentToken; // The attribute name

    if (!expectPeek(TokenType::COLON) && !expectPeek(TokenType::EQUALS)) {
        // CHTL allows both ':' and '=' for attributes.
        // If neither is found, it's a syntax error. We might have already logged a peekError.
        return nullptr;
    }
    nextToken(); // Consume the colon/equals, move to the value

    Token valueToken = m_currentToken;
    auto attribute = std::make_unique<AttributeNode>(identToken, identToken.lexeme, valueToken.lexeme);

    if (!expectPeek(TokenType::SEMICOLON)) {
        return nullptr;
    }

    return attribute;
}

std::unique_ptr<ElementNode> Parser::parseElement() {
    auto element = std::make_unique<ElementNode>(m_currentToken, m_currentToken.lexeme);

    if (!expectPeek(TokenType::LEFT_BRACE)) {
        return nullptr;
    }
    nextToken(); // consume '{'

    while (!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        if (currentTokenIs(TokenType::IDENTIFIER) && (peekTokenIs(TokenType::COLON) || peekTokenIs(TokenType::EQUALS))) {
            // This is an attribute
            auto attr = parseAttribute();
            if (attr) {
                element->addAttribute(std::move(attr));
            }
        } else {
            // This is a child element/statement
            auto child = parseStatement();
            if (child) {
                element->addChild(std::move(child));
            }
        }
        nextToken();
    }

    if (!currentTokenIs(TokenType::RIGHT_BRACE)) {
         // Missing closing brace, error should be logged by caller if needed
        return nullptr;
    }

    return element;
}


std::unique_ptr<TextNode> Parser::parseTextElement() {
    Token textKeywordToken = m_currentToken;

    if (!expectPeek(TokenType::LEFT_BRACE)) {
        return nullptr;
    }
    nextToken(); // consume '{'

    // The content of the text block is the next token
    Token contentToken = m_currentToken;
    auto textNode = std::make_unique<TextNode>(contentToken, contentToken.lexeme);

    if (!expectPeek(TokenType::RIGHT_BRACE)) {
        return nullptr;
    }

    return textNode;
}

std::unique_ptr<CommentNode> Parser::parseComment() {
    return std::make_unique<CommentNode>(m_currentToken, m_currentToken.lexeme);
}
