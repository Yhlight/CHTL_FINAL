#include "Parser.h"
#include <sstream>

namespace CHTL {

Parser::Parser(Lexer& lexer) : m_lexer(lexer) {
    // Initialize with two tokens to populate curToken and peekToken
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    m_curToken = m_peekToken;
    m_peekToken = m_lexer.nextToken();
}

bool Parser::currentTokenIs(TokenType type) const {
    return m_curToken.type == type;
}

bool Parser::peekTokenIs(TokenType type) const {
    return m_peekToken.type == type;
}

bool Parser::expectPeek(TokenType type) {
    if (peekTokenIs(type)) {
        nextToken();
        return true;
    } else {
        std::stringstream ss;
        ss << "Error: Expected next token to be " << TokenTypeToString(type)
           << ", but got " << TokenTypeToString(m_peekToken.type) << " instead.";
        m_errors.push_back(ss.str());
        return false;
    }
}

std::unique_ptr<ProgramNode> Parser::parseProgram() {
    auto program = std::make_unique<ProgramNode>();
    while (!currentTokenIs(TokenType::END_OF_FILE)) {
        auto stmt = parseStatement();
        if (stmt) {
            program->children.push_back(stmt);
        }
        nextToken();
    }
    return program;
}

std::shared_ptr<Node> Parser::parseStatement() {
    if (currentTokenIs(TokenType::IDENTIFIER)) {
        // Could be an element or a top-level text/style block.
        // For now, we only support elements at the top level.
        if (m_curToken.literal == "text") {
            return parseTextNode();
        }
        return parseElement();
    } else if (currentTokenIs(TokenType::COMMENT_GEN)) {
        return parseCommentNode();
    }
    return nullptr;
}

std::shared_ptr<ElementNode> Parser::parseElement() {
    auto element = std::make_shared<ElementNode>();
    element->tagName = m_curToken.literal;

    if (!expectPeek(TokenType::LBRACE)) {
        return nullptr;
    }

    nextToken(); // Consume '{'

    while (!currentTokenIs(TokenType::RBRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        if (currentTokenIs(TokenType::IDENTIFIER)) {
            if (peekTokenIs(TokenType::COLON) || peekTokenIs(TokenType::EQUALS)) {
                auto attr = parseAttribute();
                if (attr) {
                    element->attributes.push_back(attr);
                }
            } else if (m_curToken.literal == "text") {
                 auto text = parseTextNode();
                 if (text) {
                     element->children.push_back(text);
                 }
            } else {
                auto child = parseElement();
                if (child) {
                    element->children.push_back(child);
                }
            }
        } else if (currentTokenIs(TokenType::COMMENT_GEN)) {
            auto comment = parseCommentNode();
            if (comment) {
                element->children.push_back(comment);
            }
        }
        nextToken();
    }

    if (!currentTokenIs(TokenType::RBRACE)) {
        m_errors.push_back("Error: Unterminated element block, expected '}'.");
        return nullptr;
    }

    return element;
}

std::shared_ptr<AttributeNode> Parser::parseAttribute() {
    auto attribute = std::make_shared<AttributeNode>();
    attribute->name = m_curToken.literal;

    nextToken(); // consume identifier
    if (!currentTokenIs(TokenType::COLON) && !currentTokenIs(TokenType::EQUALS)) {
        m_errors.push_back("Error: Expected ':' or '=' after attribute name.");
        return nullptr;
    }

    nextToken(); // consume ':' or '='

    attribute->value = m_curToken.literal;

    // An unquoted literal might be tokenized as multiple identifiers.
    // This is a simplification for now. The parser will need to be smarter.
    if (!peekTokenIs(TokenType::SEMICOLON)) {
        // Could handle multi-word unquoted literals here if needed in the future.
    } else {
       nextToken(); // consume semicolon
    }


    return attribute;
}

std::shared_ptr<TextNode> Parser::parseTextNode() {
    auto textNode = std::make_shared<TextNode>();

    if (!expectPeek(TokenType::LBRACE)) {
        return nullptr;
    }
    nextToken(); // Consume '{'

    std::stringstream content;
    while (!currentTokenIs(TokenType::RBRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        content << m_curToken.literal;
        if (!peekTokenIs(TokenType::RBRACE)) {
            content << " "; // Add space between joined tokens
        }
        nextToken();
    }

    textNode->content = content.str();

    if (!currentTokenIs(TokenType::RBRACE)) {
        m_errors.push_back("Error: Unterminated text block, expected '}'.");
        return nullptr;
    }

    return textNode;
}

std::shared_ptr<CommentNode> Parser::parseCommentNode() {
    auto commentNode = std::make_shared<CommentNode>();
    commentNode->text = m_curToken.literal;
    return commentNode;
}

} // namespace CHTL
