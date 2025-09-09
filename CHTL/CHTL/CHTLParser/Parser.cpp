#include "Parser.h"
#include <memory>
#include <sstream>
#include <iostream>

namespace CHTL {

Parser::Parser(Lexer& lexer, Context& context) : m_lexer(lexer), m_context(context) {
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
    std::string msg = "expected next token to be " + std::to_string(static_cast<int>(t)) +
                      ", got " + std::to_string(static_cast<int>(m_peekToken.type)) + " instead";
    m_errors.push_back(msg);
}

bool Parser::expectPeek(TokenType t) {
    if (m_peekToken.type == t) {
        nextToken();
        return true;
    } else {
        peekError(t);
        return false;
    }
}

std::unique_ptr<Program> Parser::parseProgram() {
    auto program = std::make_unique<Program>();
    while (m_currentToken.type != TokenType::EOF_TOKEN) {
        auto stmt = parseStatement(true);
        if (stmt) {
            program->statements.push_back(std::move(stmt));
        }
        nextToken();
    }
    return program;
}

std::unique_ptr<Statement> Parser::parseStatement(bool is_global_scope) {
    if (m_currentToken.type == TokenType::LBRACKET) {
        if (m_peekToken.literal == "Template") return parseTemplateNode();
        if (m_peekToken.literal == "Namespace") return parseNamespaceNode();
        if (m_peekToken.literal == "Import") return parseImportNode();
    }

    switch (m_currentToken.type) {
        case TokenType::TYPE_STYLE:
        case TokenType::TYPE_ELEMENT:
        case TokenType::TYPE_VAR:
            return parseTemplateInstantiationNode();
        default:
            break;
    }

    if (m_currentToken.type == TokenType::IDENTIFIER) {
        if (m_currentToken.literal == "text") return parseTextNode();
        if (m_currentToken.literal == "style") {
            return is_global_scope ? std::unique_ptr<Statement>(parseRawBlockNode(RawBlockType::CSS)) : std::unique_ptr<Statement>(parseStyleNode());
        }
        if (m_currentToken.literal == "script") {
             if (is_global_scope) return parseRawBlockNode(RawBlockType::JS);
        }
        return parseElementNode();
    }
    return nullptr;
}

std::unique_ptr<RawBlockNode> Parser::parseRawBlockNode(RawBlockType type) {
    auto node = std::make_unique<RawBlockNode>();
    node->token = m_currentToken;
    node->type = type;

    if (!expectPeek(TokenType::LBRACE)) return nullptr;

    std::stringstream content_stream;
    int brace_count = 1;
    nextToken();

    while (brace_count > 0 && m_currentToken.type != TokenType::EOF_TOKEN) {
        if (m_currentToken.type == TokenType::LBRACE) brace_count++;
        else if (m_currentToken.type == TokenType::RBRACE) brace_count--;

        if (brace_count > 0) {
            content_stream << m_currentToken.literal << " ";
        }
        nextToken();
    }

    node->content = content_stream.str();
    return node;
}


std::unique_ptr<TemplateNode> Parser::parseTemplateNode() {
    auto node = std::make_unique<TemplateNode>();
    node->token = m_currentToken;

    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    if (!expectPeek(TokenType::RBRACKET)) return nullptr;
    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    node->type = m_currentToken;
    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    node->name = m_currentToken;
    if (!expectPeek(TokenType::LBRACE)) return nullptr;

    nextToken();

    while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::EOF_TOKEN) {
        auto stmt = parseStatement(false);
        if (stmt) {
            node->body.push_back(std::move(stmt));
        }
        nextToken();
    }

    auto raw_node_ptr = node.get();
    m_context.addTemplate(raw_node_ptr->name.literal, std::move(node));

    return nullptr;
}

std::unique_ptr<TemplateInstantiationNode> Parser::parseTemplateInstantiationNode() {
    auto node = std::make_unique<TemplateInstantiationNode>();
    node->token = m_currentToken;

    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    node->name = m_currentToken;

    if (m_peekToken.type == TokenType::SEMICOLON) {
        nextToken();
    }
    return node;
}

std::unique_ptr<NamespaceNode> Parser::parseNamespaceNode() {
    auto node = std::make_unique<NamespaceNode>();
    node->token = m_currentToken;

    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    if (!expectPeek(TokenType::RBRACKET)) return nullptr;
    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    node->name = m_currentToken;

    if (m_peekToken.type == TokenType::LBRACE) {
        nextToken();
        while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::EOF_TOKEN) {
            auto stmt = parseStatement(false);
            if (stmt) {
                node->body.push_back(std::move(stmt));
            }
            nextToken();
        }
    }
    return node;
}

