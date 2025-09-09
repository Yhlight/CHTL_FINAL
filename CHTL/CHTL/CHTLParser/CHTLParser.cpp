#include "CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <stdexcept>

namespace CHTL {

CHTLParser::CHTLParser(const std::string& source)
    : lexer_(std::make_unique<Lexer>(source)) {
}

std::shared_ptr<BaseNode> CHTLParser::parse() {
    auto root = std::make_shared<BaseNode>(NodeType::ROOT, "root");
    node_stack_.push(root);
    
    while (lexer_->hasMoreTokens()) {
        auto token = currentToken();
        
        if (token->isType(TokenType::TEMPLATE)) {
            auto templateNode = parseTemplate();
            if (templateNode) {
                root->addChild(templateNode);
            }
        } else if (token->isType(TokenType::CUSTOM)) {
            auto customNode = parseCustom();
            if (customNode) {
                root->addChild(customNode);
            }
        } else if (token->isType(TokenType::ORIGIN)) {
            auto originNode = parseOrigin();
            if (originNode) {
                root->addChild(originNode);
            }
        } else if (token->isType(TokenType::IMPORT)) {
            auto importNode = parseImport();
            if (importNode) {
                root->addChild(importNode);
            }
        } else if (token->isType(TokenType::NAMESPACE)) {
            auto namespaceNode = parseNamespace();
            if (namespaceNode) {
                root->addChild(namespaceNode);
            }
        } else if (token->isType(TokenType::CONFIGURATION)) {
            auto configNode = parseConfiguration();
            if (configNode) {
                root->addChild(configNode);
            }
        } else if (token->isType(TokenType::IDENTIFIER) || token->isType(TokenType::LITERAL)) {
            auto elementNode = parseElement();
            if (elementNode) {
                root->addChild(elementNode);
            }
        } else {
            nextToken(); // 跳过未知token
        }
    }
    
    return root;
}

std::shared_ptr<BaseNode> CHTLParser::parseElement() {
    auto token = currentToken();
    std::string tagName = token->getValue();
    nextToken();
    
    auto element = createElementNode(tagName);
    
    // 解析属性
    parseAttributes(element);
    
    // 检查是否有子内容
    if (match(TokenType::LEFT_BRACE)) {
        consume(TokenType::LEFT_BRACE);
        node_stack_.push(element);
        
        while (!match(TokenType::RIGHT_BRACE) && lexer_->hasMoreTokens()) {
            auto childToken = currentToken();
            
            if (childToken->isType(TokenType::TEXT)) {
                nextToken();
                auto textNode = createTextNode(childToken->getValue());
                element->addChild(textNode);
            } else if (childToken->isType(TokenType::IDENTIFIER) || childToken->isType(TokenType::LITERAL)) {
                auto childElement = parseElement();
                if (childElement) {
                    element->addChild(childElement);
                }
            } else if (childToken->isType(TokenType::STYLE)) {
                nextToken();
                parseStyleBlock(element);
            } else if (childToken->isType(TokenType::SCRIPT)) {
                nextToken();
                parseScriptBlock(element);
            } else {
                nextToken(); // 跳过未知token
            }
        }
        
        consume(TokenType::RIGHT_BRACE);
        node_stack_.pop();
    }
    
    return element;
}

std::shared_ptr<BaseNode> CHTLParser::parseTemplate() {
    consume(TokenType::TEMPLATE);
    
    auto typeToken = currentToken();
    NodeType templateType;
    if (typeToken->isType(TokenType::AT_STYLE)) {
        templateType = NodeType::TEMPLATE_STYLE;
    } else if (typeToken->isType(TokenType::AT_ELEMENT)) {
        templateType = NodeType::TEMPLATE_ELEMENT;
    } else if (typeToken->isType(TokenType::AT_VAR)) {
        templateType = NodeType::TEMPLATE_VAR;
    } else {
        error("Expected @Style, @Element, or @Var after [Template]");
        return nullptr;
    }
    
    nextToken();
    auto nameToken = currentToken();
    std::string name = nameToken->getValue();
    nextToken();
    
    auto templateNode = createTemplateNode(templateType, name);
    
    if (match(TokenType::LEFT_BRACE)) {
        consume(TokenType::LEFT_BRACE);
        node_stack_.push(templateNode);
        
        while (!match(TokenType::RIGHT_BRACE) && lexer_->hasMoreTokens()) {
            auto childElement = parseElement();
            if (childElement) {
                templateNode->addChild(childElement);
            }
        }
        
        consume(TokenType::RIGHT_BRACE);
        node_stack_.pop();
    }
    
    return templateNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseCustom() {
    consume(TokenType::CUSTOM);
    
    auto typeToken = currentToken();
    NodeType customType;
    if (typeToken->isType(TokenType::AT_STYLE)) {
        customType = NodeType::CUSTOM_STYLE;
    } else if (typeToken->isType(TokenType::AT_ELEMENT)) {
        customType = NodeType::CUSTOM_ELEMENT;
    } else if (typeToken->isType(TokenType::AT_VAR)) {
        customType = NodeType::CUSTOM_VAR;
    } else {
        error("Expected @Style, @Element, or @Var after [Custom]");
        return nullptr;
    }
    
    nextToken();
    auto nameToken = currentToken();
    std::string name = nameToken->getValue();
    nextToken();
    
    auto customNode = createCustomNode(customType, name);
    
    if (match(TokenType::LEFT_BRACE)) {
        consume(TokenType::LEFT_BRACE);
        node_stack_.push(customNode);
        
        while (!match(TokenType::RIGHT_BRACE) && lexer_->hasMoreTokens()) {
            auto childElement = parseElement();
            if (childElement) {
                customNode->addChild(childElement);
            }
        }
        
        consume(TokenType::RIGHT_BRACE);
        node_stack_.pop();
    }
    
    return customNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseOrigin() {
    consume(TokenType::ORIGIN);
    
    auto typeToken = currentToken();
    std::string type = typeToken->getValue();
    nextToken();
    
    auto originNode = std::make_shared<BaseNode>(NodeType::ORIGIN, type);
    
    if (match(TokenType::LEFT_BRACE)) {
        consume(TokenType::LEFT_BRACE);
        
        std::string content;
        while (!match(TokenType::RIGHT_BRACE) && lexer_->hasMoreTokens()) {
            auto token = currentToken();
            content += token->getValue();
            nextToken();
        }
        
        consume(TokenType::RIGHT_BRACE);
        originNode->setAttribute("content", content);
    }
    
    return originNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseImport() {
    consume(TokenType::IMPORT);
    
    auto typeToken = currentToken();
    std::string type = typeToken->getValue();
    nextToken();
    
    auto importNode = std::make_shared<BaseNode>(NodeType::IMPORT, type);
    
    if (match(TokenType::FROM)) {
        consume(TokenType::FROM);
        auto pathToken = currentToken();
        importNode->setAttribute("path", pathToken->getValue());
        nextToken();
    }
    
    return importNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseNamespace() {
    consume(TokenType::NAMESPACE);
    
    auto nameToken = currentToken();
    std::string name = nameToken->getValue();
    nextToken();
    
    auto namespaceNode = std::make_shared<BaseNode>(NodeType::NAMESPACE, name);
    
    if (match(TokenType::LEFT_BRACE)) {
        consume(TokenType::LEFT_BRACE);
        node_stack_.push(namespaceNode);
        
        while (!match(TokenType::RIGHT_BRACE) && lexer_->hasMoreTokens()) {
            auto childElement = parseElement();
            if (childElement) {
                namespaceNode->addChild(childElement);
            }
        }
        
        consume(TokenType::RIGHT_BRACE);
        node_stack_.pop();
    }
    
    return namespaceNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseConfiguration() {
    consume(TokenType::CONFIGURATION);
    
    auto configNode = std::make_shared<BaseNode>(NodeType::CONFIG, "configuration");
    
    if (match(TokenType::LEFT_BRACE)) {
        consume(TokenType::LEFT_BRACE);
        
        while (!match(TokenType::RIGHT_BRACE) && lexer_->hasMoreTokens()) {
            auto keyToken = currentToken();
            std::string key = keyToken->getValue();
            nextToken();
            
            if (match(TokenType::EQUALS)) {
                consume(TokenType::EQUALS);
                auto valueToken = currentToken();
                std::string value = valueToken->getValue();
                nextToken();
                
                configNode->setAttribute(key, value);
            }
        }
        
        consume(TokenType::RIGHT_BRACE);
    }
    
    return configNode;
}

std::unique_ptr<Token> CHTLParser::currentToken() {
    return lexer_->nextToken();
}

std::unique_ptr<Token> CHTLParser::nextToken() {
    return lexer_->nextToken();
}

std::unique_ptr<Token> CHTLParser::peekToken() {
    return lexer_->peekToken();
}

bool CHTLParser::match(TokenType type) {
    auto token = peekToken();
    return token && token->isType(type);
}

bool CHTLParser::match(const std::string& value) {
    auto token = peekToken();
    return token && token->getValue() == value;
}

void CHTLParser::consume(TokenType type) {
    auto token = nextToken();
    if (!token || !token->isType(type)) {
        error("Expected token type " + std::to_string(static_cast<int>(type)));
    }
}

void CHTLParser::consume(const std::string& value) {
    auto token = nextToken();
    if (!token || token->getValue() != value) {
        error("Expected token value: " + value);
    }
}

void CHTLParser::skipTo(TokenType type) {
    while (lexer_->hasMoreTokens()) {
        auto token = nextToken();
        if (token->isType(type)) {
            break;
        }
    }
}

std::string CHTLParser::parseIdentifier() {
    auto token = currentToken();
    if (token->isType(TokenType::IDENTIFIER) || token->isType(TokenType::LITERAL)) {
        nextToken();
        return token->getValue();
    }
    error("Expected identifier");
    return "";
}

std::string CHTLParser::parseString() {
    auto token = currentToken();
    if (token->isType(TokenType::STRING)) {
        nextToken();
        return token->getValue();
    }
    error("Expected string");
    return "";
}

std::string CHTLParser::parseLiteral() {
    auto token = currentToken();
    if (token->isType(TokenType::LITERAL)) {
        nextToken();
        return token->getValue();
    }
    error("Expected literal");
    return "";
}

void CHTLParser::parseAttributes(std::shared_ptr<BaseNode> node) {
    while (lexer_->hasMoreTokens()) {
        auto token = peekToken();
        
        if (token->isType(TokenType::LEFT_BRACE) || token->isType(TokenType::SEMICOLON)) {
            break;
        }
        
        if (token->isType(TokenType::IDENTIFIER) || token->isType(TokenType::LITERAL)) {
            std::string key = token->getValue();
            nextToken();
            
            if (match(TokenType::COLON) || match(TokenType::EQUALS)) {
                nextToken();
                std::string value;
                
                if (match(TokenType::STRING)) {
                    value = parseString();
                } else if (match(TokenType::LITERAL)) {
                    value = parseLiteral();
                } else {
                    value = ""; // 无值属性
                }
                
                node->setAttribute(key, value);
            } else {
                // 无值属性
                node->setAttribute(key, "");
            }
        } else {
            nextToken(); // 跳过未知token
        }
    }
}

void CHTLParser::parseStyleBlock(std::shared_ptr<BaseNode> node) {
    if (match(TokenType::LEFT_BRACE)) {
        consume(TokenType::LEFT_BRACE);
        
        std::string cssContent;
        while (!match(TokenType::RIGHT_BRACE) && lexer_->hasMoreTokens()) {
            auto token = currentToken();
            cssContent += token->getValue();
            nextToken();
        }
        
        consume(TokenType::RIGHT_BRACE);
        
        auto styleNode = std::make_shared<BaseNode>(NodeType::STYLE, "style");
        styleNode->setAttribute("content", cssContent);
        node->addChild(styleNode);
    }
}

void CHTLParser::parseScriptBlock(std::shared_ptr<BaseNode> node) {
    if (match(TokenType::LEFT_BRACE)) {
        consume(TokenType::LEFT_BRACE);
        
        std::string jsContent;
        while (!match(TokenType::RIGHT_BRACE) && lexer_->hasMoreTokens()) {
            auto token = currentToken();
            jsContent += token->getValue();
            nextToken();
        }
        
        consume(TokenType::RIGHT_BRACE);
        
        auto scriptNode = std::make_shared<BaseNode>(NodeType::SCRIPT, "script");
        scriptNode->setAttribute("content", jsContent);
        node->addChild(scriptNode);
    }
}

void CHTLParser::error(const std::string& message) {
    throw std::runtime_error("Parse error: " + message);
}

void CHTLParser::expect(TokenType type) {
    if (!match(type)) {
        error("Expected token type " + std::to_string(static_cast<int>(type)));
    }
}

void CHTLParser::expect(const std::string& value) {
    if (!match(value)) {
        error("Expected token value: " + value);
    }
}

std::shared_ptr<BaseNode> CHTLParser::createElementNode(const std::string& tagName) {
    return std::make_shared<ElementNode>(tagName);
}

std::shared_ptr<BaseNode> CHTLParser::createTextNode(const std::string& content) {
    return std::make_shared<TextNode>(content);
}

std::shared_ptr<BaseNode> CHTLParser::createTemplateNode(NodeType type, const std::string& name) {
    return std::make_shared<BaseNode>(type, name);
}

std::shared_ptr<BaseNode> CHTLParser::createCustomNode(NodeType type, const std::string& name) {
    return std::make_shared<BaseNode>(type, name);
}

} // namespace CHTL