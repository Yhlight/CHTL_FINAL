#include "Parser.h"
#include <iostream>
#include "../../Util/StringUtil/StringUtil.h"

namespace CHTL {

Parser::Parser(Lexer& l) : m_lexer(l) {
    // Initialize the token stream by getting the first two tokens
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    m_currentToken = m_peekToken;
    m_peekToken = m_lexer.nextToken();
}

const std::vector<std::string>& Parser::getErrors() const {
    return m_errors;
}

void Parser::peekError(TokenType t) {
    std::string error_msg = "Error: Expected next token to be " + token_type_to_string.at(t) +
                           ", got " + token_type_to_string.at(m_peekToken.type) + " instead.";
    m_errors.push_back(error_msg);
    std::cerr << error_msg << std::endl;
}

bool Parser::currentTokenIs(TokenType t) const {
    return m_currentToken.type == t;
}

bool Parser::peekTokenIs(TokenType t) const {
    return m_peekToken.type == t;
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

std::unique_ptr<RootNode> Parser::parseProgram() {
    auto program = std::make_unique<RootNode>();
    parseStatementList(program.get());
    return program;
}

void Parser::parseStatementList(Node* parent) {
    while (!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        auto stmt = parseStatement(parent);
        if (stmt) {
            parent->addChild(std::move(stmt));
        }
        nextToken();
    }
}

std::unique_ptr<Node> Parser::parseStatement(Node* parent) {
    if (currentTokenIs(TokenType::IDENTIFIER)) {
        return parseElementStatement(parent);
    } else if (currentTokenIs(TokenType::KEYWORD_TEXT)) {
        return parseTextStatement();
    } else if (currentTokenIs(TokenType::KEYWORD_STYLE)) {
        // Style statements modify their parent, they don't produce a new node.
        if (auto element_parent = dynamic_cast<ElementNode*>(parent)) {
            parseStyleStatement(element_parent);
        } else {
            m_errors.push_back("Error: Style block can only be a child of an element.");
        }
        return nullptr;
    }
    return nullptr;
}

std::unique_ptr<ElementNode> Parser::parseElementStatement(Node* parent) {
    auto element = std::make_unique<ElementNode>(m_currentToken.literal);

    if (!expectPeek(TokenType::LEFT_BRACE)) {
        return nullptr;
    }
    nextToken(); // Consume '{'

    // In here, we can have attributes or child elements/statements.
    while (!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        if (currentTokenIs(TokenType::IDENTIFIER) && peekTokenIs(TokenType::COLON)) {
            parseAttributes(element.get());
        } else {
             auto stmt = parseStatement(element.get());
             if (stmt) {
                 element->addChild(std::move(stmt));
             }
        }
        nextToken();
    }

    return element;
}

void Parser::parseAttributes(ElementNode* element) {
    std::string key = m_currentToken.literal;

    if (!expectPeek(TokenType::COLON)) return;
    nextToken(); // consume ':'

    if (currentTokenIs(TokenType::STRING) || currentTokenIs(TokenType::IDENTIFIER)) {
        element->setAttribute(key, m_currentToken.literal);
    } else {
        m_errors.push_back("Error: Invalid attribute value for key '" + key + "'.");
    }

    if (peekTokenIs(TokenType::SEMICOLON)) {
        nextToken();
    }
}

std::unique_ptr<TextNode> Parser::parseTextStatement() {
    if (!expectPeek(TokenType::LEFT_BRACE)) return nullptr;
    nextToken(); // consume '{'

    if (!currentTokenIs(TokenType::STRING) && !currentTokenIs(TokenType::IDENTIFIER)) {
        m_errors.push_back("Error: text block must contain a string or unquoted literal.");
        return nullptr;
    }

    auto textNode = std::make_unique<TextNode>(m_currentToken.literal);

    if (!expectPeek(TokenType::RIGHT_BRACE)) return nullptr;

    return textNode;
}

void Parser::parseStyleStatement(ElementNode* owner) {
    if (!expectPeek(TokenType::LEFT_BRACE)) return;
    nextToken(); // consume '{'

    while (!currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        if (currentTokenIs(TokenType::IDENTIFIER) && peekTokenIs(TokenType::COLON)) {
            std::string key = m_currentToken.literal;
            nextToken(); // consume identifier
            nextToken(); // consume ':'

            std::string value;
            // CHTL allows complex unquoted values, e.g., `border: 1px solid black;`
            // We'll consume tokens until we hit a semicolon.
            while(!currentTokenIs(TokenType::SEMICOLON) && !currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
                value += m_currentToken.literal;
                if(peekTokenIs(TokenType::SEMICOLON) || peekTokenIs(TokenType::RIGHT_BRACE)) {
                    // don't add space at the end
                } else {
                    value += " ";
                }
                nextToken();
            }
            StringUtil::Trim(value);
            owner->setStyle(key, value);

            // current token is now semicolon, loop will advance past it.
        } else {
            // Invalid line in style block, skip to next semicolon or brace.
            m_errors.push_back("Error: Invalid statement in style block.");
            while(!currentTokenIs(TokenType::SEMICOLON) && !currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
                nextToken();
            }
        }

        if (currentTokenIs(TokenType::SEMICOLON)) {
             nextToken(); // consume optional semicolon
        }
    }
}

} // namespace CHTL
