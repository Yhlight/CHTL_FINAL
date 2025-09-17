#include "Parser.h"
#include <stdexcept>

namespace CHTL {

Parser::Parser(const std::vector<Token>& tokens) : m_tokens(tokens) {}

std::shared_ptr<RootNode> Parser::parse() {
    auto root = std::make_shared<RootNode>();
    while (currentToken().type != TokenType::END_OF_FILE) {
        while (currentToken().type == TokenType::COMMENT) {
            advance();
        }
        if (currentToken().type == TokenType::END_OF_FILE) break;
        root->children.push_back(parseNode());
    }
    return root;
}

std::shared_ptr<BaseNode> Parser::parseNode() {
    if (currentToken().type == TokenType::KEYWORD_TEXT) {
        return parseTextBlock();
    }
    if (currentToken().type == TokenType::IDENTIFIER) {
        return parseElement();
    }
    if (currentToken().type == TokenType::LEFT_BRACKET) {
        return parseTemplateDefinition();
    }
    // Handle other top-level nodes like comments later
    throw std::runtime_error("Unexpected token in parseNode: " + currentToken().value);
}


std::shared_ptr<ElementNode> Parser::parseElement() {
    auto element = std::make_shared<ElementNode>();
    element->tagName = consume(TokenType::IDENTIFIER, "Expected element name.").value;

    consume(TokenType::LEFT_BRACE, "Expected '{' after element name.");

    while (currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
        if (currentToken().type == TokenType::KEYWORD_TEXT) {
            // This can be a text block `text {...}` or a text attribute `text: "..."`
            if (peekToken().type == TokenType::LEFT_BRACE) {
                 element->children.push_back(parseTextBlock());
            } else if (peekToken().type == TokenType::COLON || peekToken().type == TokenType::EQUALS) {
                element->attributes.push_back(parseAttribute());
            } else {
                 throw std::runtime_error("Unexpected token after 'text' keyword.");
            }
        } else if (currentToken().type == TokenType::IDENTIFIER) {
            // It can be a nested element or an attribute
            if (peekToken().type == TokenType::LEFT_BRACE) {
                element->children.push_back(parseElement());
            } else if (peekToken().type == TokenType::COLON || peekToken().type == TokenType::EQUALS) {
                element->attributes.push_back(parseAttribute());
            } else {
                throw std::runtime_error("Unexpected token after identifier in element block.");
            }
        } else if (currentToken().type == TokenType::KEYWORD_STYLE) {
            element->children.push_back(parseStyleBlock());
        } else if (currentToken().type == TokenType::AT_SIGN) {
            element->children.push_back(parseTemplateUsage());
        } else {
            throw std::runtime_error("Unexpected token in element block: " + currentToken().value);
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' after element block.");

    return element;
}

std::shared_ptr<AttributeNode> Parser::parseAttribute() {
    auto attribute = std::make_shared<AttributeNode>();

    // The key can be an identifier or a keyword like 'text'
    if(currentToken().type == TokenType::IDENTIFIER || currentToken().type == TokenType::KEYWORD_TEXT) {
        attribute->key = currentToken().value;
        advance();
    } else {
        throw std::runtime_error("Expected attribute key (identifier or 'text').");
    }

    if (!match(TokenType::COLON)) {
        consume(TokenType::EQUALS, "Expected ':' or '=' after attribute key.");
    }

    attribute->value = parseValue();

    consume(TokenType::SEMICOLON, "Expected ';' after attribute value.");
    return attribute;
}

std::shared_ptr<TextNode> Parser::parseTextBlock() {
    auto textNode = std::make_shared<TextNode>();
    consume(TokenType::KEYWORD_TEXT, "Expected 'text' keyword.");
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'text'.");

    if (currentToken().type == TokenType::STRING_LITERAL || currentToken().type == TokenType::UNQUOTED_LITERAL || currentToken().type == TokenType::IDENTIFIER) {
        textNode->content = currentToken().value;
        advance();
    } else {
        // Allow empty text blocks
        textNode->content = "";
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' after text block.");
    return textNode;
}


const Token& Parser::currentToken() const {
    return m_tokens[m_current];
}

const Token& Parser::peekToken() const {
    if (m_current + 1 < m_tokens.size()) {
        return m_tokens[m_current + 1];
    }
    return m_tokens.back(); // Return EOF token
}


void Parser::advance() {
    if (m_current < m_tokens.size()) {
        m_current++;
    }
}

bool Parser::match(TokenType type) {
    if (currentToken().type == type) {
        advance();
        return true;
    }
    return false;
}

const Token& Parser::consume(TokenType type, const std::string& errorMessage) {
    if (currentToken().type == type) {
        const Token& consumedToken = currentToken();
        advance();
        return consumedToken;
    }
    throw std::runtime_error(errorMessage + " Found " + currentToken().value + " instead.");
}


std::shared_ptr<StyleNode> Parser::parseStyleBlock() {
    auto styleNode = std::make_shared<StyleNode>();
    consume(TokenType::KEYWORD_STYLE, "Expected 'style' keyword.");
    consume(TokenType::LEFT_BRACE, "Expected '{' after 'style'.");

    while (currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
        if (currentToken().type == TokenType::AT_SIGN) {
            styleNode->children.push_back(parseTemplateUsage());
        } else {
            auto propNode = std::make_shared<CssPropertyNode>();
            propNode->key = consume(TokenType::IDENTIFIER, "Expected CSS property key.").value;
            consume(TokenType::COLON, "Expected ':' after CSS property key.");

            propNode->value = parseValue();

            consume(TokenType::SEMICOLON, "Expected ';' after CSS property value.");
            styleNode->children.push_back(propNode);
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' after style block.");
    return styleNode;
}

std::shared_ptr<TemplateNode> Parser::parseTemplateDefinition() {
    auto templateNode = std::make_shared<TemplateNode>();

    consume(TokenType::LEFT_BRACKET, "Expected '[' to start template definition.");
    consume(TokenType::KEYWORD_TEMPLATE, "Expected 'Template' keyword.");
    consume(TokenType::RIGHT_BRACKET, "Expected ']' after 'Template' keyword.");

    consume(TokenType::AT_SIGN, "Expected '@' before template type.");

    if (currentToken().type == TokenType::KEYWORD_STYLE) {
        templateNode->templateType = TemplateType::STYLE;
        advance();
    } else if (currentToken().type == TokenType::KEYWORD_ELEMENT) {
        templateNode->templateType = TemplateType::ELEMENT;
        advance();
    } else if (currentToken().type == TokenType::KEYWORD_VAR) {
        templateNode->templateType = TemplateType::VAR;
        advance();
    } else {
        throw std::runtime_error("Expected template type (Style, Element, or Var).");
    }

    templateNode->name = consume(TokenType::IDENTIFIER, "Expected template name.").value;

    consume(TokenType::LEFT_BRACE, "Expected '{' for template body.");

    // For now, let's focus on parsing style templates.
    // The body of a style template is essentially a style block.
    if (templateNode->templateType == TemplateType::STYLE) {
        auto styleNode = std::make_shared<StyleNode>();
        while (currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
            if (currentToken().type == TokenType::AT_SIGN) {
                styleNode->children.push_back(parseTemplateUsage());
            } else {
                auto propNode = std::make_shared<CssPropertyNode>();
                propNode->key = consume(TokenType::IDENTIFIER, "Expected CSS property key.").value;
                consume(TokenType::COLON, "Expected ':' after CSS property key.");
                propNode->value = parseValue();
                consume(TokenType::SEMICOLON, "Expected ';' after CSS property value.");
                styleNode->children.push_back(propNode);
            }
        }
        templateNode->body.push_back(styleNode);
    } else if (templateNode->templateType == TemplateType::ELEMENT) {
        while (currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
            templateNode->body.push_back(parseNode());
        }
    } else if (templateNode->templateType == TemplateType::VAR) {
        while (currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
            auto varNode = std::make_shared<VarDeclarationNode>();
            varNode->name = consume(TokenType::IDENTIFIER, "Expected variable name.").value;
            consume(TokenType::COLON, "Expected ':' after variable name.");
            if (currentToken().type == TokenType::IDENTIFIER || currentToken().type == TokenType::UNQUOTED_LITERAL || currentToken().type == TokenType::STRING_LITERAL) {
                varNode->value = currentToken().value;
                advance();
            } else {
                throw std::runtime_error("Expected variable value.");
            }
            consume(TokenType::SEMICOLON, "Expected ';' after variable value.");
            templateNode->body.push_back(varNode);
        }
    }


    consume(TokenType::RIGHT_BRACE, "Expected '}' to close template body.");

    TemplateRegistry::getInstance().registerTemplate(templateNode->name, templateNode);

    return templateNode;
}

std::shared_ptr<TemplateUsageNode> Parser::parseTemplateUsage() {
    auto usageNode = std::make_shared<TemplateUsageNode>();
    consume(TokenType::AT_SIGN, "Expected '@' for template usage.");

    if (currentToken().type == TokenType::KEYWORD_STYLE) {
        usageNode->templateType = TemplateType::STYLE;
        advance();
    } else if (currentToken().type == TokenType::KEYWORD_ELEMENT) {
        usageNode->templateType = TemplateType::ELEMENT;
        advance();
    } else if (currentToken().type == TokenType::KEYWORD_VAR) {
        usageNode->templateType = TemplateType::VAR;
        advance();
    } else {
        throw std::runtime_error("Expected template type (Style, Element, or Var) after '@'.");
    }

    usageNode->name = consume(TokenType::IDENTIFIER, "Expected template name for usage.").value;

    consume(TokenType::SEMICOLON, "Expected ';' after template usage.");

    return usageNode;
}

std::shared_ptr<BaseNode> Parser::parseValue() {
    if (currentToken().type == TokenType::IDENTIFIER && peekToken().type == TokenType::LEFT_PAREN) {
        return parseVarUsage();
    }

    if (currentToken().type == TokenType::STRING_LITERAL || currentToken().type == TokenType::IDENTIFIER || currentToken().type == TokenType::UNQUOTED_LITERAL) {
        auto literal = std::make_shared<LiteralNode>();
        literal->value = currentToken().value;
        advance();
        return literal;
    }

    throw std::runtime_error("Expected a value (literal or variable usage).");
}

std::shared_ptr<VarUsageNode> Parser::parseVarUsage() {
    auto varUsage = std::make_shared<VarUsageNode>();
    varUsage->groupName = consume(TokenType::IDENTIFIER, "Expected variable group name.").value;
    consume(TokenType::LEFT_PAREN, "Expected '(' after variable group name.");
    varUsage->varName = consume(TokenType::IDENTIFIER, "Expected variable name inside parentheses.").value;
    consume(TokenType::RIGHT_PAREN, "Expected ')' after variable name.");
    return varUsage;
}

} // namespace CHTL
