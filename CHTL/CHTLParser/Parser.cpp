#include "Parser.h"
#include <iostream>
#include "../../Util/StringUtil/StringUtil.h"

namespace CHTL {

Parser::Parser(Lexer& l) : m_lexer(l) {
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
    }
    peekError(t);
    return false;
}

std::unique_ptr<RootNode> Parser::parseProgram() {
    auto program = std::make_unique<RootNode>();
    while (!currentTokenIs(TokenType::END_OF_FILE)) {
        auto stmt = parseStatement(program.get());
        if (stmt) {
            program->addChild(std::move(stmt));
        }
        nextToken();
    }
    return program;
}

std::unique_ptr<Node> Parser::parseStatement(Node* parent) {
    if (currentTokenIs(TokenType::IDENTIFIER)) {
        return parseElementStatement(parent);
    } else if (currentTokenIs(TokenType::KEYWORD_TEXT)) {
        return parseTextStatement();
    } else if (currentTokenIs(TokenType::KEYWORD_STYLE)) {
        return parseStyleNodeStatement();
    }
    return nullptr;
}

std::unique_ptr<ElementNode> Parser::parseElementStatement(Node* parent) {
    auto element = std::make_unique<ElementNode>(m_currentToken.literal);
    if (!expectPeek(TokenType::LEFT_BRACE)) return nullptr;

    while (!peekTokenIs(TokenType::RIGHT_BRACE) && !peekTokenIs(TokenType::END_OF_FILE)) {
        nextToken();
        if (currentTokenIs(TokenType::RIGHT_BRACE)) break;

        if (currentTokenIs(TokenType::IDENTIFIER) && peekTokenIs(TokenType::COLON)) {
            parseAttributes(element.get());
        } else {
             auto stmt = parseStatement(element.get());
             if (stmt) {
                 element->addChild(std::move(stmt));
             }
        }
    }

    if (!expectPeek(TokenType::RIGHT_BRACE)) return nullptr;
    return element;
}

void Parser::parseAttributes(ElementNode* element) {
    std::string key = m_currentToken.literal;
    nextToken(); // consume IDENTIFIER
    nextToken(); // consume COLON

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
    nextToken();

    if (!currentTokenIs(TokenType::STRING) && !currentTokenIs(TokenType::IDENTIFIER)) {
        m_errors.push_back("Error: text block must contain a string or unquoted literal.");
        return nullptr;
    }

    auto textNode = std::make_unique<TextNode>(m_currentToken.literal);
    if (!expectPeek(TokenType::RIGHT_BRACE)) return nullptr;
    return textNode;
}

std::unique_ptr<StyleNode> Parser::parseStyleNodeStatement() {
    auto styleNode = std::make_unique<StyleNode>();
    if (!expectPeek(TokenType::LEFT_BRACE)) return nullptr;

    while (!peekTokenIs(TokenType::RIGHT_BRACE) && !peekTokenIs(TokenType::END_OF_FILE)) {
        nextToken();
        if (currentTokenIs(TokenType::RIGHT_BRACE)) break;

        if (currentTokenIs(TokenType::DOT) || currentTokenIs(TokenType::HASH) || currentTokenIs(TokenType::AMPERSAND)) {
            auto rule = parseCssRuleNode();
            if(rule) styleNode->addChild(std::move(rule));
        } else if (currentTokenIs(TokenType::IDENTIFIER) && peekTokenIs(TokenType::COLON)) {
            std::string key = m_currentToken.literal;
            nextToken(); // consume IDENTIFIER
            nextToken(); // consume COLON

            std::string value;
            while(!currentTokenIs(TokenType::SEMICOLON) && !currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
                if (m_currentToken.type == TokenType::STRING) {
                    value += "\"" + m_currentToken.literal + "\"";
                } else {
                    value += m_currentToken.literal;
                }
                if (!peekTokenIs(TokenType::SEMICOLON) && !peekTokenIs(TokenType::RIGHT_BRACE)) value += " ";
                nextToken();
            }
            StringUtil::Trim(value);
            styleNode->setInlineStyle(key, value);
        } else {
            m_errors.push_back("Error: Invalid statement in style block.");
            while(!currentTokenIs(TokenType::SEMICOLON) && !currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
                nextToken();
            }
        }
    }

    if (!expectPeek(TokenType::RIGHT_BRACE)) return nullptr;
    return styleNode;
}

std::unique_ptr<CssRuleNode> Parser::parseCssRuleNode() {
    std::string selector;
    while (!currentTokenIs(TokenType::LEFT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        selector += m_currentToken.literal;
        nextToken();
    }
    StringUtil::Trim(selector);

    auto ruleNode = std::make_unique<CssRuleNode>(selector);
    if (!currentTokenIs(TokenType::LEFT_BRACE)) return nullptr;

    while(!peekTokenIs(TokenType::RIGHT_BRACE) && !peekTokenIs(TokenType::END_OF_FILE)) {
        nextToken();
        if (currentTokenIs(TokenType::RIGHT_BRACE)) break;

        if(currentTokenIs(TokenType::IDENTIFIER) && peekTokenIs(TokenType::COLON)) {
            std::string key = m_currentToken.literal;
            nextToken();
            nextToken();

            std::string value;
            while(!currentTokenIs(TokenType::SEMICOLON) && !currentTokenIs(TokenType::RIGHT_BRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
                if (m_currentToken.type == TokenType::STRING) {
                    value += "\"" + m_currentToken.literal + "\"";
                } else {
                    value += m_currentToken.literal;
                }
                if (!peekTokenIs(TokenType::SEMICOLON) && !peekTokenIs(TokenType::RIGHT_BRACE)) value += " ";
                nextToken();
            }
            StringUtil::Trim(value);
            ruleNode->setProperty(key, value);
        }
    }

    if (!expectPeek(TokenType::RIGHT_BRACE)) return nullptr;
    return ruleNode;
}

} // namespace CHTL
