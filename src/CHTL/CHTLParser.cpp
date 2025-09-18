#include "CHTLParser.h"
#include <sstream>
#include <iostream>

namespace CHTL {

CHTLParser::CHTLParser(const TokenList& tokens)
    : tokens_(tokens), currentPos_(0) {
}

NodePtr CHTLParser::parse() {
    return parseDocument();
}

NodePtr CHTLParser::parseDocument() {
    auto document = std::make_shared<ElementNode>("html");
    
    while (hasMoreTokens()) {
        auto node = parseElement();
        if (node) {
            document->addChild(node);
        }
    }
    
    return document;
}

NodePtr CHTLParser::parseElement() {
    if (!hasMoreTokens()) {
        return nullptr;
    }
    
    auto token = currentToken();
    
    // 解析不同类型的节点
    if (isTemplateStart()) {
        return parseTemplate();
    } else if (isCustomStart()) {
        return parseCustom();
    } else if (isStyleStart()) {
        return parseStyle();
    } else if (isScriptStart()) {
        return parseScript();
    } else if (isOriginStart()) {
        return parseOrigin();
    } else if (isImportStart()) {
        return parseImport();
    } else if (isConfigurationStart()) {
        return parseConfiguration();
    } else if (isNamespaceStart()) {
        return parseNamespace();
    } else if (isOperatorStart()) {
        return parseOperator();
    } else if (token->isType(TokenType::IDENTIFIER) || token->isType(TokenType::LITERAL)) {
        // 普通HTML元素
        return parseElement();
    } else if (token->isType(TokenType::STRING) || token->isType(TokenType::LITERAL)) {
        // 文本节点
        return parseText();
    }
    
    reportUnexpectedToken("element, template, custom, style, script, origin, import, configuration, namespace, or operator");
    return nullptr;
}

NodePtr CHTLParser::parseText() {
    auto token = currentToken();
    std::string text = token->getValue();
    advance();
    
    auto textNode = std::make_shared<TextNode>(text);
    textNode->setPosition(token->getLine(), token->getColumn());
    
    return textNode;
}

NodePtr CHTLParser::parseTemplate() {
    consume(TokenType::TEMPLATE);
    
    auto token = currentToken();
    if (!token->isType(TokenType::AT)) {
        reportError("Expected @ after [Template]");
        return nullptr;
    }
    advance();
    
    auto typeToken = currentToken();
    TokenType templateType = typeToken->getType();
    advance();
    
    auto nameToken = currentToken();
    std::string name = nameToken->getValue();
    advance();
    
    auto templateNode = parseTemplateBlock(templateType);
    if (templateNode) {
        templateNode->setName(name);
    }
    
    return templateNode;
}

NodePtr CHTLParser::parseCustom() {
    consume(TokenType::CUSTOM);
    
    auto token = currentToken();
    if (!token->isType(TokenType::AT)) {
        reportError("Expected @ after [Custom]");
        return nullptr;
    }
    advance();
    
    auto typeToken = currentToken();
    TokenType customType = typeToken->getType();
    advance();
    
    auto nameToken = currentToken();
    std::string name = nameToken->getValue();
    advance();
    
    auto customNode = parseCustomBlock(customType);
    if (customNode) {
        customNode->setName(name);
    }
    
    return customNode;
}

NodePtr CHTLParser::parseStyle() {
    consume(TokenType::STYLE);
    
    auto styleNode = std::make_shared<ElementNode>("style");
    parseStyleBlock(styleNode);
    
    return styleNode;
}

NodePtr CHTLParser::parseScript() {
    consume(TokenType::SCRIPT);
    
    auto scriptNode = std::make_shared<ElementNode>("script");
    parseScriptBlock(scriptNode);
    
    return scriptNode;
}

NodePtr CHTLParser::parseOrigin() {
    consume(TokenType::ORIGIN);
    
    auto token = currentToken();
    if (!token->isType(TokenType::AT)) {
        reportError("Expected @ after [Origin]");
        return nullptr;
    }
    advance();
    
    auto typeToken = currentToken();
    std::string type = typeToken->getValue();
    advance();
    
    auto originNode = std::make_shared<ElementNode>("origin");
    originNode->setAttribute("type", type);
    
    // 解析原始内容
    if (match(TokenType::LEFT_BRACE)) {
        consume(TokenType::LEFT_BRACE);
        
        std::string content;
        while (hasMoreTokens() && !match(TokenType::RIGHT_BRACE)) {
            content += currentToken()->getValue();
            advance();
        }
        
        consume(TokenType::RIGHT_BRACE);
        originNode->setContent(content);
    }
    
    return originNode;
}

NodePtr CHTLParser::parseImport() {
    consume(TokenType::IMPORT);
    
    auto token = currentToken();
    if (!token->isType(TokenType::AT)) {
        reportError("Expected @ after [Import]");
        return nullptr;
    }
    advance();
    
    auto typeToken = currentToken();
    std::string type = typeToken->getValue();
    advance();
    
    auto importNode = std::make_shared<ElementNode>("import");
    importNode->setAttribute("type", type);
    
    // 解析导入路径
    std::string path = parseImportPath();
    importNode->setAttribute("path", path);
    
    return importNode;
}

NodePtr CHTLParser::parseConfiguration() {
    consume(TokenType::CONFIGURATION);
    
    auto configNode = std::make_shared<ElementNode>("configuration");
    parseConfigurationBlock();
    
    return configNode;
}

NodePtr CHTLParser::parseNamespace() {
    consume(TokenType::NAMESPACE);
    
    auto nameToken = currentToken();
    std::string name = nameToken->getValue();
    advance();
    
    auto namespaceNode = std::make_shared<ElementNode>("namespace");
    namespaceNode->setName(name);
    
    if (match(TokenType::LEFT_BRACE)) {
        parseNamespaceBlock();
    }
    
    return namespaceNode;
}

NodePtr CHTLParser::parseOperator() {
    auto token = currentToken();
    TokenType opType = token->getType();
    advance();
    
    auto operatorNode = std::make_shared<ElementNode>("operator");
    operatorNode->setAttribute("type", GlobalMap::getTokenTypeName(opType));
    
    return operatorNode;
}

void CHTLParser::parseAttributes(ElementNode* element) {
    while (hasMoreTokens() && !match(TokenType::LEFT_BRACE) && !match(TokenType::RIGHT_BRACE)) {
        auto token = currentToken();
        
        if (token->isType(TokenType::IDENTIFIER) || token->isType(TokenType::LITERAL)) {
            std::string name = token->getValue();
            advance();
            
            if (match(TokenType::COLON) || match(TokenType::EQUALS)) {
                advance(); // 跳过 : 或 =
                
                auto valueToken = currentToken();
                std::string value = valueToken->getValue();
                advance();
                
                element->setAttribute(name, value);
            }
        } else {
            break;
        }
    }
}

void CHTLParser::parseInlineStyles(ElementNode* element) {
    // 解析内联样式
    while (hasMoreTokens() && !match(TokenType::RIGHT_BRACE)) {
        auto token = currentToken();
        
        if (token->isType(TokenType::IDENTIFIER) || token->isType(TokenType::LITERAL)) {
            std::string property = token->getValue();
            advance();
            
            if (match(TokenType::COLON)) {
                advance(); // 跳过 :
                
                auto valueToken = currentToken();
                std::string value = valueToken->getValue();
                advance();
                
                element->addInlineStyle(property, value);
            }
        } else {
            break;
        }
    }
}

void CHTLParser::parseBlock(NodePtr node) {
    if (match(TokenType::LEFT_BRACE)) {
        consume(TokenType::LEFT_BRACE);
        
        while (hasMoreTokens() && !match(TokenType::RIGHT_BRACE)) {
            auto child = parseElement();
            if (child) {
                node->addChild(child);
            }
        }
        
        consume(TokenType::RIGHT_BRACE);
    }
}

void CHTLParser::parseStyleBlock(NodePtr node) {
    if (match(TokenType::LEFT_BRACE)) {
        consume(TokenType::LEFT_BRACE);
        
        std::string css;
        while (hasMoreTokens() && !match(TokenType::RIGHT_BRACE)) {
            css += currentToken()->getValue();
            advance();
        }
        
        consume(TokenType::RIGHT_BRACE);
        node->setContent(css);
    }
}

void CHTLParser::parseScriptBlock(NodePtr node) {
    if (match(TokenType::LEFT_BRACE)) {
        consume(TokenType::LEFT_BRACE);
        
        std::string js;
        while (hasMoreTokens() && !match(TokenType::RIGHT_BRACE)) {
            js += currentToken()->getValue();
            advance();
        }
        
        consume(TokenType::RIGHT_BRACE);
        node->setContent(js);
    }
}

std::string CHTLParser::parseExpression() {
    std::string expression;
    
    while (hasMoreTokens() && !match(TokenType::SEMICOLON) && !match(TokenType::RIGHT_BRACE)) {
        expression += currentToken()->getValue();
        advance();
    }
    
    return expression;
}

std::string CHTLParser::parseConditionalExpression() {
    // 解析条件表达式
    return parseExpression();
}

std::string CHTLParser::parseArithmeticExpression() {
    // 解析算术表达式
    return parseExpression();
}

NodePtr CHTLParser::parseTemplateBlock(TokenType templateType) {
    auto templateNode = std::make_shared<ElementNode>("template");
    templateNode->setAttribute("type", GlobalMap::getTokenTypeName(templateType));
    
    if (match(TokenType::LEFT_BRACE)) {
        parseBlock(templateNode);
    }
    
    return templateNode;
}

void CHTLParser::parseTemplateContent(NodePtr templateNode) {
    parseBlock(templateNode);
}

NodePtr CHTLParser::parseCustomBlock(TokenType customType) {
    auto customNode = std::make_shared<ElementNode>("custom");
    customNode->setAttribute("type", GlobalMap::getTokenTypeName(customType));
    
    if (match(TokenType::LEFT_BRACE)) {
        parseBlock(customNode);
    }
    
    return customNode;
}

void CHTLParser::parseCustomContent(NodePtr customNode) {
    parseBlock(customNode);
}

NodePtr CHTLParser::parseImportStatement() {
    return parseImport();
}

std::string CHTLParser::parseImportPath() {
    auto token = currentToken();
    std::string path = token->getValue();
    advance();
    
    // 处理 from 关键字
    if (match(TokenType::FROM)) {
        advance();
        auto pathToken = currentToken();
        path = pathToken->getValue();
        advance();
    }
    
    return path;
}

NodePtr CHTLParser::parseConfigurationBlock() {
    auto configNode = std::make_shared<ElementNode>("configuration");
    
    if (match(TokenType::LEFT_BRACE)) {
        parseBlock(configNode);
    }
    
    return configNode;
}

void CHTLParser::parseConfigurationContent(NodePtr configNode) {
    parseBlock(configNode);
}

NodePtr CHTLParser::parseNamespaceBlock() {
    auto namespaceNode = std::make_shared<ElementNode>("namespace");
    
    if (match(TokenType::LEFT_BRACE)) {
        parseBlock(namespaceNode);
    }
    
    return namespaceNode;
}

void CHTLParser::parseNamespaceContent(NodePtr namespaceNode) {
    parseBlock(namespaceNode);
}

NodePtr CHTLParser::parseOperatorBlock() {
    auto operatorNode = std::make_shared<ElementNode>("operator");
    
    if (match(TokenType::LEFT_BRACE)) {
        parseBlock(operatorNode);
    }
    
    return operatorNode;
}

void CHTLParser::parseOperatorContent(NodePtr operatorNode) {
    parseBlock(operatorNode);
}

bool CHTLParser::hasMoreTokens() const {
    return currentPos_ < tokens_.size();
}

TokenPtr CHTLParser::currentToken() const {
    if (currentPos_ < tokens_.size()) {
        return tokens_[currentPos_];
    }
    return nullptr;
}

TokenPtr CHTLParser::peekToken(size_t offset) const {
    size_t pos = currentPos_ + offset;
    if (pos < tokens_.size()) {
        return tokens_[pos];
    }
    return nullptr;
}

void CHTLParser::advance() {
    if (currentPos_ < tokens_.size()) {
        currentPos_++;
    }
}

bool CHTLParser::match(TokenType type) const {
    auto token = currentToken();
    return token && token->isType(type);
}

bool CHTLParser::match(const std::string& value) const {
    auto token = currentToken();
    return token && token->getValue() == value;
}

void CHTLParser::consume(TokenType type, const std::string& expected) {
    if (!match(type)) {
        reportUnexpectedToken(expected.empty() ? GlobalMap::getTokenTypeName(type) : expected);
        return;
    }
    advance();
}

void CHTLParser::consume(const std::string& value, const std::string& expected) {
    if (!match(value)) {
        reportUnexpectedToken(expected.empty() ? value : expected);
        return;
    }
    advance();
}

bool CHTLParser::isElementStart() const {
    auto token = currentToken();
    return token && (token->isType(TokenType::IDENTIFIER) || token->isType(TokenType::LITERAL));
}

bool CHTLParser::isTemplateStart() const {
    return match(TokenType::TEMPLATE);
}

bool CHTLParser::isCustomStart() const {
    return match(TokenType::CUSTOM);
}

bool CHTLParser::isStyleStart() const {
    return match(TokenType::STYLE);
}

bool CHTLParser::isScriptStart() const {
    return match(TokenType::SCRIPT);
}

bool CHTLParser::isOriginStart() const {
    return match(TokenType::ORIGIN);
}

bool CHTLParser::isImportStart() const {
    return match(TokenType::IMPORT);
}

bool CHTLParser::isConfigurationStart() const {
    return match(TokenType::CONFIGURATION);
}

bool CHTLParser::isNamespaceStart() const {
    return match(TokenType::NAMESPACE);
}

bool CHTLParser::isOperatorStart() const {
    auto token = currentToken();
    return token && (token->isType(TokenType::DELETE) || token->isType(TokenType::INSERT) ||
                     token->isType(TokenType::INHERIT));
}

void CHTLParser::setErrorHandler(std::function<void(const std::string&, size_t, size_t)> handler) {
    errorHandler_ = handler;
}

void CHTLParser::reportError(const std::string& message) {
    if (errorHandler_) {
        auto token = currentToken();
        if (token) {
            errorHandler_(message, token->getLine(), token->getColumn());
        } else {
            errorHandler_(message, 0, 0);
        }
    } else {
        std::cerr << "Parse Error: " << message << std::endl;
    }
}

void CHTLParser::reportUnexpectedToken(const std::string& expected) {
    auto token = currentToken();
    std::string message = "Unexpected token '" + (token ? token->getValue() : "EOF") + 
                         "', expected " + expected;
    reportError(message);
}

} // namespace CHTL