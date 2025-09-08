#include "Parser.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/ElementTemplateNode.h"
#include "../CHTLNode/StyleTemplateNode.h"
#include "../CHTLNode/VarTemplateNode.h"
#include "../CHTLNode/CustomElementNode.h"
#include "../CHTLNode/CustomStyleTemplateNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/StyleRuleNode.h"
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
    if (check(TokenType::KEYWORD_ORIGIN)) return originDeclaration();
    if (check(TokenType::IDENTIFIER) && checkNext(TokenType::LEFT_BRACE)) return element();
    if (check(TokenType::TEXT)) return textNode();
    if (check(TokenType::STYLE)) return styleNode();

    std::cerr << "Parse Error: Unexpected token " << currentToken.lexeme << " at line " << currentToken.line << std::endl;
    advance();
    return nullptr;
}

// =================================================================
// Expression Parsing
// =================================================================

std::unique_ptr<ExpressionNode> Parser::parseExpression() {
    return parseTernary();
}

std::unique_ptr<ExpressionNode> Parser::parseTernary() {
    auto expr = parseLogicalOr();

    if (match(TokenType::QUESTION)) {
        auto trueExpr = parseTernary();
        consume(TokenType::COLON, "Expect ':' after true expression in ternary operator.");
        auto falseExpr = parseTernary();
        return std::make_unique<TernaryOpNode>(std::move(expr), std::move(trueExpr), std::move(falseExpr));
    }

    return expr;
}

