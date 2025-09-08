#include "Parser.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ElementTemplateNode.h"
#include "../CHTLNode/StyleTemplateNode.h"
#include "../CHTLNode/VarTemplateNode.h"
#include "../CHTLNode/CustomElementNode.h"
#include "../CHTLNode/CustomStyleTemplateNode.h"
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
    previousToken = currentToken;
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
    if (check(TokenType::KEYWORD_CUSTOM)) return customDeclaration();
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
        if (check(TokenType::IDENTIFIER) && checkNext(TokenType::COLON)) {
            std::string key = currentToken.lexeme;
            advance();
            consume(TokenType::COLON, "Expect ':' after style property name.");
            node->properties[key] = parseValue();
            consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
        } else if (match(TokenType::AT_STYLE)) {
            std::string templateName = currentToken.lexeme;
            consume(TokenType::IDENTIFIER, "Expect template name after '@Style'.");

            if (match(TokenType::LEFT_BRACE)) {
                // --- Custom Style Specialization ---
                if (!customStyleTemplates.count(templateName)) {
                    std::cerr << "Parse Error: Custom style template '" << templateName << "' not found." << std::endl;
                    // Skip block
                    int braceCount = 1;
                    while (braceCount > 0 && !check(TokenType::END_OF_FILE)) { advance(); if(check(TokenType::LEFT_BRACE)) braceCount++; if(check(TokenType::RIGHT_BRACE)) braceCount--; }
                    continue;
                }

                const auto& tmpl = customStyleTemplates.at(templateName);
                auto specializedProps = tmpl->properties; // Clone base properties

                while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
                    if (match(TokenType::KEYWORD_DELETE)) {
                        std::string keyToDelete = currentToken.lexeme;
                        consume(TokenType::IDENTIFIER, "Expect property name after 'delete'.");
                        consume(TokenType::SEMICOLON, "Expect ';' after property name.");
                        specializedProps.erase(keyToDelete);
                    } else if (check(TokenType::IDENTIFIER)) {
                        std::string key = currentToken.lexeme;
                        advance();
                        consume(TokenType::COLON, "Expect ':' for property specialization.");
                        specializedProps[key] = parseValue();
                        consume(TokenType::SEMICOLON, "Expect ';' after property value.");
                    } else {
                        std::cerr << "Parse Error: Unexpected token in custom style specialization." << std::endl;
                        advance();
                    }
                }
                consume(TokenType::RIGHT_BRACE, "Expect '}' after specialization block.");

                // Apply specialized properties to the node
                for (const auto& prop : specializedProps) {
                    node->properties[prop.first] = prop.second;
                }

            } else if (match(TokenType::SEMICOLON)) {
                // --- Simple Template Inclusion ---
                if (styleTemplates.count(templateName)) {
                    for (const auto& prop : styleTemplates.at(templateName)->properties) {
                        node->properties[prop.first] = prop.second;
                    }
                } else {
                     std::cerr << "Parse Error: Style template '" << templateName << "' not found." << std::endl;
                }
            } else {
                std::cerr << "Parse Error: Expect '{' or ';' after @Style usage." << std::endl;
            }
        } else {
            std::cerr << "Parse Error: Unexpected token '" << currentToken.lexeme << "' in style block." << std::endl;
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
            } else if (check(TokenType::AT_STYLE) || check(TokenType::KEYWORD_INHERIT)) {
                bool isInherit = match(TokenType::KEYWORD_INHERIT);
                if (!isInherit) consume(TokenType::AT_STYLE, "Expect '@Style' for inheritance.");
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
            if (check(TokenType::AT_ELEMENT) || check(TokenType::KEYWORD_INHERIT)) {
                bool isInherit = match(TokenType::KEYWORD_INHERIT);
                if(!isInherit) consume(TokenType::AT_ELEMENT, "Expect '@Element' for inheritance.");
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

BaseNode* Parser::findNodeByTag(BaseNode* root, const std::string& tagName) {
    if (!root) return nullptr;

    std::vector<BaseNode*> queue;

    // Seed the queue with initial children
    if (auto* customElem = dynamic_cast<CustomElementNode*>(root)) {
        for (const auto& child : customElem->children) {
            queue.push_back(child.get());
        }
    } else if (auto* elem = dynamic_cast<ElementNode*>(root)) {
        // This case is for searching within a standard element, if ever needed.
        for (const auto& child : elem->children) {
            queue.push_back(child.get());
        }
    } else {
        // Not a container, can't search.
        return nullptr;
    }

    size_t head = 0;
    while(head < queue.size()) {
        BaseNode* current = queue[head++];
        if (auto* elem = dynamic_cast<ElementNode*>(current)) {
            if (elem->tagName == tagName) {
                return elem; // Found it
            }
            // Add this element's children to the queue for deeper searching
            for (const auto& child : elem->children) {
                queue.push_back(child.get());
            }
        }
    }

    return nullptr; // Not found
}

void Parser::mergeStyles(ElementNode* targetNode, ElementNode* specNode) {
    StyleNode* targetStyleNode = nullptr;
    StyleNode* specStyleNode = nullptr;

    for (const auto& child : specNode->children) {
        if (auto* sn = dynamic_cast<StyleNode*>(child.get())) {
            specStyleNode = sn;
            break;
        }
    }
    if (!specStyleNode) return;

    for (const auto& child : targetNode->children) {
        if (auto* sn = dynamic_cast<StyleNode*>(child.get())) {
            targetStyleNode = sn;
            break;
        }
    }
    if (!targetStyleNode) {
        auto newStyleNode = std::make_unique<StyleNode>();
        setParent(targetNode, newStyleNode.get());
        targetStyleNode = newStyleNode.get();
        targetNode->children.push_back(std::move(newStyleNode));
    }

    for (const auto& prop : specStyleNode->properties) {
        targetStyleNode->properties[prop.first] = prop.second;
    }
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
    if (check(TokenType::STRING) || check(TokenType::IDENTIFIER)) {
        value = currentToken.lexeme;
        advance();
    } else {
        std::cerr << "Parse Error: Expected a value at line " << currentToken.line << std::endl;
        advance();
    }
    return value;
}

void Parser::handleCustomElementUsage(const std::string& templateName, ElementNode* parentNode) {
    if (!customElementTemplates.count(templateName)) {
        std::cerr << "Parse Error: Custom element template '" << templateName << "' not found." << std::endl;
        // The calling function `element()` already matched a '{', so we need to skip until the matching '}'
        int braceCount = 1;
        while (braceCount > 0 && !check(TokenType::END_OF_FILE)) {
            advance();
            if (check(TokenType::LEFT_BRACE)) braceCount++;
            if (check(TokenType::RIGHT_BRACE)) braceCount--;
        }
        return;
    }
    const auto& baseTmpl = customElementTemplates.at(templateName);
    auto clonedRoot = baseTmpl->clone();
    auto* clonedCustomElement = dynamic_cast<CustomElementNode*>(clonedRoot.get());

    if (!clonedCustomElement) {
        std::cerr << "Internal Error: Cloned template is not a CustomElementNode." << std::endl;
        return;
    }

    // The call site already consumed the '{', so we parse the content.
    while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
        if (check(TokenType::IDENTIFIER) && checkNext(TokenType::LEFT_BRACE)) {
            auto specNode = element();
            if (!specNode) continue;

            BaseNode* targetBaseNode = findNodeByTag(clonedCustomElement, specNode->tagName);
            if (auto* targetNode = dynamic_cast<ElementNode*>(targetBaseNode)) {

                for (const auto& attr : specNode->attributes) {
                    targetNode->attributes[attr.first] = attr.second;
                }

                mergeStyles(targetNode, specNode.get());

                for (auto& child : specNode->children) {
                    if (dynamic_cast<StyleNode*>(child.get())) {
                        continue;
                    }
                    setParent(targetNode, child.get());
                    targetNode->children.push_back(std::move(child));
                }

            } else {
                std::cerr << "Parse Warning: Element '" << specNode->tagName << "' in specialization not found in custom template '" << templateName << "'. Ignoring." << std::endl;
            }
        } else {
            std::cerr << "Parse Error: Unexpected token '" << currentToken.lexeme << "' in specialization block for '" << templateName << "'." << std::endl;
            advance();
        }
    }

    for (auto& child : clonedCustomElement->children) {
        setParent(parentNode, child.get());
        parentNode->children.push_back(std::move(child));
    }

    consume(TokenType::RIGHT_BRACE, "Expect '}' after custom element specialization block.");
}

std::unique_ptr<BaseNode> Parser::customDeclaration() {
    consume(TokenType::KEYWORD_CUSTOM, "Expect '[Custom]' keyword.");

    if (match(TokenType::AT_ELEMENT)) {
        auto node = std::make_unique<CustomElementNode>();
        node->name = currentToken.lexeme;
        consume(TokenType::IDENTIFIER, "Expect custom element name.");
        consume(TokenType::LEFT_BRACE, "Expect '{' after name.");

        while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
            auto child = declaration();
            if (child) {
                setParent(node.get(), child.get());
                node->children.push_back(std::move(child));
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after body.");
        customElementTemplates[node->name] = std::move(node);
    } else if (match(TokenType::AT_STYLE)) {
        auto node = std::make_unique<CustomStyleTemplateNode>();
        node->name = currentToken.lexeme;
        consume(TokenType::IDENTIFIER, "Expect custom style name.");
        consume(TokenType::LEFT_BRACE, "Expect '{' after name.");

        while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
            if (check(TokenType::IDENTIFIER)) {
                std::string key = currentToken.lexeme;
                advance();
                if (match(TokenType::COLON)) {
                    // Property with value
                    node->properties[key] = parseValue();
                    consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
                } else if (match(TokenType::SEMICOLON)) {
                    // Valueless property
                    node->valuelessProperties.push_back(key);
                } else {
                    std::cerr << "Parse Error: Unexpected token after property name '" << key << "' in custom style." << std::endl;
                }
            } else {
                std::cerr << "Parse Error: Unexpected token in custom style definition." << std::endl;
                advance();
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after custom style body.");
        customStyleTemplates[node->name] = std::move(node);

    } else {
        std::cerr << "Parse Error: Expected '@Element' or '@Style' after '[Custom]' at line " << currentToken.line << std::endl;
        advance();
    }

    return nullptr;
}
