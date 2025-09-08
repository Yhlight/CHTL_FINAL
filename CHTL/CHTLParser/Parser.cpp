#include "Parser.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ElementTemplateNode.h"
#include "../CHTLNode/StyleTemplateNode.h"
#include "../CHTLNode/VarTemplateNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include <iostream>

Parser::Parser(Lexer& lexer) : lexer(lexer) {
    advance();
    advance(); // Initialize both currentToken and nextToken
}

void Parser::advance() {
    currentToken = nextToken;
    nextToken = lexer.getNextToken();
    // Add error handling for lexical errors if needed
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
    // Proper error handling should be implemented here
    std::cerr << "Parse Error: " << message << " at line " << currentToken.line << ". Got " << currentToken.lexeme << std::endl;
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
            root->children.push_back(std::move(decl));
        }
    }
    return root;
}

std::unique_ptr<BaseNode> Parser::declaration() {
    if (check(TokenType::KEYWORD_TEMPLATE)) {
        return templateDeclaration();
    }
    if (check(TokenType::IDENTIFIER)) {
        if (checkNext(TokenType::LEFT_BRACE)) {
            return element();
        }
    }
    if (check(TokenType::TEXT)) {
        return textNode();
    }
    if (check(TokenType::STYLE)) {
        return styleNode();
    }

    std::cerr << "Parse Error: Unexpected token " << currentToken.lexeme << " at line " << currentToken.line << std::endl;
    advance();
    return nullptr;
}