std::unique_ptr<ExpressionNode> Parser::parseLogicalOr() {
    auto expr = parseLogicalAnd();
    while (match(TokenType::OR_OR)) {
        Token op = previousToken;
        auto right = parseLogicalAnd();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> Parser::parseLogicalAnd() {
    auto expr = parseEquality();
    while (match(TokenType::AND_AND)) {
        Token op = previousToken;
        auto right = parseEquality();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> Parser::parseEquality() {
    auto expr = parseComparison();
    while (match(TokenType::EQUAL_EQUAL) || match(TokenType::NOT_EQUAL)) {
        Token op = previousToken;
        auto right = parseComparison();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> Parser::parseComparison() {
    auto expr = parseTerm();
    while (match(TokenType::GREATER) || match(TokenType::GREATER_EQUAL) || match(TokenType::LESS) || match(TokenType::LESS_EQUAL)) {
        Token op = previousToken;
        auto right = parseTerm();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> Parser::parseTerm() {
    auto expr = parseFactor();
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        Token op = previousToken;
        auto right = parseFactor();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> Parser::parseFactor() {
    auto expr = parsePrimary();
    while (match(TokenType::STAR) || match(TokenType::SLASH)) {
        Token op = previousToken;
        auto right = parsePrimary();
        expr = std::make_unique<BinaryOpNode>(std::move(expr), op, std::move(right));
    }
    return expr;
}

std::unique_ptr<ExpressionNode> Parser::parsePrimary() {
    if (match(TokenType::STRING) || match(TokenType::NUMBER)) {
        return std::make_unique<LiteralNode>(previousToken);
    }

    if (match(TokenType::IDENTIFIER)) {
        // This could be a literal identifier (like 'red') or a property access (like 'width')
        // The generator will have to figure out which it is.
        if (check(TokenType::LEFT_PAREN)) {
            // This is a variable template usage
            std::string templateName = previousToken.lexeme;
            advance(); // consume '('
            std::string varName = currentToken.lexeme;
            consume(TokenType::IDENTIFIER, "Expect variable name inside parentheses.");
            if (match(TokenType::EQUAL)) {
                auto newValue = parseExpression();
                consume(TokenType::RIGHT_PAREN, "Expect ')' after specialized value.");
                return newValue;
            } else {
                consume(TokenType::RIGHT_PAREN, "Expect ')' after variable name.");
                if (varTemplates.count(templateName) && varTemplates[templateName]->variables.count(varName)) {
                    Token valueToken = {TokenType::STRING, varTemplates[templateName]->variables[varName], currentToken.line, currentToken.column, currentToken.position};
                    return std::make_unique<LiteralNode>(valueToken);
                }
                std::cerr << "Parse Error: Variable " << templateName << "(" << varName << ") not found." << std::endl;
                return nullptr;
            }
        }
        return std::make_unique<PropertyAccessNode>(previousToken);
    }

    if (match(TokenType::DOT)) {
        // This is for selectors like .box.width
        std::string selector = "." + currentToken.lexeme;
        advance();
        consume(TokenType::DOT, "Expect '.' between selector and property.");
        selector += "." + currentToken.lexeme;
        advance();
        return std::make_unique<PropertyAccessNode>(Token{TokenType::IDENTIFIER, selector, previousToken.line, previousToken.column, previousToken.position});
    }

    if (match(TokenType::LEFT_PAREN)) {
        auto expr = parseExpression();
        consume(TokenType::RIGHT_PAREN, "Expect ')' after expression.");
        return expr;
    }

    // Default case for simple unquoted values
    auto literal = std::make_unique<LiteralNode>(currentToken);
    advance();
    return literal;
}

std::unique_ptr<BaseNode> Parser::originDeclaration() {
    consume(TokenType::KEYWORD_ORIGIN, "Expect '[Origin]' keyword.");
    consume(TokenType::AT, "Expect '@' before origin type.");
    consume(TokenType::IDENTIFIER, "Expect type name after '@'.");

    if (check(TokenType::IDENTIFIER)) {
        advance();
    }

    consume(TokenType::LEFT_BRACE, "Expect '{' after [Origin] declaration.");

    size_t start = previousToken.position + previousToken.lexeme.length();

    const std::string& source = lexer.getSource();
    int braceCount = 1;
    size_t end = start;
    while (braceCount > 0 && end < source.length()) {
        if (source[end] == '{') {
            braceCount++;
        } else if (source[end] == '}') {
            braceCount--;
        }
        end++;
    }

    if (braceCount > 0) {
        std::cerr << "Parse Error: Unterminated [Origin] block." << std::endl;
        while(!check(TokenType::END_OF_FILE)) advance();
        return nullptr;
    }

    std::string rawContent = source.substr(start, end - 1 - start);

    lexer.setPosition(end);

    advance();
    advance();

    auto node = std::make_unique<OriginNode>();
    node->content = rawContent;
    return node;
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
        } else if (match(TokenType::AT)) {
            consume(TokenType::IDENTIFIER, "Expect 'Element' after '@'.");
            std::string templateName = currentToken.lexeme;
            consume(TokenType::IDENTIFIER, "Expect template name after '@Element'.");

            if (match(TokenType::SEMICOLON)) {
                auto usageNode = std::make_unique<TemplateUsageNode>();
                usageNode->type = TemplateType::ELEMENT;
                usageNode->name = templateName;
                node->children.push_back(std::move(usageNode));
            } else if (match(TokenType::LEFT_BRACE)) {
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
            node->inlineProperties[key] = parseExpression();
            consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
        } else if (check(TokenType::DOT) || check(TokenType::HASH) || check(TokenType::AMPERSAND)) {
            auto rule = std::make_unique<StyleRuleNode>();
            std::string selector_str;

            if (match(TokenType::AMPERSAND)) {
                selector_str = "&";
                if (check(TokenType::COLON)) {
                    selector_str += currentToken.lexeme;
                    advance();
                    if (check(TokenType::COLON)) {
                        selector_str += currentToken.lexeme;
                        advance();
                    }
                    selector_str += currentToken.lexeme;
                    consume(TokenType::IDENTIFIER, "Expect pseudo-class/element name after ':'.");
                }
            } else {
                selector_str = currentToken.lexeme;
                advance();
                selector_str += currentToken.lexeme;
                consume(TokenType::IDENTIFIER, "Expect identifier after '.' or '#'.");
            }

            rule->selector = selector_str;

            consume(TokenType::LEFT_BRACE, "Expect '{' after selector.");
            while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
                std::string key = currentToken.lexeme;
                consume(TokenType::IDENTIFIER, "Expect property name.");
                consume(TokenType::COLON, "Expect ':' after property name.");
                auto expr = parseValue();
                if (auto* lit = dynamic_cast<LiteralNode*>(expr.get())) {
                    rule->properties[key] = lit->value.lexeme;
                } else {
                    std::cerr << "Parse Warning: Expressions are not supported in nested style rules." << std::endl;
                }
                consume(TokenType::SEMICOLON, "Expect ';' after property value.");
            }
            consume(TokenType::RIGHT_BRACE, "Expect '}' after rule block.");
            node->rules.push_back(std::move(rule));
        } else if (match(TokenType::AT)) {
            consume(TokenType::IDENTIFIER, "Expect 'Style' after '@'.");
            std::string templateName = currentToken.lexeme;
            consume(TokenType::IDENTIFIER, "Expect template name after '@Style'.");

            if (match(TokenType::SEMICOLON)) {
                 if (styleTemplates.count(templateName)) {
                    for (const auto& prop : styleTemplates.at(templateName)->properties) {
                        Token token = {TokenType::STRING, prop.second, currentToken.line, currentToken.column, currentToken.position};
                        node->inlineProperties[prop.first] = std::make_unique<LiteralNode>(token);
                    }
                } else if (customStyleTemplates.count(templateName)) {
                    const auto& tmpl = customStyleTemplates.at(templateName);
                    for(const auto& prop : tmpl->properties) {
                        Token token = {TokenType::STRING, prop.second, currentToken.line, currentToken.column, currentToken.position};
                        node->inlineProperties[prop.first] = std::make_unique<LiteralNode>(token);
                    }
                } else {
                     std::cerr << "Parse Error: Style template '" << templateName << "' not found." << std::endl;
                }
            } else if (match(TokenType::LEFT_BRACE)) {
                if (!customStyleTemplates.count(templateName)) {
                    std::cerr << "Parse Error: Custom style template '" << templateName << "' not found." << std::endl;
                    int braceCount = 1;
                    while (braceCount > 0 && !check(TokenType::END_OF_FILE)) { advance(); if(check(TokenType::LEFT_BRACE)) braceCount++; if(check(TokenType::RIGHT_BRACE)) braceCount--; }
                    continue;
                }

                const auto& tmpl = customStyleTemplates.at(templateName);
                auto specializedProps = tmpl->properties;

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
                        auto expr = parseValue();
                        if (auto* lit = dynamic_cast<LiteralNode*>(expr.get())) {
                             specializedProps[key] = lit->value.lexeme;
                        } else {
                            std::cerr << "Parse Warning: Expressions in custom style specialization are not fully supported yet." << std::endl;
                        }
                        consume(TokenType::SEMICOLON, "Expect ';' after property value.");
                    } else {
                        std::cerr << "Parse Error: Unexpected token in custom style specialization." << std::endl;
                        advance();
                    }
                }
                consume(TokenType::RIGHT_BRACE, "Expect '}' after specialization block.");

                for (const auto& prop : specializedProps) {
                    Token token = {TokenType::STRING, prop.second, currentToken.line, currentToken.column, currentToken.position};
                    node->inlineProperties[prop.first] = std::make_unique<LiteralNode>(token);
                }
            } else {
                std::cerr << "Parse Error: Expect '{' or ';' after @Style usage." << std::endl;
            }
        }
        else {
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
        auto expr = parseValue();
        if (auto* lit = dynamic_cast<LiteralNode*>(expr.get())) {
            element.attributes[key] = lit->value.lexeme;
        } else {
            std::cerr << "Parse Warning: Expressions in attributes are not supported." << std::endl;
        }
        consume(TokenType::SEMICOLON, "Expect ';' after attribute value.");
    }
}

std::unique_ptr<BaseNode> Parser::templateDeclaration() {
    consume(TokenType::KEYWORD_TEMPLATE, "Expect '[Template]' keyword.");
    consume(TokenType::AT, "Expect '@' before template type.");

    if (currentToken.type != TokenType::IDENTIFIER) {
        std::cerr << "Parse Error: Expect template type (Style, Element, Var) after '@'." << std::endl;
        return nullptr;
    }

    std::string templateType = currentToken.lexeme;
    advance();

    if (templateType == "Style") {
        auto node = std::make_unique<StyleTemplateNode>();
        node->name = currentToken.lexeme;
        consume(TokenType::IDENTIFIER, "Expect template name.");
        consume(TokenType::LEFT_BRACE, "Expect '{' after template name.");
        while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
            if (check(TokenType::IDENTIFIER)) {
                std::string key = currentToken.lexeme;
                advance();
                consume(TokenType::COLON, "Expect ':' after property name.");
                auto expr = parseValue();
                if (auto* lit = dynamic_cast<LiteralNode*>(expr.get())) {
                    node->properties[key] = lit->value.lexeme;
                } else {
                    std::cerr << "Parse Warning: Expressions in style template definitions are not fully supported yet." << std::endl;
                }
                consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
            } else if (match(TokenType::AT) || match(TokenType::KEYWORD_INHERIT)) {
                if(previousToken.type != TokenType::KEYWORD_INHERIT) consume(TokenType::IDENTIFIER, "Expect 'Style' for inheritance.");
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
    } else if (templateType == "Element") {
        auto node = std::make_unique<ElementTemplateNode>();
        node->name = currentToken.lexeme;
        consume(TokenType::IDENTIFIER, "Expect template name.");
        consume(TokenType::LEFT_BRACE, "Expect '{' after name.");
        while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
            if (match(TokenType::AT) || match(TokenType::KEYWORD_INHERIT)) {
                if(previousToken.type != TokenType::KEYWORD_INHERIT) consume(TokenType::IDENTIFIER, "Expect 'Element' for inheritance.");
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
    } else if (templateType == "Var") {
        auto node = std::make_unique<VarTemplateNode>();
        node->name = currentToken.lexeme;
        consume(TokenType::IDENTIFIER, "Expect template name.");
        consume(TokenType::LEFT_BRACE, "Expect '{' after name.");
        while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
            if (check(TokenType::IDENTIFIER)) {
                std::string key = currentToken.lexeme;
                advance();
                consume(TokenType::COLON, "Expect ':' after variable name.");
                auto expr = parseValue();
                if (auto* lit = dynamic_cast<LiteralNode*>(expr.get())) {
                    node->variables[key] = lit->value.lexeme;
                } else {
                    std::cerr << "Parse Warning: Expressions in var template definitions are not fully supported yet." << std::endl;
                }
                consume(TokenType::SEMICOLON, "Expect ';' after variable value.");
            } else {
                std::cerr << "Parse Error: Unexpected token in var template." << std::endl;
                advance();
            }
        }
        consume(TokenType::RIGHT_BRACE, "Expect '}' after template body.");
        varTemplates[node->name] = std::move(node);
    } else {
        std::cerr << "Parse Error: Unknown template type '" << templateType << "'." << std::endl;
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

    for (const auto& prop : specStyleNode->inlineProperties) {
        if (prop.second) {
            targetStyleNode->inlineProperties[prop.first] = std::unique_ptr<ExpressionNode>(static_cast<ExpressionNode*>(prop.second->clone().release()));
        }
    }
    for (const auto& rule : specStyleNode->rules) {
        targetStyleNode->rules.push_back(std::unique_ptr<StyleRuleNode>(static_cast<StyleRuleNode*>(rule->clone().release())));
    }
}

std::unique_ptr<ExpressionNode> Parser::parseValue() {
    return parseExpression();
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
    consume(TokenType::AT, "Expect '@' before custom type.");

    if (currentToken.type != TokenType::IDENTIFIER) {
        std::cerr << "Parse Error: Expect custom type (Style, Element) after '@'." << std::endl;
        return nullptr;
    }

    std::string customType = currentToken.lexeme;
    advance();

    if (customType == "Element") {
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
    } else if (customType == "Style") {
        auto node = std::make_unique<CustomStyleTemplateNode>();
        node->name = currentToken.lexeme;
        consume(TokenType::IDENTIFIER, "Expect custom style name.");
        consume(TokenType::LEFT_BRACE, "Expect '{' after name.");

        while (!check(TokenType::RIGHT_BRACE) && !check(TokenType::END_OF_FILE)) {
            if (check(TokenType::IDENTIFIER)) {
                std::string key = currentToken.lexeme;
                advance();
                if (match(TokenType::COLON)) {
                    auto expr = parseValue();
                    if (auto* lit = dynamic_cast<LiteralNode*>(expr.get())) {
                        node->properties[key] = lit->value.lexeme;
                    } else {
                        std::cerr << "Parse Warning: Expressions are not supported in custom style definitions." << std::endl;
                    }
                    consume(TokenType::SEMICOLON, "Expect ';' after style property value.");
                } else if (match(TokenType::SEMICOLON)) {
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
        std::cerr << "Parse Error: Expected 'Element' or 'Style' after '[Custom] @' at line " << currentToken.line << std::endl;
    }

    return nullptr;
}
