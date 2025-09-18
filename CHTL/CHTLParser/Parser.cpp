#include "Parser.hpp"
#include <iostream>

namespace CHTL {

Parser::Parser(Lexer& l, CHTLContext& ctx) : lexer(l), context(ctx) {
    nextToken();
    nextToken();
}

void Parser::nextToken() {
    currentToken = peekToken;
    peekToken = lexer.nextToken();
}

const std::vector<std::string>& Parser::getErrors() const {
    return errors;
}

std::unique_ptr<Program> Parser::parseProgram() {
    auto program = std::make_unique<Program>();
    while (currentToken.type != TokenType::END_OF_FILE) {
        auto node = parseNode();
        if (node) {
            if (auto* defNode = dynamic_cast<TemplateDefinitionNode*>(node.get())) {
                context.addTemplate(defNode);
            }
            program->addChild(std::move(node));
        } else {
            nextToken();
        }
    }
    return program;
}

std::unique_ptr<Node> Parser::parseNode() {
    switch (currentToken.type) {
        case TokenType::COMMENT:    return parseCommentNode();
        case TokenType::IDENT:      return parseElementNode();
        case TokenType::LBRACKET:   return parseTemplateDefinition();
        case TokenType::AT:         // Top-level usage is handled in parseProgram
        default:                    return nullptr;
    }
}

std::unique_ptr<InheritanceNode> Parser::parseInheritanceStatement() {
    if (currentToken.type == TokenType::INHERIT) {
        nextToken();
    }
    if (currentToken.type != TokenType::AT) return nullptr;
    nextToken();

    TemplateType type;
    if (currentToken.literal == "Style") type = TemplateType::STYLE;
    else if (currentToken.literal == "Element") type = TemplateType::ELEMENT;
    else return nullptr;
    nextToken();

    std::string name = currentToken.literal;
    nextToken();

    if (currentToken.type != TokenType::SEMICOLON) return nullptr;
    nextToken();

    return std::make_unique<InheritanceNode>(type, name);
}

void Parser::expandElementTemplate(std::vector<std::unique_ptr<Node>>& nodes, const std::string& templateName) {
    auto* templateDef = context.getElementTemplate(templateName);
    if (!templateDef) {
        errors.push_back("Element template not found: " + templateName);
        return;
    }

    for (const auto& child : templateDef->children) {
        if (auto* inheritNode = dynamic_cast<InheritanceNode*>(child.get())) {
            expandElementTemplate(nodes, inheritNode->name);
        } else {
            nodes.push_back(child->clone());
        }
    }
}


std::vector<std::unique_ptr<Node>> Parser::parseTemplateUsage() {
    nextToken(); // consume '@'
    if (currentToken.literal != "Element") return {};
    nextToken(); // consume 'Element'

    std::string name = currentToken.literal;
    nextToken(); // consume name

    if (currentToken.type != TokenType::SEMICOLON) {
        errors.push_back("Expected ';' after template usage.");
    }
    nextToken(); // consume ';'

    std::vector<std::unique_ptr<Node>> clonedNodes;
    expandElementTemplate(clonedNodes, name);
    return clonedNodes;
}

void Parser::expandStyleTemplate(ElementNode* parent, const std::string& templateName) {
    auto* templateDef = context.getStyleTemplate(templateName);
    if (!templateDef) {
        errors.push_back("Style template not found: " + templateName);
        return;
    }

    for (const auto& child : templateDef->children) {
        if (auto* inheritNode = dynamic_cast<InheritanceNode*>(child.get())) {
            expandStyleTemplate(parent, inheritNode->name);
        } else if (auto* propNode = dynamic_cast<StylePropertyNode*>(child.get())) {
            parent->styles[propNode->key] = propNode->value;
        }
    }
}


void Parser::parseAttributes(ElementNode* element) {
    std::string key = currentToken.literal;
    nextToken();
    nextToken();

    std::string value;
    if (currentToken.type == TokenType::STRING || currentToken.type == TokenType::IDENT) {
        value = currentToken.literal;
    }
    element->attributes[key] = value;
    nextToken();

    if (currentToken.type == TokenType::SEMICOLON) {
        nextToken();
    }
}

std::unique_ptr<StylePropertyNode> Parser::parseStyleProperty() {
    std::string key = currentToken.literal;
    nextToken();
    nextToken();

    std::string value;
    while (currentToken.type != TokenType::SEMICOLON && currentToken.type != TokenType::RBRACE) {
        value += currentToken.literal;
        nextToken();
    }

    if (currentToken.type == TokenType::SEMICOLON) {
        nextToken();
    }
    return std::make_unique<StylePropertyNode>(key, value);
}

void Parser::parseStyleBlock(ElementNode* parent) {
    nextToken();
    nextToken();

    while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
        if (currentToken.type == TokenType::AT) {
            nextToken();
            if (currentToken.literal != "Style") { break; }
            nextToken();
            std::string templateName = currentToken.literal;
            expandStyleTemplate(parent, templateName);
            nextToken();
            if (currentToken.type == TokenType::SEMICOLON) nextToken();

        } else {
            auto propNode = parseStyleProperty();
            if (propNode) {
                parent->styles[propNode->key] = propNode->value;
            }
        }
    }
    nextToken();
}

