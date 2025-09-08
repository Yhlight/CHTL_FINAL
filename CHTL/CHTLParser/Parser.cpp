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

Parser::Selector Parser::parseSelector() {
    Selector selector;
    selector.tagName = currentToken.lexeme;
    consume(TokenType::IDENTIFIER, "Expect a tag name for the selector.");

    if (match(TokenType::LEFT_BRACKET)) {
        if (check(TokenType::NUMBER)) {
            try {
                selector.index = std::stoi(currentToken.lexeme);
            } catch (const std::invalid_argument& e) {
                std::cerr << "Parse Error: Invalid number for index at line " << currentToken.line << std::endl;
                selector.index = -1;
            } catch (const std::out_of_range& e) {
                std::cerr << "Parse Error: Index number out of range at line " << currentToken.line << std::endl;
                selector.index = -1;
            }
            advance();
        } else {
            std::cerr << "Parse Error: Expect a number inside brackets for index." << std::endl;
        }
        consume(TokenType::RIGHT_BRACKET, "Expect ']' after index.");
    }
    return selector;
}

BaseNode* Parser::findNodeBySelector(BaseNode* root, const Parser::Selector& selector) {
    if (!root) return nullptr;

    std::vector<BaseNode*> candidates;
    std::vector<BaseNode*> queue;

    if (auto* customElem = dynamic_cast<CustomElementNode*>(root)) {
        for (const auto& child : customElem->children) {
            queue.push_back(child.get());
        }
    } else if (auto* elem = dynamic_cast<ElementNode*>(root)) {
        for (const auto& child : elem->children) {
            queue.push_back(child.get());
        }
    }

    size_t head = 0;
    while(head < queue.size()) {
        BaseNode* current = queue[head++];
        if (auto* elem = dynamic_cast<ElementNode*>(current)) {
            if (elem->tagName == selector.tagName) {
                candidates.push_back(elem);
            }
            for (const auto& child : elem->children) {
                queue.push_back(child.get());
            }
        }
    }

    if (selector.index != -1) {
        if (selector.index >= 0 && static_cast<size_t>(selector.index) < candidates.size()) {
            return candidates[selector.index];
        } else {
            std::cerr << "Parse Warning: Index " << selector.index << " for tag '" << selector.tagName << "' is out of bounds." << std::endl;
            return nullptr;
        }
    }

    if (!candidates.empty()) {
        if (candidates.size() > 1) {
             std::cerr << "Parse Warning: Multiple elements with tag '" << selector.tagName << "' found. Use an index to specify one." << std::endl;
        }
        return candidates[0];
    }

    return nullptr;
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
        int braceCount = 1;
        while (braceCount > 0 && !check(TokenType::END_OF_FILE)) { advance(); if(check(TokenType::LEFT_BRACE)) braceCount++; if(check(TokenType::RIGHT_BRACE)) braceCount--; }
        return;
    }
    const auto& baseTmpl = customElementTemplates.at(templateName);
    auto clonedRoot = baseTmpl->clone();
    auto* clonedCustomElement = dynamic_cast<CustomElementNode*>(clonedRoot.get());

    if (!clonedCustomElement) {
        std::cerr << "Internal Error: Cloned template is not a CustomElementNode." << std::endl;
        return;
    }

    // --- Phase 1: Collect Patches ---
    std::vector<Patch> patches;
    while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
        Patch patch;
        patch.payload = std::make_unique<ElementNode>();

        if (check(TokenType::IDENTIFIER)) {
            patch.type = PatchType::Modify;
            patch.selector = parseSelector();
            consume(TokenType::LEFT_BRACE, "Expect '{' after selector.");
            while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
                if (check(TokenType::IDENTIFIER) && checkNext(TokenType::COLON)) attributes(*patch.payload);
                else if (check(TokenType::STYLE)) patch.payload->children.push_back(styleNode());
                else {
                    auto child = declaration();
                    if(child) patch.payload->children.push_back(std::move(child));
                }
            }
            consume(TokenType::RIGHT_BRACE, "Expect '}' after specialization body.");
        } else if (match(TokenType::KEYWORD_INSERT)) {
            TokenType position = currentToken.type;
            advance();
            if (position == TokenType::KEYWORD_AFTER) patch.type = PatchType::InsertAfter;
            else if (position == TokenType::KEYWORD_BEFORE) patch.type = PatchType::InsertBefore;
            else if (position == TokenType::KEYWORD_REPLACE) patch.type = PatchType::Replace;
            else if (position == TokenType::KEYWORD_ATTOP) patch.type = PatchType::InsertAtTop;
            else if (position == TokenType::KEYWORD_ATBOTTOM) patch.type = PatchType::InsertAtBottom;

            if (patch.type != PatchType::InsertAtTop && patch.type != PatchType::InsertAtBottom) {
                patch.selector = parseSelector();
            }

            consume(TokenType::LEFT_BRACE, "Expect '{' for insert block.");
            while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
                auto newNode = declaration();
                if(newNode) patch.payload->children.push_back(std::move(newNode));
            }
            consume(TokenType::RIGHT_BRACE, "Expect '}' after insert block.");
        } else if (match(TokenType::KEYWORD_DELETE)) {
            patch.type = PatchType::Delete;
            patch.selector = parseSelector();
            patch.payload = nullptr;
            consume(TokenType::SEMICOLON, "Expect ';' after delete selector.");
        } else {
             std::cerr << "Parse Error: Unexpected token '" << currentToken.lexeme << "' in specialization block." << std::endl;
             advance();
             continue;
        }
        patches.push_back(std::move(patch));
    }

    // --- Phase 2: Apply Patches ---
    for (auto& patch : patches) {
        BaseNode* targetNode = (patch.type != PatchType::InsertAtTop && patch.type != PatchType::InsertAtBottom)
                             ? findNodeBySelector(clonedCustomElement, patch.selector)
                             : nullptr;

        std::vector<std::unique_ptr<BaseNode>>* parentChildren = nullptr;
        BaseNode* parentNodeForPatch = nullptr;

        if (patch.type != PatchType::InsertAtTop && patch.type != PatchType::InsertAtBottom) {
            if (!targetNode || !targetNode->parent) {
                std::cerr << "Parse Warning: Target for patch not found or has no parent." << std::endl;
                continue;
            }
            parentNodeForPatch = targetNode->parent;
            if(auto* p = dynamic_cast<ElementNode*>(parentNodeForPatch)) parentChildren = &p->children;
            else if(auto* p = dynamic_cast<CustomElementNode*>(parentNodeForPatch)) parentChildren = &p->children;
        } else {
            parentNodeForPatch = clonedCustomElement;
            parentChildren = &clonedCustomElement->children;
        }

        if (!parentChildren) {
            std::cerr << "Internal Error: Could not get children list from parent." << std::endl;
            continue;
        }

        auto it = (targetNode) ? std::find_if(parentChildren->begin(), parentChildren->end(),
            [&](const std::unique_ptr<BaseNode>& p) { return p.get() == targetNode; }) : parentChildren->end();

        switch (patch.type) {
            case PatchType::Modify:
                if (auto* target = dynamic_cast<ElementNode*>(targetNode)) {
                    for (const auto& attr : patch.payload->attributes) target->attributes[attr.first] = attr.second;
                    mergeStyles(target, patch.payload.get());
                    for (auto& child : patch.payload->children) {
                        if (dynamic_cast<StyleNode*>(child.get())) continue;
                        setParent(target, child.get());
                        target->children.push_back(std::move(child));
                    }
                }
                break;
            case PatchType::InsertAfter:
                 if (it != parentChildren->end()) {
                    for(auto& node : patch.payload->children) setParent(parentNodeForPatch, node.get());
                    parentChildren->insert(std::next(it), std::make_move_iterator(patch.payload->children.begin()), std::make_move_iterator(patch.payload->children.end()));
                 }
                break;
            case PatchType::InsertBefore:
                 if (it != parentChildren->end()) {
                    for(auto& node : patch.payload->children) setParent(parentNodeForPatch, node.get());
                    parentChildren->insert(it, std::make_move_iterator(patch.payload->children.begin()), std::make_move_iterator(patch.payload->children.end()));
                 }
                break;
            case PatchType::Replace:
                if (it != parentChildren->end()) {
                    for(auto& node : patch.payload->children) setParent(parentNodeForPatch, node.get());
                    it = parentChildren->erase(it);
                    parentChildren->insert(it, std::make_move_iterator(patch.payload->children.begin()), std::make_move_iterator(patch.payload->children.end()));
                }
                break;
            case PatchType::InsertAtTop:
            case PatchType::InsertAtBottom:
                {
                    BaseNode* insertionParent = parentNodeForPatch;
                    std::vector<std::unique_ptr<BaseNode>>* insertionChildren = parentChildren;

                    if (clonedCustomElement->children.size() == 1) {
                        if (auto* singleRoot = dynamic_cast<ElementNode*>(clonedCustomElement->children[0].get())) {
                            insertionParent = singleRoot;
                            insertionChildren = &singleRoot->children;
                        }
                    }
                    for(auto& node : patch.payload->children) setParent(insertionParent, node.get());

                    if(patch.type == PatchType::InsertAtTop) {
                        insertionChildren->insert(insertionChildren->begin(), std::make_move_iterator(patch.payload->children.begin()), std::make_move_iterator(patch.payload->children.end()));
                    } else {
                        insertionChildren->insert(insertionChildren->end(), std::make_move_iterator(patch.payload->children.begin()), std::make_move_iterator(patch.payload->children.end()));
                    }
                }
                break;
            case PatchType::Delete:
                if (it != parentChildren->end()) {
                    parentChildren->erase(it);
                }
                break;
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
