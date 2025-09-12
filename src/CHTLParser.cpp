#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {

CHTLParser::CHTLParser(std::shared_ptr<CHTLContext> context)
    : m_context(context)
    , m_currentIndex(0)
    , m_debugMode(false)
{
}

CHTLParser::~CHTLParser() {
}

std::shared_ptr<BaseNode> CHTLParser::parse(const TokenList& tokens) {
    m_tokens = tokens;
    m_currentIndex = 0;
    
    if (m_debugMode) {
        std::cout << "[CHTLParser] Starting parse with " << tokens.size() << " tokens" << std::endl;
    }
    
    try {
        return parseDocument();
    } catch (const std::exception& e) {
        addError("Parse error: " + std::string(e.what()));
        return nullptr;
    }
}

void CHTLParser::reset() {
    m_currentIndex = 0;
}

void CHTLParser::setDebugMode(bool debug) {
    m_debugMode = debug;
}

bool CHTLParser::isDebugMode() const {
    return m_debugMode;
}

std::shared_ptr<BaseNode> CHTLParser::parseDocument() {
    auto document = std::make_shared<BaseNode>(NodeType::ELEMENT);
    document->setName("document");
    
    skipWhitespace();
    
    while (!isAtEnd()) {
        auto node = parseElement();
        if (node) {
            document->addChild(node);
        } else {
            // 尝试解析其他类型的节点
            if (checkToken(TokenType::TEXT)) {
                node = parseText();
                if (node) {
                    document->addChild(node);
                }
            } else if (checkToken(TokenType::COMMENT)) {
                node = parseComment();
                if (node) {
                    document->addChild(node);
                }
            } else if (checkToken(TokenType::TEMPLATE)) {
                node = parseTemplate();
                if (node) {
                    document->addChild(node);
                }
            } else if (checkToken(TokenType::CUSTOM)) {
                node = parseCustom();
                if (node) {
                    document->addChild(node);
                }
            } else if (checkToken(TokenType::ORIGIN)) {
                node = parseOrigin();
                if (node) {
                    document->addChild(node);
                }
            } else if (checkToken(TokenType::IMPORT)) {
                node = parseImport();
                if (node) {
                    document->addChild(node);
                }
            } else if (checkToken(TokenType::CONFIGURATION)) {
                node = parseConfiguration();
                if (node) {
                    document->addChild(node);
                }
            } else if (checkToken(TokenType::NAMESPACE)) {
                node = parseNamespace();
                if (node) {
                    document->addChild(node);
                }
            } else {
                addError("Unexpected token: " + currentToken().getValue());
                nextToken();
            }
        }
        
        skipWhitespace();
    }
    
    return document;
}

