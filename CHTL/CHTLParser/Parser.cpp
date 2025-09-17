#include "Parser.h"
#include "StylePropertyNode.h"
#include <sstream>

namespace CHTL {

Parser::Parser(Lexer& lexer) : m_lexer(lexer) {
    // Prime the parser with the first two tokens
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    m_currentToken = m_peekToken;
    m_peekToken = m_lexer.NextToken();
}

bool Parser::currentTokenIs(TokenType t) {
    return m_currentToken.type == t;
}

bool Parser::peekTokenIs(TokenType t) {
    return m_peekToken.type == t;
}

void Parser::peekError(TokenType t) {
    std::stringstream ss;
    ss << "line " << m_peekToken.line << ":" << m_peekToken.column
       << " - expected next token to be of type " << (int)t
       << ", got type " << (int)m_peekToken.type << " instead.";
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

std::unique_ptr<Program> Parser::ParseProgram() {
    auto program = std::make_unique<Program>();
    while (!currentTokenIs(TokenType::END_OF_FILE)) {
        std::unique_ptr<Node> stmt = nullptr;
        if (currentTokenIs(TokenType::LBRACKET)) {
            auto templateDef = parseTemplateDefinition();
            if (templateDef) {
                // Add to registry, not statements
                program->templateRegistry[templateDef->name] = std::move(templateDef);
            }
        } else {
            stmt = parseStatement();
            if (stmt) {
                program->statements.push_back(std::move(stmt));
            }
        }
        nextToken();
    }
    return program;
}

std::unique_ptr<Node> Parser::parseStatement() {
    if (currentTokenIs(TokenType::IDENTIFIER)) {
        if (m_currentToken.literal == "text") {
            return parseTextNode();
        }
        if (m_currentToken.literal == "style") {
            return parseStyleNode();
        }
        return parseElementNode();
    } else if (currentTokenIs(TokenType::AT)) {
        return parseTemplateUsage();
    }
    return nullptr;
}

std::unique_ptr<TemplateDefinitionNode> Parser::parseTemplateDefinition() {
    auto node = std::make_unique<TemplateDefinitionNode>();

    // Expect [Template]
    if (!expectPeek(TokenType::IDENTIFIER) || m_currentToken.literal != "Template") return nullptr;
    if (!expectPeek(TokenType::RBRACKET)) return nullptr;
    if (!expectPeek(TokenType::AT)) return nullptr;

    // Expect @Type
    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    node->templateType = m_currentToken.literal;

    // Expect Name
    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    node->name = m_currentToken.literal;

    // Expect { ... body ... }
    if (!expectPeek(TokenType::LBRACE)) return nullptr;

    // The body of the template depends on its type. For now, we only handle @Style.
    if (node->templateType == "Style") {
        // A style template body is just like a style block.
        // We can reuse the parseStyleNode but it returns the wrong type.
        // Let's parse it manually for now.
        auto styleBody = std::make_unique<StyleNode>();
        nextToken(); // consume '{'
        while (!currentTokenIs(TokenType::RBRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
            if (!currentTokenIs(TokenType::IDENTIFIER)) { nextToken(); continue; }
            auto propNode = std::make_unique<StylePropertyNode>();
            propNode->key = m_currentToken.literal;

            if (!expectPeek(TokenType::COLON)) return nullptr;

            std::string value = "";
            nextToken(); // consume ':'
            while (!currentTokenIs(TokenType::SEMICOLON) && !currentTokenIs(TokenType::RBRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
                value += m_currentToken.literal;
                nextToken();
            }
            propNode->value = value;
            styleBody->items.push_back(std::move(propNode));

            if (!currentTokenIs(TokenType::SEMICOLON)) return nullptr;
            nextToken(); // consume ';'
        }
        node->body = std::move(styleBody);
    } else {
        // Error: unsupported template type
        return nullptr;
    }

    if (!currentTokenIs(TokenType::RBRACE)) return nullptr;

    return node;
}

std::unique_ptr<TemplateUsageNode> Parser::parseTemplateUsage() {
    auto node = std::make_unique<TemplateUsageNode>();

    // Expect @Type
    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    node->templateType = m_currentToken.literal;

    // Expect Name
    if (!expectPeek(TokenType::IDENTIFIER)) return nullptr;
    node->name = m_currentToken.literal;

    // Expect ;
    if (!expectPeek(TokenType::SEMICOLON)) return nullptr;

    return node;
}

std::unique_ptr<StyleNode> Parser::parseStyleNode() {
    auto node = std::make_unique<StyleNode>();

    if (!expectPeek(TokenType::LBRACE)) {
        return nullptr;
    }
    nextToken(); // consume '{'

    while (!currentTokenIs(TokenType::RBRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        if (currentTokenIs(TokenType::AT)) {
            node->items.push_back(parseTemplateUsage());
        } else if (currentTokenIs(TokenType::IDENTIFIER)) {
            auto propNode = std::make_unique<StylePropertyNode>();
            propNode->key = m_currentToken.literal;

            if (!expectPeek(TokenType::COLON)) return nullptr;

            std::string value = "";
            nextToken(); // consume ':'
            while (!currentTokenIs(TokenType::SEMICOLON) && !currentTokenIs(TokenType::RBRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
                value += m_currentToken.literal;
                nextToken();
            }
            propNode->value = value;
            node->items.push_back(std::move(propNode));
        } else {
            // Error
            nextToken();
        }

        // Consume the semicolon if it's there
        if (currentTokenIs(TokenType::SEMICOLON)) {
            nextToken();
        }
    }

    return node;
}

void Parser::parseAttribute(ElementNode* node) {
    std::string key = m_currentToken.literal;

    // Expect ':' or '='
    if (!peekTokenIs(TokenType::COLON) && !peekTokenIs(TokenType::ASSIGN)) {
        peekError(TokenType::COLON); // Just report one for the error message
        return;
    }
    nextToken(); // consume identifier

    // Expect value (string or identifier)
    if (!peekTokenIs(TokenType::STRING) && !peekTokenIs(TokenType::IDENTIFIER)) {
        peekError(TokenType::STRING);
        return;
    }
    nextToken(); // consume ':' or '='

    std::string value = m_currentToken.literal;
    node->attributes.push_back({key, value});

    // Expect semicolon at the end
    if (!peekTokenIs(TokenType::SEMICOLON)) {
        peekError(TokenType::SEMICOLON);
        return;
    }
    nextToken(); // consume value
}

std::unique_ptr<ElementNode> Parser::parseElementNode() {
    auto node = std::make_unique<ElementNode>();
    node->tagName = m_currentToken.literal;

    if (!expectPeek(TokenType::LBRACE)) {
        return nullptr;
    }

    nextToken(); // Consume '{'

    while (!currentTokenIs(TokenType::RBRACE) && !currentTokenIs(TokenType::END_OF_FILE)) {
        if (currentTokenIs(TokenType::IDENTIFIER) && (peekTokenIs(TokenType::COLON) || peekTokenIs(TokenType::ASSIGN))) {
            parseAttribute(node.get());
        } else {
            auto stmt = parseStatement();
            if (stmt) {
                node->children.push_back(std::move(stmt));
            }
        }
        nextToken(); // Consume the last token of the statement/attribute (e.g. '}' or ';')
    }

    if (!currentTokenIs(TokenType::RBRACE)) {
        // Missing closing brace error could be added here
        return nullptr;
    }

    return node;
}

std::unique_ptr<TextNode> Parser::parseTextNode() {
    auto node = std::make_unique<TextNode>();

    if (!expectPeek(TokenType::LBRACE)) {
        return nullptr;
    }

    if (!expectPeek(TokenType::STRING)) {
        return nullptr;
    }

    node->value = m_currentToken.literal;

    if (!expectPeek(TokenType::RBRACE)) {
        return nullptr;
    }

    return node;
}

} // namespace CHTL
