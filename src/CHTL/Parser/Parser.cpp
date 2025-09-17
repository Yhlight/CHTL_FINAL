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
        auto child = parseNode();
        child->parent = root;
        root->children.push_back(child);
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
        return parseDefinitionBlock();
    }
    throw std::runtime_error("Unexpected token in parseNode: " + currentToken().value);
}


std::shared_ptr<ElementNode> Parser::parseElement() {
    auto element = std::make_shared<ElementNode>();
    element->tagName = consume(TokenType::IDENTIFIER, "Expected element name.").value;

    consume(TokenType::LEFT_BRACE, "Expected '{' after element name.");

    while (currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
        std::shared_ptr<BaseNode> childNode;
        bool isAttribute = false;

        if (currentToken().type == TokenType::KEYWORD_TEXT) {
            if (peekToken().type == TokenType::LEFT_BRACE) {
                 childNode = parseTextBlock();
            } else if (peekToken().type == TokenType::COLON || peekToken().type == TokenType::EQUALS) {
                childNode = parseAttribute();
                isAttribute = true;
            } else {
                 throw std::runtime_error("Unexpected token after 'text' keyword.");
            }
        } else if (currentToken().type == TokenType::IDENTIFIER) {
            if (peekToken().type == TokenType::LEFT_BRACE) {
                childNode = parseElement();
            } else if (peekToken().type == TokenType::COLON || peekToken().type == TokenType::EQUALS) {
                childNode = parseAttribute();
                isAttribute = true;
            } else {
                throw std::runtime_error("Unexpected token after identifier in element block.");
            }
        } else if (currentToken().type == TokenType::KEYWORD_STYLE) {
            childNode = parseStyleBlock();
        } else if (currentToken().type == TokenType::AT_SIGN) {
            childNode = parseUsage();
        } else {
            throw std::runtime_error("Unexpected token in element block: " + currentToken().value);
        }

        childNode->parent = element;
        if (isAttribute) {
            element->attributes.push_back(std::dynamic_pointer_cast<AttributeNode>(childNode));
        } else {
            element->children.push_back(childNode);
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' after element block.");

    return element;
}

std::shared_ptr<AttributeNode> Parser::parseAttribute() {
    auto attribute = std::make_shared<AttributeNode>();

    if(currentToken().type == TokenType::IDENTIFIER || currentToken().type == TokenType::KEYWORD_TEXT) {
        attribute->key = currentToken().value;
        advance();
    } else {
        throw std::runtime_error("Expected attribute key (identifier or 'text').");
    }

    if (!match(TokenType::COLON)) {
        consume(TokenType::EQUALS, "Expected ':' or '=' after attribute key.");
    }

    auto valueNode = parseValue();
    valueNode->parent = attribute;
    attribute->value = valueNode;

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
    return m_tokens.back();
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
        std::shared_ptr<BaseNode> child;
        if (currentToken().type == TokenType::AT_SIGN) {
            child = parseUsage();
        } else {
            auto propNode = std::make_shared<CssPropertyNode>();
            propNode->key = consume(TokenType::IDENTIFIER, "Expected CSS property key.").value;
            consume(TokenType::COLON, "Expected ':' after CSS property key.");

            auto valueNode = parseValue();
            valueNode->parent = propNode;
            propNode->value = valueNode;

            consume(TokenType::SEMICOLON, "Expected ';' after CSS property value.");
            child = propNode;
        }
        child->parent = styleNode;
        styleNode->children.push_back(child);
    }

    consume(TokenType::RIGHT_BRACE, "Expected '}' after style block.");
    return styleNode;
}

std::shared_ptr<BaseNode> Parser::parseDefinitionBlock() {
    consume(TokenType::LEFT_BRACKET, "Expected '[' to start definition block.");

    bool isCustom = match(TokenType::KEYWORD_CUSTOM);
    if (!isCustom) {
        consume(TokenType::KEYWORD_TEMPLATE, "Expected 'Template' or 'Custom' keyword.");
    }

    consume(TokenType::RIGHT_BRACKET, "Expected ']' after keyword.");
    consume(TokenType::AT_SIGN, "Expected '@' before type.");

    TemplateType type;
    if (match(TokenType::KEYWORD_STYLE)) {
        type = TemplateType::STYLE;
    } else if (match(TokenType::KEYWORD_ELEMENT)) {
        type = TemplateType::ELEMENT;
    } else if (match(TokenType::KEYWORD_VAR)) {
        type = TemplateType::VAR;
    } else {
        throw std::runtime_error("Expected definition type (Style, Element, or Var).");
    }

    std::string name = consume(TokenType::IDENTIFIER, "Expected definition name.").value;
    consume(TokenType::LEFT_BRACE, "Expected '{' for definition body.");

    if (isCustom) {
        auto customNode = std::make_shared<CustomNode>();
        customNode->name = name;
        customNode->customType = type;

        if (type == TemplateType::STYLE) {
            auto styleNode = std::make_shared<StyleNode>();
            styleNode->parent = customNode;
            while (currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
                std::shared_ptr<BaseNode> child;
                if (currentToken().type == TokenType::AT_SIGN) {
                    child = parseUsage();
                } else {
                    auto propNode = std::make_shared<CssPropertyNode>();
                    propNode->key = consume(TokenType::IDENTIFIER, "Expected CSS property key.").value;
                    consume(TokenType::COLON, "Expected ':' after CSS property key.");
                    auto valueNode = parseValue();
                    valueNode->parent = propNode;
                    propNode->value = valueNode;
                    consume(TokenType::SEMICOLON, "Expected ';' after CSS property value.");
                    child = propNode;
                }
                child->parent = styleNode;
                styleNode->children.push_back(child);
            }
            customNode->body.push_back(styleNode);
        } else {
            while(currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) advance();
        }

        consume(TokenType::RIGHT_BRACE, "Expected '}' to close custom body.");
        TemplateRegistry::getInstance().registerDefinition(name, customNode);
        return customNode;
    } else {
        auto templateNode = std::make_shared<TemplateNode>();
        templateNode->name = name;
        templateNode->templateType = type;

        if (type == TemplateType::STYLE) {
            auto styleNode = std::make_shared<StyleNode>();
            styleNode->parent = templateNode;
            while (currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
                std::shared_ptr<BaseNode> child;
                 if (currentToken().type == TokenType::AT_SIGN) {
                    child = parseUsage();
                } else {
                    auto propNode = std::make_shared<CssPropertyNode>();
                    propNode->key = consume(TokenType::IDENTIFIER, "Expected CSS property key.").value;
                    consume(TokenType::COLON, "Expected ':' after CSS property key.");
                    auto valueNode = parseValue();
                    valueNode->parent = propNode;
                    propNode->value = valueNode;
                    consume(TokenType::SEMICOLON, "Expected ';' after CSS property value.");
                    child = propNode;
                }
                child->parent = styleNode;
                styleNode->children.push_back(child);
            }
            templateNode->body.push_back(styleNode);
        } else if (type == TemplateType::ELEMENT) {
            while (currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
                auto child = parseNode();
                child->parent = templateNode;
                templateNode->body.push_back(child);
            }
        } else if (type == TemplateType::VAR) {
             while (currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
                auto varNode = std::make_shared<VarDeclarationNode>();
                varNode->name = consume(TokenType::IDENTIFIER, "Expected variable name.").value;
                consume(TokenType::COLON, "Expected ':' after variable name.");
                varNode->value = consume(TokenType::STRING_LITERAL, "Expected string literal for var value.").value;
                consume(TokenType::SEMICOLON, "Expected ';' after variable value.");
                varNode->parent = templateNode;
                templateNode->body.push_back(varNode);
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expected '}' to close template body.");
        TemplateRegistry::getInstance().registerDefinition(name, templateNode);
        return templateNode;
    }
}

std::shared_ptr<BaseNode> Parser::parseUsage() {
    consume(TokenType::AT_SIGN, "Expected '@' for usage.");

    TemplateType type;
    if (match(TokenType::KEYWORD_STYLE)) {
        type = TemplateType::STYLE;
    } else if (match(TokenType::KEYWORD_ELEMENT)) {
        type = TemplateType::ELEMENT;
    } else {
        throw std::runtime_error("Expected usage type (Style or Element).");
    }

    std::string name = consume(TokenType::IDENTIFIER, "Expected name for usage.").value;

    if (match(TokenType::SEMICOLON)) {
        auto usageNode = std::make_shared<TemplateUsageNode>();
        usageNode->templateType = type;
        usageNode->name = name;
        return usageNode;
    } else if (match(TokenType::LEFT_BRACE)) {
        auto customUsageNode = std::make_shared<CustomUsageNode>();
        customUsageNode->usageType = type;
        customUsageNode->name = name;

        while(currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
            std::shared_ptr<BaseNode> child;
            if (currentToken().type == TokenType::KEYWORD_DELETE) {
                child = parseDelete();
            } else if (currentToken().type == TokenType::KEYWORD_INSERT) {
                child = parseInsert();
            } else {
                throw std::runtime_error("Unsupported specialization syntax.");
            }
            child->parent = customUsageNode;
            customUsageNode->specializationBody.push_back(child);
        }

        consume(TokenType::RIGHT_BRACE, "Expected '}' to close specialization body.");
        return customUsageNode;
    } else {
        throw std::runtime_error("Expected ';' or '{' after usage name.");
    }
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

std::shared_ptr<DeleteNode> Parser::parseDelete() {
    consume(TokenType::KEYWORD_DELETE, "Expected 'delete' keyword.");
    auto deleteNode = std::make_shared<DeleteNode>();

    do {
        deleteNode->targets.push_back(consume(TokenType::IDENTIFIER, "Expected target for delete.").value);
    } while (match(TokenType::COMMA));

    consume(TokenType::SEMICOLON, "Expected ';' after delete statement.");
    return deleteNode;
}

std::shared_ptr<InsertNode> Parser::parseInsert() {
    consume(TokenType::KEYWORD_INSERT, "Expected 'insert' keyword.");
    auto insertNode = std::make_shared<InsertNode>();

    if (match(TokenType::KEYWORD_AFTER)) {
        insertNode->position = InsertPosition::AFTER;
    } else if (match(TokenType::KEYWORD_BEFORE)) {
        insertNode->position = InsertPosition::BEFORE;
    } else if (match(TokenType::KEYWORD_REPLACE)) {
        insertNode->position = InsertPosition::REPLACE;
    } else if (match(TokenType::KEYWORD_AT)) {
        if (match(TokenType::KEYWORD_TOP)) {
            insertNode->position = InsertPosition::AT_TOP;
        } else if (match(TokenType::KEYWORD_BOTTOM)) {
            insertNode->position = InsertPosition::AT_BOTTOM;
        } else {
            throw std::runtime_error("Expected 'top' or 'bottom' after 'at'.");
        }
    } else {
        throw std::runtime_error("Expected position keyword (after, before, etc.) after 'insert'.");
    }

    std::string selector_str = consume(TokenType::IDENTIFIER, "Expected selector tag name.").value;
    if (match(TokenType::LEFT_BRACKET)) {
        selector_str += "[";
        selector_str += consume(TokenType::UNQUOTED_LITERAL, "Expected index in selector.").value;
        selector_str += "]";
        consume(TokenType::RIGHT_BRACKET, "Expected ']' after selector index.");
    }
    insertNode->selector = selector_str;

    consume(TokenType::LEFT_BRACE, "Expected '{' for insert body.");
    while(currentToken().type != TokenType::RIGHT_BRACE && currentToken().type != TokenType::END_OF_FILE) {
        auto child = parseNode();
        child->parent = insertNode;
        insertNode->body.push_back(child);
    }
    consume(TokenType::RIGHT_BRACE, "Expected '}' to close insert body.");

    return insertNode;
}

} // namespace CHTL