std::unique_ptr<ImportNode> Parser::parseImportNode() {
    auto node = std::make_unique<ImportNode>();
    node->token = m_currentToken;

    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    if (!expectPeek(TokenType::RBRACKET)) return nullptr;

    nextToken();
    node->type = m_currentToken;

    if (!expectPeek(TokenType::IDENTIFIER) || m_currentToken.literal != "from") return nullptr;
    if (!expectPeek(TokenType::STRING_LITERAL)) return nullptr;
    node->path = m_currentToken.literal;

    return node;
}

std::unique_ptr<ElementNode> Parser::parseElementNode() {
    auto element = std::make_unique<ElementNode>();
    element->token = m_currentToken;

    if (!expectPeek(TokenType::LBRACE)) return nullptr;

    nextToken();

    while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::EOF_TOKEN) {
        if (m_currentToken.type == TokenType::IDENTIFIER) {
            if (m_peekToken.type == TokenType::COLON || m_peekToken.type == TokenType::EQUALS) {
                auto attr = parseAttributeNode();
                if (attr) element->attributes.push_back(std::move(attr));
            } else {
                 auto child = parseStatement(false);
                 if (child) element->children.push_back(std::move(child));
                 continue;
            }
        }
        nextToken();
    }

    return element;
}

std::unique_ptr<StyleNode> Parser::parseStyleNode() {
    auto styleNode = std::make_unique<StyleNode>();
    styleNode->token = m_currentToken;

    if (!expectPeek(TokenType::LBRACE)) return nullptr;
    nextToken();

    while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::EOF_TOKEN) {
        std::unique_ptr<Statement> stmt = nullptr;

        switch (m_currentToken.type) {
            case TokenType::TYPE_STYLE:
                stmt = parseTemplateInstantiationNode();
                break;
            case TokenType::IDENTIFIER:
                if (m_peekToken.type == TokenType::LBRACE) stmt = parseStyleRuleNode();
                else if (m_peekToken.type == TokenType::COLON || m_peekToken.type == TokenType::EQUALS) {
                    stmt = parseStylePropertyNode();
                }
                break;
            default:
                break;
        }

        if (stmt) styleNode->children.push_back(std::move(stmt));
        nextToken();
    }
    return styleNode;
}

std::unique_ptr<StylePropertyNode> Parser::parseStylePropertyNode() {
    auto prop = std::make_unique<StylePropertyNode>();
    prop->key = m_currentToken;

    if (!expectPeek(TokenType::COLON) && m_currentToken.type != TokenType::EQUALS) return nullptr;
    nextToken();

    prop->value = parseExpression();

    if (m_peekToken.type == TokenType::SEMICOLON) nextToken();
    return prop;
}

std::unique_ptr<StyleRuleNode> Parser::parseStyleRuleNode() {
    auto rule = std::make_unique<StyleRuleNode>();
    rule->selector = m_currentToken;

    if (!expectPeek(TokenType::LBRACE)) return nullptr;
    nextToken();

    while (m_currentToken.type != TokenType::RBRACE && m_currentToken.type != TokenType::EOF_TOKEN) {
        auto prop = parseStylePropertyNode();
        if (prop) rule->properties.push_back(std::move(prop));
        nextToken();
    }
    return rule;
}

std::unique_ptr<TextNode> Parser::parseTextNode() {
    auto textNode = std::make_unique<TextNode>();
    textNode->token = m_currentToken;

    if (!expectPeek(TokenType::LBRACE)) return nullptr;
    nextToken();

    if (m_currentToken.type == TokenType::STRING_LITERAL || m_currentToken.type == TokenType::IDENTIFIER) {
        textNode->value = m_currentToken.literal;
    }

    if (!expectPeek(TokenType::RBRACE)) return nullptr;

    return textNode;
}

std::unique_ptr<AttributeNode> Parser::parseAttributeNode() {
    auto attr = std::make_unique<AttributeNode>();
    attr->key = m_currentToken;

    if (!expectPeek(TokenType::COLON) && m_currentToken.type != TokenType::EQUALS) return nullptr;
    nextToken();

    attr->value = parseExpression();

    if (m_peekToken.type == TokenType::SEMICOLON) nextToken();

    return attr;
}

std::unique_ptr<Expression> Parser::parseExpression() {
    if (m_currentToken.type == TokenType::IDENTIFIER) {
        auto ident = std::make_unique<Identifier>();
        ident->token = m_currentToken;
        ident->value = m_currentToken.literal;
        return ident;
    } else if (m_currentToken.type == TokenType::STRING_LITERAL) {
        auto str = std::make_unique<StringLiteral>();
        str->token = m_currentToken;
        str->value = m_currentToken.literal;
        return str;
    }
    return nullptr;
}

}