std::unique_ptr<TemplateDefinitionNode> Parser::parseTemplateDefinition() {
    nextToken();
    nextToken();
    nextToken();
    nextToken();

    TemplateType type;
    if (currentToken.literal == "Style") type = TemplateType::STYLE;
    else if (currentToken.literal == "Element") type = TemplateType::ELEMENT;
    else return nullptr;
    nextToken();

    std::string name = currentToken.literal;
    auto templateNode = std::make_unique<TemplateDefinitionNode>(type, name);
    nextToken();
    nextToken();

    while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
        if (currentToken.type == TokenType::INHERIT || currentToken.type == TokenType::AT) {
            auto inheritNode = parseInheritanceStatement();
            if(inheritNode) templateNode->addChild(std::move(inheritNode));
        }
        else if (type == TemplateType::STYLE) {
            auto propNode = parseStyleProperty();
            if (propNode) templateNode->addChild(std::move(propNode));
        } else {
            auto child = parseNode();
            if (child) templateNode->addChild(std::move(child));
        }
    }
    nextToken();
    return templateNode;
}

std::unique_ptr<ElementNode> Parser::parseElementNode() {
    auto elementNode = std::make_unique<ElementNode>(currentToken.literal);

    if (elementNode->tagName == "text") {
        nextToken();
        if (currentToken.type != TokenType::LBRACE) return nullptr;
        nextToken();
        std::string textContent;
        if (currentToken.type == TokenType::STRING) {
            textContent = currentToken.literal;
            nextToken();
        } else {
            while(currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
                textContent += currentToken.literal;
                nextToken();
                if (currentToken.type != TokenType::RBRACE) textContent += " ";
            }
        }
        auto textNode = std::make_unique<TextNode>(textContent);
        elementNode->addChild(std::move(textNode));
        if (currentToken.type != TokenType::RBRACE) return nullptr;
        nextToken();
        return elementNode;
    }
    nextToken();

    if (currentToken.type != TokenType::LBRACE) return nullptr;
    nextToken();

    while (currentToken.type != TokenType::RBRACE && currentToken.type != TokenType::END_OF_FILE) {
        if (currentToken.type == TokenType::AT) {
            auto nodes = parseTemplateUsage();
            for (auto& node : nodes) elementNode->addChild(std::move(node));
        } else if (currentToken.type == TokenType::COMMENT) {
            auto childNode = parseCommentNode();
            if (childNode) elementNode->addChild(std::move(childNode));
        } else if (currentToken.type == TokenType::IDENT) {
            if (currentToken.literal == "style") {
                parseStyleBlock(elementNode.get());
            } else if (peekToken.type == TokenType::COLON || peekToken.type == TokenType::EQUAL) {
                parseAttributes(elementNode.get());
            } else {
                auto childNode = parseNode();
                if (childNode) elementNode->addChild(std::move(childNode));
            }
        } else {
            errors.push_back("Unexpected token inside element block: " + currentToken.literal);
            nextToken();
        }
    }
    nextToken();
    return elementNode;
}

std::unique_ptr<CommentNode> Parser::parseCommentNode() {
    auto node = std::make_unique<CommentNode>(currentToken.literal);
    nextToken();
    return node;
}

} // namespace CHTL
