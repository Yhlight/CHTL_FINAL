#include "Parser.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ElementTemplateNode.h"
#include "../CHTLNode/StyleTemplateNode.h"
#include "../CHTLNode/VarTemplateNode.h"
#include "../CHTLNode/CustomElementNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/OriginNode.h"
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
    if (check(TokenType::KEYWORD_CUSTOM)) {
        return customDeclaration();
    }
    if (check(TokenType::KEYWORD_ORIGIN)) {
        return originDeclaration();
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
            std::string templateName = currentToken.lexeme;
            consume(TokenType::IDENTIFIER, "Expect template name after '@Element'.");

            if (match(TokenType::SEMICOLON)) { // Regular template usage
                auto usageNode = std::make_unique<TemplateUsageNode>();
                usageNode->type = TemplateType::ELEMENT;
                usageNode->name = templateName;
                node->children.push_back(std::move(usageNode));
            } else if (match(TokenType::LEFT_BRACE)) { // Custom element usage with specialization
                handleCustomElementUsage(templateName, node.get());
            } else {
                std::cerr << "Parse Error: Expect ';' or '{' after @Element usage at line " << currentToken.line << std::endl;
            }
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
            std::string templateName = currentToken.lexeme;
            consume(TokenType::IDENTIFIER, "Expect template name after '@Style'.");

            if (match(TokenType::SEMICOLON)) { // Regular template usage
                if (styleTemplates.count(templateName)) {
                    const auto& tmpl = styleTemplates.at(templateName);
                    for (const auto& prop : tmpl->properties) {
                        node->properties[prop.first] = prop.second;
                    }
                } else {
                    std::cerr << "Parse Error: Style template '" << templateName << "' not found at line " << currentToken.line << std::endl;
                }
            } else if (match(TokenType::LEFT_BRACE)) { // Custom style usage
                if (!customStyleTemplates.count(templateName)) {
                    std::cerr << "Parse Error: Custom style template '" << templateName << "' not found at line " << currentToken.line << std::endl;
                    // Consume the block to recover
                    while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) { advance(); }
                    consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
                    return nullptr;
                }

                const auto& baseTmpl = customStyleTemplates.at(templateName);
                std::map<std::string, std::string> specializedProps = baseTmpl->properties;
                std::vector<std::string> propsToDelete;

                while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
                    if (match(TokenType::KEYWORD_DELETE)) {
                        do {
                            propsToDelete.push_back(currentToken.lexeme);
                            consume(TokenType::IDENTIFIER, "Expect property name after 'delete'.");
                        } while (match(TokenType::COMMA));
                        consume(TokenType::SEMICOLON, "Expect ';' after delete statement.");
                    } else if (check(TokenType::IDENTIFIER)) {
                        std::string key = currentToken.lexeme;
                        advance();
                        consume(TokenType::COLON, "Expect ':' after property name.");
                        specializedProps[key] = parseValue();
                        consume(TokenType::SEMICOLON, "Expect ';' after property value.");
                    } else {
                        std::cerr << "Parse Error: Unexpected token in custom style usage at line " << currentToken.line << std::endl;
                        advance();
                    }
                }

                // Process deletions
                for (const auto& prop : propsToDelete) {
                    specializedProps.erase(prop);
                }

                // Merge the specialized properties into the current style node
                for (const auto& prop : specializedProps) {
                    node->properties[prop.first] = prop.second;
                }

                consume(TokenType::RIGHT_BRACE, "Expect '}' after custom style usage block.");
            } else {
                std::cerr << "Parse Error: Expect ';' or '{' after @Style usage at line " << currentToken.line << std::endl;
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
            } else if (check(TokenType::AT_STYLE) || check(TokenType::KEYWORD_INHERIT)) { // Style Template Inheritance
                match(TokenType::KEYWORD_INHERIT); // Consume 'inherit' if it exists
                consume(TokenType::AT_STYLE, "Expect '@Style' after inherit keyword or for implicit inheritance.");
                std::string baseTemplateName = currentToken.lexeme;
                consume(TokenType::IDENTIFIER, "Expect base template name.");
                consume(TokenType::SEMICOLON, "Expect ';' after base template usage.");
                if (styleTemplates.count(baseTemplateName)) {
                    const auto& baseTmpl = styleTemplates.at(baseTemplateName);
                    for (const auto& prop : baseTmpl->properties) {
                        node->properties[prop.first] = prop.second;
                    }
                } else {
                    std::cerr << "Parse Error: Base style template '" << baseTemplateName << "' not found at line " << currentToken.line << std::endl;
                }
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
            if (check(TokenType::AT_ELEMENT) || check(TokenType::KEYWORD_INHERIT)) { // Element Template Inheritance
                match(TokenType::KEYWORD_INHERIT); // Consume 'inherit' if it exists
                consume(TokenType::AT_ELEMENT, "Expect '@Element' after inherit keyword or for implicit inheritance.");
                std::string baseTemplateName = currentToken.lexeme;
                consume(TokenType::IDENTIFIER, "Expect base template name.");
                consume(TokenType::SEMICOLON, "Expect ';' after base template usage.");
                if (elementTemplates.count(baseTemplateName)) {
                    const auto& baseTmpl = elementTemplates.at(baseTemplateName);
                    for (const auto& child : baseTmpl->children) {
                        node->children.push_back(child->clone());
                    }
                } else {
                    std::cerr << "Parse Error: Base element template '" << baseTemplateName << "' not found at line " << currentToken.line << std::endl;
                }
            } else {
                auto child = declaration();
                if (child) {
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

std::unique_ptr<BaseNode> Parser::customDeclaration() {
    consume(TokenType::KEYWORD_CUSTOM, "Expect '[Custom]' keyword.");

    if (match(TokenType::AT_STYLE)) {
        auto node = std::make_unique<CustomStyleNode>();
        node->name = currentToken.lexeme;
        consume(TokenType::IDENTIFIER, "Expect custom style name.");
        consume(TokenType::LEFT_BRACE, "Expect '{' after name.");

        while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
            if (check(TokenType::IDENTIFIER)) {
                std::string key = currentToken.lexeme;
                advance();

                if (match(TokenType::COLON)) { // Valued property
                    node->properties[key] = parseValue();
                    consume(TokenType::SEMICOLON, "Expect ';' after valued property.");
                } else { // Valueless property
                    node->valuelessProperties.push_back(key);
                    if (match(TokenType::COMMA)) {
                        // Continue parsing more valueless properties
                        continue;
                    }
                    consume(TokenType::SEMICOLON, "Expect ';' after valueless property list.");
                }
            } else {
                std::cerr << "Parse Error: Unexpected token in custom style at line " << currentToken.line << std::endl;
                advance();
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after body.");
        customStyleTemplates[node->name] = std::move(node);

    } else if (match(TokenType::AT_ELEMENT)) {
        auto node = std::make_unique<CustomElementNode>();
        node->name = currentToken.lexeme;
        consume(TokenType::IDENTIFIER, "Expect custom element name.");
        consume(TokenType::LEFT_BRACE, "Expect '{' after name.");

        while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
            auto child = declaration();
            if (child) {
                node->children.push_back(std::move(child));
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after body.");
        customElementTemplates[node->name] = std::move(node);
    } else {
        std::cerr << "Parse Error: Expected '@Style' or '@Element' after '[Custom]' at line " << currentToken.line << std::endl;
        advance();
    }

    return nullptr; // Custom declarations don't produce a node in the main AST
}

// Forward declarations for helpers
BaseNode* findNodeByTag(BaseNode* root, const std::string& tagName);
void mergeStyles(ElementNode* targetNode, ElementNode* specNode);

void Parser::handleCustomElementUsage(const std::string& templateName, ElementNode* parentNode) {
    if (!customElementTemplates.count(templateName)) {
        std::cerr << "Parse Error: Custom element template '" << templateName << "' not found at line " << currentToken.line << std::endl;
        while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) { advance(); }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after block.");
        return;
    }

    const auto& baseTmpl = customElementTemplates.at(templateName);
    auto clonedRoot = baseTmpl->clone();

    // Parse the specialization block
    while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
        auto specDecl = declaration();
        if (auto* specNode = dynamic_cast<ElementNode*>(specDecl.get())) {
            auto* targetNode = findNodeByTag(clonedRoot.get(), specNode->tagName);
            if (targetNode) {
                if(auto* targetElem = dynamic_cast<ElementNode*>(targetNode)) {
                    mergeStyles(targetElem, specNode);
                }
            } else {
                 std::cerr << "Warning: Could not find element with tag '" << specNode->tagName << "' to specialize at line " << currentToken.line << std::endl;
            }
        }
    }

    // Move the specialized children into the parent node
    if (auto* customElem = dynamic_cast<CustomElementNode*>(clonedRoot.get())) {
        for (auto& child : customElem->children) {
            parentNode->children.push_back(std::move(child));
        }
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after specialization block.");
}


std::unique_ptr<BaseNode> Parser::originDeclaration() {
    consume(TokenType::KEYWORD_ORIGIN, "Expect '[Origin]' keyword.");
    auto node = std::make_unique<OriginNode>();

    if (check(TokenType::AT_STYLE) || check(TokenType::AT_ELEMENT) || check(TokenType::AT_VAR)) {
         node->type = currentToken.lexeme;
         advance();
    } else if (check(TokenType::IDENTIFIER)) { // For custom types like @Vue
        node->type = currentToken.lexeme;
        advance();
    } else {
        consume(TokenType::IDENTIFIER, "Expect origin type identifier (e.g., @Html).");
    }

    consume(TokenType::LEFT_BRACE, "Expect '{' after origin declaration.");

    lexer.enterRawMode();
    Token rawToken = lexer.getNextToken();
    if(rawToken.type == TokenType::RAW_STRING) {
        node->rawContent = rawToken.lexeme;
    }
    lexer.exitRawMode();

    consume(TokenType::RIGHT_BRACE, "Expect '}' after origin block.");

    return node;
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

BaseNode* findNodeByTag(BaseNode* root, const std::string& tagName) {
    if (auto* elem = dynamic_cast<ElementNode*>(root)) {
        if (elem->tagName == tagName) {
            return elem;
        }
        for (const auto& child : elem->children) {
            if (auto* found = findNodeByTag(child.get(), tagName)) {
                return found;
            }
        }
    } else if (auto* customElem = dynamic_cast<CustomElementNode*>(root)) {
         for (const auto& child : customElem->children) {
            if (auto* found = findNodeByTag(child.get(), tagName)) {
                return found;
            }
        }
    }
    return nullptr;
}

void mergeStyles(ElementNode* targetNode, ElementNode* specNode) {
    StyleNode* targetStyleNode = nullptr;
    StyleNode* specStyleNode = nullptr;

    // Find the style node in the spec
    for (const auto& child : specNode->children) {
        if (auto* sn = dynamic_cast<StyleNode*>(child.get())) {
            specStyleNode = sn;
            break;
        }
    }
    if (!specStyleNode) return; // No styles to merge

    // Find or create the style node in the target
    for (const auto& child : targetNode->children) {
        if (auto* sn = dynamic_cast<StyleNode*>(child.get())) {
            targetStyleNode = sn;
            break;
        }
    }
    if (!targetStyleNode) {
        targetNode->children.push_back(std::make_unique<StyleNode>());
        targetStyleNode = dynamic_cast<StyleNode*>(targetNode->children.back().get());
    }

    // Merge properties
    for (const auto& prop : specStyleNode->properties) {
        targetStyleNode->properties[prop.first] = prop.second;
    }
}
