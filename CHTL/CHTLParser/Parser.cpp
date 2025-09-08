#include "Parser.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ElementTemplateNode.h"
#include "../CHTLNode/StyleTemplateNode.h"
#include "../CHTLNode/VarTemplateNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include <iostream>
#include <algorithm>

void setParent(BaseNode* parent, BaseNode* child) {
    if (parent && child) {
        child->parent = parent;
    }
}

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    advance();
    advance();
}

void Parser::advance() {
    currentToken = nextToken;
    nextToken = lexer.getNextToken();
}

bool Parser::match(TokenType type) {
    if (check(type)) {
        advance();
        return true;
    }
    return false;
}

void Parser::consume(TokenType type, const std::string& message) {
    if (check(type)) {
        advance();
        return;
    }
    std::cerr << "Parse Error: " << message << " at line " << currentToken.line << ". Got '" << currentToken.lexeme << "'" << std::endl;
    throw std::runtime_error(message);
}

bool Parser::check(TokenType type) {
    return currentToken.type == type;
}

bool Parser::checkNext(TokenType type) {
    return nextToken.type == type;
}

std::unique_ptr<ElementNode> Parser::parse() {
    auto root = std::make_unique<ElementNode>();
    root->tagName = "root";
    while (!check(TokenType::END_OF_FILE)) {
        auto decl = declaration();
        if (decl) {
            setParent(root.get(), decl.get());
            root->children.push_back(std::move(decl));
        }
    }
    return root;
}

std::unique_ptr<BaseNode> Parser::declaration() {
    if (check(TokenType::KEYWORD_TEMPLATE)) return templateDeclaration();
    if (check(TokenType::IDENTIFIER) && checkNext(TokenType::LEFT_BRACE)) return element();
    if (check(TokenType::TEXT)) return textNode();
    if (check(TokenType::STYLE)) return styleNode();

    std::cerr << "Parse Error: Unexpected token " << currentToken.lexeme << " at line " << currentToken.line << std::endl;
    advance();
    return nullptr;
}