std::shared_ptr<BaseNode> CHTLParser::parseElement() {
    if (isAtEnd()) return nullptr;
    
    // 检查是否为HTML元素
    if (currentToken().getType() == TokenType::IDENTIFIER) {
        auto element = std::make_shared<ElementNode>(currentToken().getValue());
        element->setPosition(currentToken().getLine(), currentToken().getColumn());
        
        if (m_debugMode) {
            std::cout << "[CHTLParser] Parsing element: " << currentToken().getValue() << std::endl;
        }
        
        nextToken(); // 消费元素名
        
        // 解析属性
        if (checkToken(TokenType::LEFT_BRACE)) {
            nextToken(); // 消费 {
            
            // 解析属性列表
            parseAttributes(element);
            
            // 解析子节点
            skipWhitespace();
            while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                auto child = parseElement();
                if (child) {
                    element->addChild(child);
                } else {
                    // 尝试解析文本节点
                    auto textNode = parseText();
                    if (textNode) {
                        element->addChild(textNode);
                    } else {
                        break;
                    }
                }
                skipWhitespace();
            }
            
            if (checkToken(TokenType::RIGHT_BRACE)) {
                nextToken(); // 消费 }
            } else {
                addError("Expected '}' after element");
            }
        }
        
        return element;
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseText() {
    if (isAtEnd()) return nullptr;
    
    // 检查是否为text关键字
    if (checkToken(TokenType::TEXT)) {
        auto textNode = std::make_shared<TextNode>();
        textNode->setPosition(currentToken().getLine(), currentToken().getColumn());
        
        nextToken(); // 消费text关键字
        
        if (checkToken(TokenType::LEFT_BRACE)) {
            nextToken(); // 消费 {
            
            skipWhitespace();
            
            // 收集文本内容
            std::string textContent;
            while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                if (currentToken().getType() == TokenType::STRING) {
                    textContent += currentToken().getValue();
                } else if (currentToken().getType() == TokenType::IDENTIFIER) {
                    textContent += currentToken().getValue();
                } else if (currentToken().getType() == TokenType::NUMBER) {
                    textContent += currentToken().getValue();
                } else if (!std::isspace(currentToken().getValue()[0])) {
                    textContent += currentToken().getValue();
                }
                nextToken();
            }
            
            if (checkToken(TokenType::RIGHT_BRACE)) {
                nextToken(); // 消费 }
            } else {
                addError("Expected '}' after text content");
            }
            
            textNode->setText(textContent);
            return textNode;
        } else if (checkToken(TokenType::COLON)) {
            // text: "value" 形式
            nextToken(); // 消费 :
            skipWhitespace();
            
            std::string value;
            if (currentToken().getType() == TokenType::STRING) {
                value = currentToken().getValue();
                nextToken();
            } else if (currentToken().getType() == TokenType::IDENTIFIER) {
                value = currentToken().getValue();
                nextToken();
            } else {
                addError("Expected string or identifier after text:");
            }
            
            textNode->setText(value);
            return textNode;
        }
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseComment() {
    if (isAtEnd()) return nullptr;
    
    if (currentToken().getType() == TokenType::COMMENT) {
        auto commentNode = std::make_shared<TextNode>();
        commentNode->setPosition(currentToken().getLine(), currentToken().getColumn());
        commentNode->setText(currentToken().getValue());
        commentNode->setGeneratorComment(true);
        
        nextToken();
        return commentNode;
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseTemplate() {
    // TODO: 实现模板解析
    addWarning("Template parsing not yet implemented");
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseCustom() {
    // TODO: 实现自定义解析
    addWarning("Custom parsing not yet implemented");
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseOrigin() {
    // TODO: 实现原始嵌入解析
    addWarning("Origin parsing not yet implemented");
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseImport() {
    // TODO: 实现导入解析
    addWarning("Import parsing not yet implemented");
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseConfiguration() {
    // TODO: 实现配置解析
    addWarning("Configuration parsing not yet implemented");
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseNamespace() {
    // TODO: 实现命名空间解析
    addWarning("Namespace parsing not yet implemented");
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseStyle() {
    if (isAtEnd()) return nullptr;
    
    if (checkToken(TokenType::STYLE)) {
        auto styleNode = std::make_shared<BaseNode>(NodeType::STYLE);
        styleNode->setPosition(currentToken().getLine(), currentToken().getColumn());
        
        nextToken(); // 消费style关键字
        
        if (checkToken(TokenType::LEFT_BRACE)) {
            nextToken(); // 消费 {
            
            skipWhitespace();
            
            // 解析样式属性
            parseStyleProperties(styleNode);
            
            if (checkToken(TokenType::RIGHT_BRACE)) {
                nextToken(); // 消费 }
            } else {
                addError("Expected '}' after style block");
            }
        }
        
        return styleNode;
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseScript() {
    if (isAtEnd()) return nullptr;
    
    if (checkToken(TokenType::SCRIPT)) {
        auto scriptNode = std::make_shared<BaseNode>(NodeType::SCRIPT);
        scriptNode->setPosition(currentToken().getLine(), currentToken().getColumn());
        
        nextToken(); // 消费script关键字
        
        if (checkToken(TokenType::LEFT_BRACE)) {
            nextToken(); // 消费 {
            
            skipWhitespace();
            
            // 收集脚本内容
            std::string scriptContent;
            while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                scriptContent += currentToken().getValue();
                nextToken();
            }
            
            if (checkToken(TokenType::RIGHT_BRACE)) {
                nextToken(); // 消费 }
            } else {
                addError("Expected '}' after script block");
            }
            
            scriptNode->setValue(scriptContent);
        }
        
        return scriptNode;
    }
    
    return nullptr;
}

void CHTLParser::parseAttributes(std::shared_ptr<BaseNode> element) {
    skipWhitespace();
    
    while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto attr = parseAttribute();
        if (!attr.first.empty()) {
            element->setAttribute(attr.first, attr.second);
        } else {
            break;
        }
        skipWhitespace();
    }
}

std::pair<std::string, std::string> CHTLParser::parseAttribute() {
    if (isAtEnd()) return {"", ""};
    
    std::string key;
    std::string value;
    
    // 解析属性名
    if (currentToken().getType() == TokenType::IDENTIFIER) {
        key = currentToken().getValue();
        nextToken();
        
        // 检查是否有值
        if (checkToken(TokenType::COLON) || checkToken(TokenType::EQUALS)) {
            nextToken(); // 消费 : 或 =
            skipWhitespace();
            
            // 解析属性值
            if (currentToken().getType() == TokenType::STRING) {
                value = currentToken().getValue();
                nextToken();
            } else if (currentToken().getType() == TokenType::IDENTIFIER) {
                value = currentToken().getValue();
                nextToken();
            } else if (currentToken().getType() == TokenType::NUMBER) {
                value = currentToken().getValue();
                nextToken();
            } else {
                addError("Expected value after attribute name");
            }
        }
    }
    
    return {key, value};
}

void CHTLParser::parseStyleProperties(std::shared_ptr<BaseNode> styleNode) {
    skipWhitespace();
    
    while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
        auto prop = parseStyleProperty();
        if (!prop.first.empty()) {
            styleNode->setAttribute(prop.first, prop.second);
        } else {
            break;
        }
        skipWhitespace();
    }
}

std::pair<std::string, std::string> CHTLParser::parseStyleProperty() {
    if (isAtEnd()) return {"", ""};
    
    std::string key;
    std::string value;
    
    // 解析属性名
    if (currentToken().getType() == TokenType::IDENTIFIER) {
        key = currentToken().getValue();
        nextToken();
        
        // 检查是否有值
        if (checkToken(TokenType::COLON) || checkToken(TokenType::EQUALS)) {
            nextToken(); // 消费 : 或 =
            skipWhitespace();
            
            // 解析属性值
            if (currentToken().getType() == TokenType::STRING) {
                value = currentToken().getValue();
                nextToken();
            } else if (currentToken().getType() == TokenType::IDENTIFIER) {
                value = currentToken().getValue();
                nextToken();
            } else if (currentToken().getType() == TokenType::NUMBER) {
                value = currentToken().getValue();
                nextToken();
            } else {
                addError("Expected value after style property name");
            }
        }
    }
    
    return {key, value};
}

std::string CHTLParser::parseSelector() {
    // TODO: 实现选择器解析
    return "";
}

std::string CHTLParser::parseExpression() {
    // TODO: 实现表达式解析
    return "";
}

std::string CHTLParser::parseString() {
    if (currentToken().getType() == TokenType::STRING) {
        std::string value = currentToken().getValue();
        nextToken();
        return value;
    }
    return "";
}

std::string CHTLParser::parseLiteral() {
    if (currentToken().getType() == TokenType::IDENTIFIER) {
        std::string value = currentToken().getValue();
        nextToken();
        return value;
    }
    return "";
}

void CHTLParser::skipWhitespace() {
    while (!isAtEnd() && 
           (currentToken().getType() == TokenType::IDENTIFIER && 
            std::all_of(currentToken().getValue().begin(), currentToken().getValue().end(), ::isspace))) {
        nextToken();
    }
}

bool CHTLParser::checkToken(TokenType type) const {
    return !isAtEnd() && currentToken().getType() == type;
}

bool CHTLParser::checkToken(const std::string& value) const {
    return !isAtEnd() && currentToken().getValue() == value;
}

bool CHTLParser::matchToken(TokenType type) {
    if (checkToken(type)) {
        nextToken();
        return true;
    }
    return false;
}

bool CHTLParser::matchToken(const std::string& value) {
    if (checkToken(value)) {
        nextToken();
        return true;
    }
    return false;
}

const Token& CHTLParser::currentToken() const {
    if (m_currentIndex < m_tokens.size()) {
        return m_tokens[m_currentIndex];
    }
    static Token endToken(TokenType::END_OF_FILE, "", 0, 0);
    return endToken;
}

const Token& CHTLParser::peekToken() const {
    if (m_currentIndex + 1 < m_tokens.size()) {
        return m_tokens[m_currentIndex + 1];
    }
    static Token endToken(TokenType::END_OF_FILE, "", 0, 0);
    return endToken;
}

bool CHTLParser::nextToken() {
    if (m_currentIndex < m_tokens.size()) {
        m_currentIndex++;
        return true;
    }
    return false;
}

bool CHTLParser::isAtEnd() const {
    return m_currentIndex >= m_tokens.size() || 
           currentToken().getType() == TokenType::END_OF_FILE;
}

void CHTLParser::addError(const std::string& message) {
    if (m_context) {
        std::ostringstream oss;
        oss << "Parser Error at " << currentToken().getLine() << ":" 
            << currentToken().getColumn() << ": " << message;
        m_context->addError(oss.str());
    }
}

void CHTLParser::addWarning(const std::string& message) {
    if (m_context) {
        std::ostringstream oss;
        oss << "Parser Warning at " << currentToken().getLine() << ":" 
            << currentToken().getColumn() << ": " << message;
        m_context->addWarning(oss.str());
    }
}

} // namespace CHTL