std::unique_ptr<ElementNode> Parser::element() {
    auto node = std::make_unique<ElementNode>();
    node->tagName = currentToken.lexeme;
    advance(); // consume identifier

    consume(TokenType::LEFT_BRACE, "Expect '{' after element name.");

    while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
        if (check(TokenType::IDENTIFIER)) {
            if (checkNext(TokenType::COLON) || checkNext(TokenType::EQUAL)) {
                attributes(*node);
            } else if (checkNext(TokenType::LEFT_BRACE)) {
                auto child = element();
                if (child) {
                    node->children.push_back(std::move(child));
                }
            } else {
                std::cerr << "Parse Error: Unexpected token " << nextToken.lexeme << " after identifier " << currentToken.lexeme << " at line " << currentToken.line << std::endl;
                advance();
            }
        } else if (check(TokenType::TEXT)) {
            auto child = textNode();
            if (child) {
                node->children.push_back(std::move(child));
            }
        } else if (check(TokenType::STYLE)) {
            auto child = styleNode();
            if (child) {
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
            std::cerr << "Parse Error: Unexpected token " << currentToken.lexeme << " in element " << node->tagName << " at line " << currentToken.line << std::endl;
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
            advance(); // consume property name

            consume(TokenType::COLON, "Expect ':' after style property name.");
            node->properties[key] = parseValue();
            consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
        } else if (match(TokenType::AT_STYLE)) {
            // This is a bit of a hack. A StyleNode can't have a TemplateUsageNode as a child.
            // The template usage should be handled by the generator by merging properties.
            // For now, I'll just parse it and the generator will have to handle it.
            // A better approach would be to have a separate list for template usages.
            // Let's create a new node type that can hold both properties and template usages.
            // That's too much refactoring for now.

            // The simplest approach is to expand the style template here in the parser.
            // This avoids changing the generator logic too much.

            // Let's try that.
            std::string templateName = currentToken.lexeme;
            consume(TokenType::IDENTIFIER, "Expect template name after '@Style'.");
            consume(TokenType::SEMICOLON, "Expect ';' after style template usage.");

            if (styleTemplates.count(templateName)) {
                const auto& tmpl = styleTemplates.at(templateName);
                for (const auto& prop : tmpl->properties) {
                    node->properties[prop.first] = prop.second;
                }
            } else {
                std::cerr << "Parse Error: Style template '" << templateName << "' not found at line " << currentToken.line << std::endl;
            }
        } else {
            std::cerr << "Parse Error: Unexpected token " << currentToken.lexeme << " in style block at line " << currentToken.line << std::endl;
            advance(); // Advance to avoid infinite loops
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after style block.");
    return node;
}

void Parser::attributes(ElementNode& element) {
    while (check(TokenType::IDENTIFIER) && (checkNext(TokenType::COLON) || checkNext(TokenType::EQUAL))) {
        std::string key = currentToken.lexeme;
        advance(); // consume key

        if (match(TokenType::COLON) || match(TokenType::EQUAL)) {
            element.attributes[key] = parseValue();
            consume(TokenType::SEMICOLON, "Expect ';' after attribute value.");
        }
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
                consume(TokenType::COLON, "Expect ':' after style property name.");
                if (check(TokenType::STRING) || check(TokenType::IDENTIFIER)) {
                    node->properties[key] = currentToken.lexeme;
                    advance();
                } else {
                    std::cerr << "Parse Error: Expected style property value at line " << currentToken.line << std::endl;
                    advance();
                }
                consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
            } else {
                std::cerr << "Parse Error: Unexpected token in style template at line " << currentToken.line << std::endl;
                advance();
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after template body.");
        styleTemplates[node->name] = std::move(node);

    } else if (match(TokenType::AT_ELEMENT)) {
        auto node = std::make_unique<ElementTemplateNode>();
        node->name = currentToken.lexeme;
        consume(TokenType::IDENTIFIER, "Expect template name.");
        consume(TokenType::LEFT_BRACE, "Expect '{' after template name.");

        while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
            auto child = declaration();
            if (child) {
                node->children.push_back(std::move(child));
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after template body.");
        elementTemplates[node->name] = std::move(node);

    } else if (match(TokenType::AT_VAR)) {
        auto node = std::make_unique<VarTemplateNode>();
        node->name = currentToken.lexeme;
        consume(TokenType::IDENTIFIER, "Expect template name.");
        consume(TokenType::LEFT_BRACE, "Expect '{' after template name.");

        while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
            if (check(TokenType::IDENTIFIER)) {
                std::string key = currentToken.lexeme;
                advance();
                consume(TokenType::COLON, "Expect ':' after variable name.");
                if (check(TokenType::STRING) || check(TokenType::IDENTIFIER)) {
                    node->variables[key] = currentToken.lexeme;
                    advance();
                } else {
                    std::cerr << "Parse Error: Expected variable value at line " << currentToken.line << std::endl;
                    advance();
                }
                consume(TokenType::SEMICOLON, "Expect ';' after variable value.");
            } else {
                std::cerr << "Parse Error: Unexpected token in var template at line " << currentToken.line << std::endl;
                advance();
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after template body.");
        varTemplates[node->name] = std::move(node);
    } else {
        std::cerr << "Parse Error: Expected '@Style', '@Element', or '@Var' after '[Template]' at line " << currentToken.line << std::endl;
        advance();
    }

    return nullptr; // Template declarations don't produce a node in the main AST
}

std::string Parser::parseValue() {
    if (check(TokenType::IDENTIFIER) && checkNext(TokenType::LEFT_PAREN)) {
        std::string templateName = currentToken.lexeme;
        advance(); // consume template name
        consume(TokenType::LEFT_PAREN, "Expect '(' after variable template name.");

        std::string varName = currentToken.lexeme;
        consume(TokenType::IDENTIFIER, "Expect variable name inside parentheses.");
        consume(TokenType::RIGHT_PAREN, "Expect ')' after variable name.");

        if (varTemplates.count(templateName)) {
            const auto& tmpl = varTemplates.at(templateName);
            if (tmpl->variables.count(varName)) {
                return tmpl->variables.at(varName);
            } else {
                std::cerr << "Parse Error: Variable '" << varName << "' not found in template '" << templateName << "' at line " << currentToken.line << std::endl;
                return ""; // Return empty string on error
            }
        } else {
            std::cerr << "Parse Error: Variable template '" << templateName << "' not found at line " << currentToken.line << std::endl;
            return ""; // Return empty string on error
        }
    }

    // If not a variable template usage, it's a simple value
    if (check(TokenType::STRING) || check(TokenType::IDENTIFIER)) {
        std::string value = currentToken.lexeme;
        advance();
        return value;
    }

    std::cerr << "Parse Error: Expected a value (string, identifier, or variable usage) at line " << currentToken.line << std::endl;
    advance(); // Consume the unexpected token
    return ""; // Return empty string on error
}