std::unique_ptr<ElementNode> Parser::element() {
    auto node = std::make_unique<ElementNode>();
    node->tagName = currentToken.lexeme;
    advance();
    consume(TokenType::LEFT_BRACE, "Expect '{' after element name.");
    while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
        if (check(TokenType::IDENTIFIER)) {
            if (checkNext(TokenType::COLON)) {
                attributes(*node);
            } else if (checkNext(TokenType::LEFT_BRACE)) {
                auto child = element();
                if (child) {
                    setParent(node.get(), child.get());
                    node->children.push_back(std::move(child));
                }
            } else {
                std::cerr << "Parse Error: Unexpected token after identifier '" << currentToken.lexeme << "'" << std::endl;
                advance();
            }
        } else if (check(TokenType::TEXT)) {
            auto child = textNode();
            if (child) {
                setParent(node.get(), child.get());
                node->children.push_back(std::move(child));
            }
        } else if (check(TokenType::STYLE)) {
            auto child = styleNode();
            if (child) {
                setParent(node.get(), child.get());
                node->children.push_back(std::move(child));
            }
        } else if (match(TokenType::AT_ELEMENT)) {
            auto usageNode = std::make_unique<TemplateUsageNode>();
            usageNode->type = TemplateType::ELEMENT;
            usageNode->name = currentToken.lexeme;
            consume(TokenType::IDENTIFIER, "Expect template name after '@Element'.");
            consume(TokenType::SEMICOLON, "Expect ';' after element template usage.");
            node->children.push_back(std::move(usageNode));
        } else {
            std::cerr << "Parse Error: Unexpected token '" << currentToken.lexeme << "' in element '" << node->tagName << "'." << std::endl;
            advance();
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after element block.");
    return node;
}

std::unique_ptr<TextNode> Parser::textNode() {
    consume(TokenType::TEXT, "Expect 'text' keyword.");
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'text'.");
    auto node = std::make_unique<TextNode>();
    if (check(TokenType::STRING) || check(TokenType::IDENTIFIER)) {
        node->content = currentToken.lexeme;
        advance();
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after text content.");
    return node;
}

std::unique_ptr<StyleNode> Parser::styleNode() {
    consume(TokenType::STYLE, "Expect 'style' keyword.");
    consume(TokenType::LEFT_BRACE, "Expect '{' after 'style'.");
    auto node = std::make_unique<StyleNode>();
    while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
        if (check(TokenType::IDENTIFIER)) {
            std::string key = currentToken.lexeme;
            advance();
            consume(TokenType::COLON, "Expect ':' after style property name.");
            node->properties[key] = parseValue();
            consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
        } else if (match(TokenType::AT_STYLE)) {
            std::string templateName = currentToken.lexeme;
            consume(TokenType::IDENTIFIER, "Expect template name after '@Style'.");
            consume(TokenType::SEMICOLON, "Expect ';' after style template usage.");
            if (styleTemplates.count(templateName)) {
                for (const auto& prop : styleTemplates.at(templateName)->properties) {
                    node->properties[prop.first] = prop.second;
                }
            } else {
                std::cerr << "Parse Error: Style template '" << templateName << "' not found." << std::endl;
            }
        } else {
            std::cerr << "Parse Error: Unexpected token in style block." << std::endl;
            advance();
        }
    }
    consume(TokenType::RIGHT_BRACE, "Expect '}' after style block.");
    return node;
}

void Parser::attributes(ElementNode& element) {
    while (check(TokenType::IDENTIFIER) && checkNext(TokenType::COLON)) {
        std::string key = currentToken.lexeme;
        advance();
        consume(TokenType::COLON, "Expect ':' after attribute name.");
        element.attributes[key] = parseValue();
        consume(TokenType::SEMICOLON, "Expect ';' after attribute value.");
    }
}

std::unique_ptr<BaseNode> Parser::templateDeclaration() {
    consume(TokenType::KEYWORD_TEMPLATE, "Expect '[Template]' keyword.");

    if (match(TokenType::AT_STYLE)) {
        auto node = std::make_unique<StyleTemplateNode>();
        node->name = currentToken.lexeme;
        consume(TokenType::IDENTIFIER, "Expect template name.");
        consume(TokenType::LEFT_BRACE, "Expect '{' after template name.");
        while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
            if (check(TokenType::IDENTIFIER)) {
                std::string key = currentToken.lexeme;
                advance();
                consume(TokenType::COLON, "Expect ':' after property name.");
                node->properties[key] = parseValue();
                consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
            } else if (check(TokenType::AT_STYLE) || match(TokenType::KEYWORD_INHERIT)) {
                match(TokenType::KEYWORD_INHERIT);
                consume(TokenType::AT_STYLE, "Expect '@Style' for inheritance.");
                std::string baseTemplateName = currentToken.lexeme;
                consume(TokenType::IDENTIFIER, "Expect base template name.");
                consume(TokenType::SEMICOLON, "Expect ';' after base template usage.");
                if (styleTemplates.count(baseTemplateName)) {
                    for (const auto& prop : styleTemplates.at(baseTemplateName)->properties) {
                        node->properties[prop.first] = prop.second;
                    }
                } else {
                    std::cerr << "Parse Error: Base style template '" << baseTemplateName << "' not found." << std::endl;
                }
            } else {
                std::cerr << "Parse Error: Unexpected token in style template." << std::endl;
                advance();
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after template body.");
        styleTemplates[node->name] = std::move(node);
    } else if (match(TokenType::AT_ELEMENT)) {
        auto node = std::make_unique<ElementTemplateNode>();
        node->name = currentToken.lexeme;
        consume(TokenType::IDENTIFIER, "Expect template name.");
        consume(TokenType::LEFT_BRACE, "Expect '{' after name.");
        while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
            if (check(TokenType::AT_ELEMENT) || match(TokenType::KEYWORD_INHERIT)) {
                match(TokenType::KEYWORD_INHERIT);
                consume(TokenType::AT_ELEMENT, "Expect '@Element' for inheritance.");
                std::string baseTemplateName = currentToken.lexeme;
                consume(TokenType::IDENTIFIER, "Expect base template name.");
                consume(TokenType::SEMICOLON, "Expect ';' after base template usage.");
                if (elementTemplates.count(baseTemplateName)) {
                    for (const auto& child : elementTemplates.at(baseTemplateName)->children) {
                        auto clonedChild = child->clone();
                        setParent(node.get(), clonedChild.get());
                        node->children.push_back(std::move(clonedChild));
                    }
                } else {
                    std::cerr << "Parse Error: Base element template '" << baseTemplateName << "' not found." << std::endl;
                }
            } else {
                auto child = declaration();
                if (child) {
                    setParent(node.get(), child.get());
                    node->children.push_back(std::move(child));
                }
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after template body.");
        elementTemplates[node->name] = std::move(node);
    } else if (match(TokenType::AT_VAR)) {
        auto node = std::make_unique<VarTemplateNode>();
        node->name = currentToken.lexeme;
        consume(TokenType::IDENTIFIER, "Expect template name.");
        consume(TokenType::LEFT_BRACE, "Expect '{' after name.");
        while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
            if (check(TokenType::IDENTIFIER)) {
                std::string key = currentToken.lexeme;
                advance();
                consume(TokenType::COLON, "Expect ':' after variable name.");
                node->variables[key] = parseValue();
                consume(TokenType::SEMICOLON, "Expect ';' after variable value.");
            } else {
                std::cerr << "Parse Error: Unexpected token in var template." << std::endl;
                advance();
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after template body.");
        varTemplates[node->name] = std::move(node);
    }
    return nullptr;
}

std::string Parser::parseValue() {
    if (check(TokenType::IDENTIFIER) && checkNext(TokenType::LEFT_PAREN)) {
        std::string templateName = currentToken.lexeme;
        advance();
        consume(TokenType::LEFT_PAREN, "Expect '(' after variable template name.");
        std::string varName = currentToken.lexeme;
        consume(TokenType::IDENTIFIER, "Expect variable name inside parentheses.");
        consume(TokenType::RIGHT_PAREN, "Expect ')' after variable name.");
        if (varTemplates.count(templateName) && varTemplates[templateName]->variables.count(varName)) {
            return varTemplates[templateName]->variables[varName];
        }
        std::cerr << "Parse Error: Variable " << templateName << "(" << varName << ") not found." << std::endl;
        return "";
    }

    std::string value;
    // This is a simplified value parser that takes one token.
    // It should be enhanced to handle multi-token values like '16px'.
    if (check(TokenType::STRING) || check(TokenType::IDENTIFIER)) {
        value = currentToken.lexeme;
        advance();
    } else {
        std::cerr << "Parse Error: Expected a value at line " << currentToken.line << std::endl;
        advance();
    }
    return value;
}
