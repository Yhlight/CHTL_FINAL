#include "CHTL/CHTLParser.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/TemplateNode.h"
#include "CHTL/CHTLNode/CustomNode.h"
#include "CHTL/CHTLNode/ImportNode.h"
#include "CHTL/CHTLNode/NamespaceNode.h"
#include "CHTL/CHTLNode/ConfigurationNode.h"
#include "CHTL/CHTLNode/ModuleNode.h"
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

std::shared_ptr<BaseNode> CHTLParser::parse(const std::string& sourceCode) {
    // 简单的扫描器实现
    TokenList tokens;
    size_t pos = 0;
    
    if (m_debugMode) {
        std::cout << "[CHTLParser] Parsing source code: " << sourceCode.substr(0, 100) << "..." << std::endl;
    }
    
    while (pos < sourceCode.length()) {
        // 跳过空白字符
        while (pos < sourceCode.length() && std::isspace(sourceCode[pos])) {
            pos++;
        }
        
        if (pos >= sourceCode.length()) break;
        
        // 识别标识符和关键字
        if (std::isalpha(sourceCode[pos])) {
            size_t start = pos;
            while (pos < sourceCode.length() && (std::isalnum(sourceCode[pos]) || sourceCode[pos] == '-' || sourceCode[pos] == '_')) {
                pos++;
            }
            std::string value = sourceCode.substr(start, pos - start);
            
            // 检查是否为关键字
            if (value == "text") {
                tokens.addToken(TokenType::TEXT, value, 1, 1);
            } else if (value == "style") {
                tokens.addToken(TokenType::STYLE, value, 1, 1);
            } else if (value == "script") {
                tokens.addToken(TokenType::SCRIPT, value, 1, 1);
            } else if (value == "Template") {
                tokens.addToken(TokenType::TEMPLATE, value, 1, 1);
            } else if (value == "Custom") {
                tokens.addToken(TokenType::CUSTOM, value, 1, 1);
            } else if (value == "Origin") {
                tokens.addToken(TokenType::ORIGIN, value, 1, 1);
            } else if (value == "Import") {
                tokens.addToken(TokenType::IMPORT, value, 1, 1);
            } else if (value == "Namespace") {
                tokens.addToken(TokenType::NAMESPACE, value, 1, 1);
            } else if (value == "Configuration") {
                tokens.addToken(TokenType::CONFIGURATION, value, 1, 1);
            } else if (value == "Module") {
                // Only treat as MODULE token if it's at the start of a line or after [Module]
                // For configuration syntax like "Module Dependencies", treat as IDENTIFIER
                tokens.addToken(TokenType::IDENTIFIER, value, 1, 1);
            } else if (value == "except") {
                tokens.addToken(TokenType::EXCEPT, value, 1, 1);
            } else {
                tokens.addToken(TokenType::IDENTIFIER, value, 1, 1);
            }
        }
        // 识别数字
        else if (std::isdigit(sourceCode[pos])) {
            size_t start = pos;
            while (pos < sourceCode.length() && (std::isdigit(sourceCode[pos]) || sourceCode[pos] == '.')) {
                pos++;
            }
            std::string value = sourceCode.substr(start, pos - start);
            tokens.addToken(TokenType::NUMBER, value, 1, 1);
        }
        // 识别@符号
        else if (sourceCode[pos] == '@') {
            tokens.addToken(TokenType::AT, "@", 1, 1);
            pos++;
        }
        // 识别左大括号
        else if (sourceCode[pos] == '{') {
            tokens.addToken(TokenType::LEFT_BRACE, "{", 1, 1);
            pos++;
        }
        // 识别右大括号
        else if (sourceCode[pos] == '}') {
            tokens.addToken(TokenType::RIGHT_BRACE, "}", 1, 1);
            pos++;
        }
        // 识别冒号
        else if (sourceCode[pos] == ':') {
            tokens.addToken(TokenType::COLON, ":", 1, 1);
            pos++;
        }
        // 识别等号
        else if (sourceCode[pos] == '=') {
            tokens.addToken(TokenType::EQUALS, "=", 1, 1);
            pos++;
        }
        // 识别方括号关键字 [Template], [Custom], [Origin], [Import], [Namespace], [Configuration], [Module]
        else if (sourceCode[pos] == '[') {
            size_t start = pos;
            pos++; // 跳过 [
            
            // 查找匹配的 ]
            size_t endPos = pos;
            while (endPos < sourceCode.length() && sourceCode[endPos] != ']') {
                endPos++;
            }
            
            if (endPos < sourceCode.length()) {
                std::string keyword = sourceCode.substr(pos, endPos - pos);
                
                // 检查是否为已知的关键字
                if (keyword == "Template") {
                    tokens.addToken(TokenType::TEMPLATE, "[Template]", 1, 1);
                } else if (keyword == "Custom") {
                    tokens.addToken(TokenType::CUSTOM, "[Custom]", 1, 1);
                } else if (keyword == "Origin") {
                    tokens.addToken(TokenType::ORIGIN, "[Origin]", 1, 1);
                } else if (keyword == "Import") {
                    tokens.addToken(TokenType::IMPORT, "[Import]", 1, 1);
                } else if (keyword == "Namespace") {
                    tokens.addToken(TokenType::NAMESPACE, "[Namespace]", 1, 1);
                } else if (keyword == "Configuration") {
                    tokens.addToken(TokenType::CONFIGURATION, "[Configuration]", 1, 1);
                } else if (keyword == "Module") {
                    tokens.addToken(TokenType::MODULE, "[Module]", 1, 1);
                } else {
                    // 未知的方括号关键字，作为普通方括号处理
                    tokens.addToken(TokenType::LEFT_BRACKET, "[", 1, 1);
                    pos = start + 1;
                    continue;
                }
                
                pos = endPos + 1; // 跳过 ]
            } else {
                // 没有找到匹配的 ]，作为普通左方括号处理
                tokens.addToken(TokenType::LEFT_BRACKET, "[", 1, 1);
                pos = start + 1;
            }
        }
        // 识别分号
        else if (sourceCode[pos] == ';') {
            tokens.addToken(TokenType::SEMICOLON, ";", 1, 1);
            pos++;
        }
        // 识别井号（用于CSS颜色值）
        else if (sourceCode[pos] == '#') {
            tokens.addToken(TokenType::HASH, "#", 1, 1);
            pos++;
        }
        // 识别字符串
        else if (sourceCode[pos] == '"') {
            size_t start = pos;
            pos++;
            while (pos < sourceCode.length() && sourceCode[pos] != '"') {
                pos++;
            }
            if (pos < sourceCode.length()) {
                pos++;
                std::string value = sourceCode.substr(start + 1, pos - start - 2);
                tokens.addToken(TokenType::STRING, value, 1, 1);
            }
        }
        // 识别单引号字符串
        else if (sourceCode[pos] == '\'') {
            size_t start = pos;
            pos++;
            while (pos < sourceCode.length() && sourceCode[pos] != '\'') {
                pos++;
            }
            if (pos < sourceCode.length()) {
                pos++;
                std::string value = sourceCode.substr(start + 1, pos - start - 2);
                tokens.addToken(TokenType::STRING, value, 1, 1);
            }
        }
        // 识别单行注释 //
        else if (pos + 1 < sourceCode.length() && sourceCode[pos] == '/' && sourceCode[pos + 1] == '/') {
            size_t start = pos;
            pos += 2; // 跳过 //
            while (pos < sourceCode.length() && sourceCode[pos] != '\n') {
                pos++;
            }
            std::string value = sourceCode.substr(start + 2, pos - start - 2);
            tokens.addToken(TokenType::COMMENT, value, 1, 1);
        }
        // 识别多行注释 /* */
        else if (pos + 1 < sourceCode.length() && sourceCode[pos] == '/' && sourceCode[pos + 1] == '*') {
            size_t start = pos;
            pos += 2; // 跳过 /*
            while (pos + 1 < sourceCode.length() && !(sourceCode[pos] == '*' && sourceCode[pos + 1] == '/')) {
                pos++;
            }
            if (pos + 1 < sourceCode.length()) {
                pos += 2; // 跳过 */
                std::string value = sourceCode.substr(start + 2, pos - start - 4);
                tokens.addToken(TokenType::COMMENT, value, 1, 1);
            }
        }
        // 识别HTML注释 --
        else if (pos + 1 < sourceCode.length() && sourceCode[pos] == '-' && sourceCode[pos + 1] == '-') {
            size_t start = pos;
            pos += 2; // 跳过 --
            while (pos + 1 < sourceCode.length() && !(sourceCode[pos] == '-' && sourceCode[pos + 1] == '-')) {
                pos++;
            }
            if (pos + 1 < sourceCode.length()) {
                pos += 2; // 跳过 --
                std::string value = sourceCode.substr(start + 2, pos - start - 4);
                tokens.addToken(TokenType::COMMENT, value, 1, 1);
            }
        }
        // 其他字符
        else {
            pos++;
        }
    }
    
    if (m_debugMode) {
        std::cout << "[CHTLParser] Generated " << tokens.size() << " tokens:" << std::endl;
        for (size_t i = 0; i < tokens.size(); ++i) {
            const auto& token = tokens[i];
            std::cout << "  Token " << i << ": " << static_cast<int>(token.getType()) 
                      << " = \"" << token.getValue() << "\"" << std::endl;
        }
    }
    
    return parse(tokens);
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
                if (m_debugMode) {
                    std::cout << "[CHTLParser] Found TEMPLATE token, calling parseTemplate" << std::endl;
                }
                node = parseTemplate();
                if (node) {
                    document->addChild(node);
                } else {
                    if (m_debugMode) {
                        std::cout << "[CHTLParser] parseTemplate returned nullptr" << std::endl;
                    }
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
    
    // 检查是否为自定义语法
    if (checkToken(TokenType::AT)) {
        return parseCustom();
    }
    
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
    if (isAtEnd()) return nullptr;
    
    if (m_debugMode) {
        std::cout << "[CHTLParser] parseTemplate called, current token: " << currentToken().getValue() << std::endl;
    }
    
    if (checkToken(TokenType::TEMPLATE)) {
        nextToken(); // 消费 [Template]
        
        // 解析模板类型和名称
        if (checkToken(TokenType::AT)) {
            nextToken(); // 消费 @
            
            std::string templateType;
            std::string templateName;
            
            // 解析模板类型
            if (currentToken().getType() == TokenType::IDENTIFIER) {
                templateType = currentToken().getValue();
                nextToken();
                if (m_debugMode) {
                    std::cout << "[CHTLParser] Template type: " << templateType << std::endl;
                }
            } else {
                addError("Expected template type after @");
                return nullptr;
            }
            
            // 解析模板名称
            if (currentToken().getType() == TokenType::IDENTIFIER) {
                templateName = currentToken().getValue();
                nextToken();
                if (m_debugMode) {
                    std::cout << "[CHTLParser] Template name: " << templateName << std::endl;
                }
            } else {
                addError("Expected template name");
                return nullptr;
            }
            
            // 创建模板节点
            TemplateType type = TemplateType::UNKNOWN;
            if (templateType == "Style") {
                type = TemplateType::STYLE;
            } else if (templateType == "Element") {
                type = TemplateType::ELEMENT;
            } else if (templateType == "Var") {
                type = TemplateType::VAR;
            }
            
            auto templateNode = std::make_shared<TemplateNode>(type, templateName);
            templateNode->setPosition(currentToken().getLine(), currentToken().getColumn());
            
            // 解析模板内容
            if (checkToken(TokenType::LEFT_BRACE)) {
                nextToken(); // 消费 {
                
                skipWhitespace();
                
                // 根据模板类型解析内容
                if (type == TemplateType::STYLE) {
                    // 对于@Style模板，直接解析CSS属性
                    while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                        auto prop = parseStyleProperty();
                        if (!prop.first.empty()) {
                            templateNode->setAttribute(prop.first, prop.second);
                        } else {
                            break;
                        }
                        skipWhitespace();
                    }
                } else if (type == TemplateType::ELEMENT) {
                    // 对于@Element模板，解析HTML元素属性
                    while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                        if (m_debugMode) {
                            std::cout << "[CHTLParser] Parsing attribute, current token: " << currentToken().getValue() << std::endl;
                        }
                        auto attr = parseAttribute();
                        if (!attr.first.empty()) {
                            templateNode->setAttribute(attr.first, attr.second);
                        } else {
                            if (m_debugMode) {
                                std::cout << "[CHTLParser] Empty attribute, breaking" << std::endl;
                            }
                            break;
                        }
                        skipWhitespace();
                    }
                } else if (type == TemplateType::VAR) {
                    // 对于@Var模板，解析变量属性
                    while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                        auto prop = parseStyleProperty();
                        if (!prop.first.empty()) {
                            templateNode->setAttribute(prop.first, prop.second);
                        } else {
                            break;
                        }
                        skipWhitespace();
                    }
                } else {
                    // 对于其他类型，解析子元素
                    while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                        auto child = parseElement();
                        if (child) {
                            templateNode->addChild(child);
                        } else {
                            // 尝试解析其他类型的节点
                            if (checkToken(TokenType::TEXT)) {
                                child = parseText();
                                if (child) {
                                    templateNode->addChild(child);
                                }
                            } else if (checkToken(TokenType::STYLE)) {
                                child = parseStyle();
                                if (child) {
                                    templateNode->addChild(child);
                                }
                            } else {
                                break;
                            }
                        }
                        skipWhitespace();
                    }
                }
                
                if (checkToken(TokenType::RIGHT_BRACE)) {
                    nextToken(); // 消费 }
                } else {
                    addError("Expected '}' after template content");
                }
            }
            
            return templateNode;
        }
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseCustom() {
    if (isAtEnd()) return nullptr;
    
    // 检查是否是[Custom] @Style语法
    if (checkToken(TokenType::CUSTOM)) {
        nextToken(); // 消费 [Custom]
        
        // 检查@符号
        if (checkToken(TokenType::AT)) {
            nextToken(); // 消费 @
        } else {
            addError("Expected '@' after [Custom]");
            return nullptr;
        }
    }
    // 检查是否是直接的自定义语法：@Style, @Element, @Var
    else if (checkToken(TokenType::AT)) {
        nextToken(); // 消费 @
    } else {
        return nullptr;
    }
    
    std::string customType;
    std::string customName;
    
    // 解析自定义类型
    if (currentToken().getType() == TokenType::IDENTIFIER) {
        customType = currentToken().getValue();
        nextToken();
    } else {
        addError("Expected custom type after @");
        return nullptr;
    }
    
    // 解析自定义名称
    if (currentToken().getType() == TokenType::IDENTIFIER) {
        customName = currentToken().getValue();
        nextToken();
    } else {
        addError("Expected custom name");
        return nullptr;
    }
    
    // 创建自定义节点
    CustomType type = CustomType::UNKNOWN;
    if (customType == "Style") {
        type = CustomType::STYLE;
    } else if (customType == "Element") {
        type = CustomType::ELEMENT;
    } else if (customType == "Var") {
        type = CustomType::VAR;
    }
    
    auto customNode = std::make_shared<CustomNode>(type, customName);
    customNode->setPosition(currentToken().getLine(), currentToken().getColumn());
    
    // 解析自定义内容
    if (checkToken(TokenType::LEFT_BRACE)) {
        nextToken(); // 消费 {
        
        skipWhitespace();
        
        // 根据自定义类型解析内容
        if (type == CustomType::STYLE) {
            // 对于@Style，直接解析CSS属性
            while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                auto prop = parseStyleProperty();
                if (!prop.first.empty()) {
                    customNode->setAttribute(prop.first, prop.second);
                } else {
                    break;
                }
                skipWhitespace();
            }
        } else {
            // 对于其他类型，解析子元素
            while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                auto child = parseElement();
                if (child) {
                    customNode->addChild(child);
                } else {
                    // 尝试解析其他类型的节点
                    if (checkToken(TokenType::TEXT)) {
                        child = parseText();
                        if (child) {
                            customNode->addChild(child);
                        }
                    } else if (checkToken(TokenType::STYLE)) {
                        child = parseStyle();
                        if (child) {
                            customNode->addChild(child);
                        }
                    } else {
                        break;
                    }
                }
                skipWhitespace();
            }
        }
        
        if (checkToken(TokenType::RIGHT_BRACE)) {
            nextToken(); // 消费 }
        } else {
            addError("Expected '}' after custom content");
        }
    }
    
    return customNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseOrigin() {
    if (isAtEnd()) return nullptr;
    
    if (checkToken(TokenType::ORIGIN)) {
        nextToken(); // 消费 [Origin]
        
        // 解析原始类型
        std::string originType;
        if (checkToken(TokenType::AT)) {
            nextToken(); // 消费 @
            
            if (currentToken().getType() == TokenType::IDENTIFIER) {
                originType = currentToken().getValue();
                nextToken();
            } else {
                addError("Expected origin type after @");
                return nullptr;
            }
        } else {
            addError("Expected '@' after [Origin]");
            return nullptr;
        }
        
        // 创建原始节点（使用TextNode存储原始内容）
        auto originNode = std::make_shared<TextNode>();
        originNode->setPosition(currentToken().getLine(), currentToken().getColumn());
        
        // 设置节点类型为对应的原始类型
        if (originType == "Html") {
            originNode->setGeneratorComment(false);
            originNode->setName("origin_html");
        } else if (originType == "Style") {
            originNode->setGeneratorComment(false);
            originNode->setName("origin_style");
        } else if (originType == "JavaScript") {
            originNode->setGeneratorComment(false);
            originNode->setName("origin_javascript");
        } else {
            originNode->setGeneratorComment(false);
            originNode->setName("origin_" + originType);
        }
        
        // 解析原始内容
        if (checkToken(TokenType::LEFT_BRACE)) {
            nextToken(); // 消费 {
            
            // 收集原始内容
            std::string content;
            while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                content += currentToken().getValue();
                if (!isAtEnd()) {
                    nextToken();
                }
            }
            
            if (checkToken(TokenType::RIGHT_BRACE)) {
                nextToken(); // 消费 }
            } else {
                addError("Expected '}' after origin content");
            }
            
            originNode->setText(content);
        } else {
            addError("Expected '{' after origin type");
            return nullptr;
        }
        
        return originNode;
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseImport() {
    if (isAtEnd()) return nullptr;
    
    if (checkToken(TokenType::IMPORT)) {
        nextToken(); // 消费 [Import]
        
        // 解析导入类型
        ImportType importType = ImportType::UNKNOWN;
        if (checkToken(TokenType::IDENTIFIER)) {
            std::string typeStr = currentToken().getValue();
            nextToken();
            
            if (typeStr == "CHTL") {
                importType = ImportType::CHTL;
            } else if (typeStr == "HTML") {
                importType = ImportType::HTML;
            } else if (typeStr == "CSS") {
                importType = ImportType::CSS;
            } else if (typeStr == "JS") {
                importType = ImportType::JS;
            } else if (typeStr == "CMOD") {
                importType = ImportType::CMOD;
            } else if (typeStr == "CJMOD") {
                importType = ImportType::CJMOD;
            }
        }
        
        // 解析文件路径
        std::string filePath;
        if (currentToken().getType() == TokenType::STRING) {
            filePath = currentToken().getValue();
            nextToken();
        } else {
            addError("Expected file path after import type");
            return nullptr;
        }
        
        // 创建导入节点
        auto importNode = std::make_shared<ImportNode>(importType, filePath);
        importNode->setPosition(currentToken().getLine(), currentToken().getColumn());
        
        // 解析导入模式和项目
        if (checkToken(TokenType::LEFT_BRACE)) {
            nextToken(); // 消费 {
            
            skipWhitespace();
            
            // 解析导入项目
            while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                if (currentToken().getType() == TokenType::IDENTIFIER) {
                    std::string item = currentToken().getValue();
                    importNode->addImportItem(item);
                    nextToken();
                    
                    // 检查是否有别名
                    if (checkToken(TokenType::AS)) {
                        nextToken(); // 消费 as
                        if (currentToken().getType() == TokenType::IDENTIFIER) {
                            std::string alias = currentToken().getValue();
                            importNode->setAlias(alias);
                            nextToken();
                        } else {
                            addError("Expected alias after 'as'");
                        }
                    }
                } else if (currentToken().getType() == TokenType::ASTERISK) {
                    // 通配符导入
                    importNode->setImportMode(ImportMode::WILDCARD);
                    nextToken();
                } else {
                    break;
                }
                
                skipWhitespace();
            }
            
            if (checkToken(TokenType::RIGHT_BRACE)) {
                nextToken(); // 消费 }
            } else {
                addError("Expected '}' after import items");
            }
        } else {
            // 精确导入
            importNode->setImportMode(ImportMode::PRECISE);
        }
        
        return importNode;
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseConfiguration() {
    if (isAtEnd()) return nullptr;
    
    if (checkToken(TokenType::CONFIGURATION)) {
        nextToken(); // 消费 [Configuration]
        
        // 解析配置类型
        ConfigurationType configurationType = ConfigurationType::UNKNOWN;
        if (checkToken(TokenType::IDENTIFIER)) {
            std::string typeStr = currentToken().getValue();
            nextToken();
            
            if (typeStr == "Keyword") {
                configurationType = ConfigurationType::KEYWORD;
            } else if (typeStr == "Module") {
                configurationType = ConfigurationType::MODULE;
            } else if (typeStr == "Compiler") {
                configurationType = ConfigurationType::COMPILER;
            } else if (typeStr == "Output") {
                configurationType = ConfigurationType::OUTPUT;
            } else if (typeStr == "Debug") {
                configurationType = ConfigurationType::DEBUG;
            }
        }
        
        // 解析配置名称
        std::string configurationName;
        if (currentToken().getType() == TokenType::IDENTIFIER) {
            configurationName = currentToken().getValue();
            nextToken();
        } else {
            addError("Expected configuration name after configuration type");
            return nullptr;
        }
        
        // 创建配置节点
        auto configurationNode = std::make_shared<ConfigurationNode>(configurationType, configurationName);
        configurationNode->setPosition(currentToken().getLine(), currentToken().getColumn());
        
        // 解析配置内容
        if (checkToken(TokenType::LEFT_BRACE)) {
            nextToken(); // 消费 {
            
            skipWhitespace();
            
            // 解析配置项
            while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                if (currentToken().getType() == TokenType::IDENTIFIER) {
                    std::string key = currentToken().getValue();
                    nextToken();
                    
                    // 检查是否有值
                    if (checkToken(TokenType::COLON) || checkToken(TokenType::EQUALS)) {
                        nextToken(); // 消费 : 或 =
                        skipWhitespace();
                        
                        // 解析配置值（支持复合值）
                        std::string value;
                        while (!isAtEnd() && !checkToken(TokenType::SEMICOLON) && !checkToken(TokenType::RIGHT_BRACE)) {
                            if (currentToken().getType() == TokenType::STRING) {
                                value += currentToken().getValue();
                                nextToken();
                            } else if (currentToken().getType() == TokenType::IDENTIFIER) {
                                value += currentToken().getValue();
                                nextToken();
                            } else if (currentToken().getType() == TokenType::NUMBER) {
                                value += currentToken().getValue();
                                nextToken();
                            } else if (currentToken().getType() == TokenType::HASH) {
                                value += currentToken().getValue();
                                nextToken();
                            } else if (currentToken().getType() == TokenType::COLON || 
                                       currentToken().getType() == TokenType::EQUALS) {
                                // 跳过意外的冒号或等号
                                nextToken();
                            } else {
                                // 添加其他字符（如空格、连字符等）
                                value += currentToken().getValue();
                                nextToken();
                            }
                            
                            // 跳过空格
                            skipWhitespace();
                        }
                        
                        // 跳过分号（如果存在）
                        if (checkToken(TokenType::SEMICOLON)) {
                            nextToken();
                        }
                        
                        configurationNode->addConfigurationItem(key, value);
                    } else {
                        addError("Expected ':' or '=' after configuration key");
                        break;
                    }
                } else {
                    break;
                }
                
                skipWhitespace();
            }
            
            if (checkToken(TokenType::RIGHT_BRACE)) {
                nextToken(); // 消费 }
            } else {
                addError("Expected '}' after configuration content");
            }
        }
        
        return configurationNode;
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseNamespace() {
    if (isAtEnd()) return nullptr;
    
    if (checkToken(TokenType::NAMESPACE)) {
        nextToken(); // 消费 [Namespace]
        
        // 解析命名空间名称
        std::string namespaceName;
        if (currentToken().getType() == TokenType::IDENTIFIER) {
            namespaceName = currentToken().getValue();
            nextToken();
        } else {
            addError("Expected namespace name after [Namespace]");
            return nullptr;
        }
        
        // 创建命名空间节点
        auto namespaceNode = std::make_shared<NamespaceNode>(namespaceName);
        namespaceNode->setPosition(currentToken().getLine(), currentToken().getColumn());
        
        // 解析命名空间内容
        if (checkToken(TokenType::LEFT_BRACE)) {
            nextToken(); // 消费 {
            
            skipWhitespace();
            
            // 解析命名空间内容
            while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                auto child = parseElement();
                if (child) {
                    namespaceNode->addChild(child);
                } else {
                    // 尝试解析其他类型的节点
                    if (checkToken(TokenType::TEXT)) {
                        child = parseText();
                        if (child) {
                            namespaceNode->addChild(child);
                        }
                    } else if (checkToken(TokenType::STYLE)) {
                        child = parseStyle();
                        if (child) {
                            namespaceNode->addChild(child);
                        }
                    } else if (checkToken(TokenType::TEMPLATE)) {
                        child = parseTemplate();
                        if (child) {
                            namespaceNode->addChild(child);
                        }
                    } else if (checkToken(TokenType::CUSTOM)) {
                        child = parseCustom();
                        if (child) {
                            namespaceNode->addChild(child);
                        }
                    } else if (checkToken(TokenType::IMPORT)) {
                        child = parseImport();
                        if (child) {
                            namespaceNode->addChild(child);
                        }
                    } else {
                        break;
                    }
                }
                skipWhitespace();
            }
            
            if (checkToken(TokenType::RIGHT_BRACE)) {
                nextToken(); // 消费 }
            } else {
                addError("Expected '}' after namespace content");
            }
        }
        
        return namespaceNode;
    }
    
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseModule() {
    if (isAtEnd()) return nullptr;
    
    if (checkToken(TokenType::MODULE)) {
        nextToken(); // 消费 [Module]
        
        // 解析模块类型
        ModuleType moduleType = ModuleType::UNKNOWN;
        if (checkToken(TokenType::IDENTIFIER)) {
            std::string typeStr = currentToken().getValue();
            nextToken();
            
            if (typeStr == "CMOD") {
                moduleType = ModuleType::CMOD;
            } else if (typeStr == "CJMOD") {
                moduleType = ModuleType::CJMOD;
            } else if (typeStr == "HTML") {
                moduleType = ModuleType::HTML;
            } else if (typeStr == "CSS") {
                moduleType = ModuleType::CSS;
            } else if (typeStr == "JS") {
                moduleType = ModuleType::JS;
            }
        }
        
        // 解析模块名称
        std::string moduleName;
        if (currentToken().getType() == TokenType::IDENTIFIER) {
            moduleName = currentToken().getValue();
            nextToken();
        } else {
            addError("Expected module name after module type");
            return nullptr;
        }
        
        // 创建模块节点
        auto moduleNode = std::make_shared<ModuleNode>(moduleType, moduleName);
        moduleNode->setPosition(currentToken().getLine(), currentToken().getColumn());
        
        // 解析模块内容
        if (checkToken(TokenType::LEFT_BRACE)) {
            nextToken(); // 消费 {
            
            skipWhitespace();
            
            // 解析模块属性
            while (!checkToken(TokenType::RIGHT_BRACE) && !isAtEnd()) {
                if (currentToken().getType() == TokenType::IDENTIFIER) {
                    std::string key = currentToken().getValue();
                    nextToken();
                    
                    // 检查是否有值
                    if (checkToken(TokenType::COLON) || checkToken(TokenType::EQUALS)) {
                        nextToken(); // 消费 : 或 =
                        skipWhitespace();
                        
                        // 解析模块属性值
                        std::string value;
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
                            addError("Expected value after module attribute");
                            break;
                        }
                        
                        // 设置模块属性
                        if (key == "version") {
                            moduleNode->setModuleVersion(value);
                        } else if (key == "path") {
                            moduleNode->setModulePath(value);
                        } else if (key == "description") {
                            moduleNode->setModuleDescription(value);
                        } else if (key == "dependency") {
                            // 解析依赖
                            moduleNode->addModuleDependency(value);
                        } else if (key == "export") {
                            // 解析导出
                            moduleNode->setModuleExport(value, "");
                        } else {
                            addWarning("Unknown module attribute: " + key);
                        }
                    } else {
                        addError("Expected ':' or '=' after module attribute");
                        break;
                    }
                } else {
                    break;
                }
                
                skipWhitespace();
            }
            
            if (checkToken(TokenType::RIGHT_BRACE)) {
                nextToken(); // 消费 }
            } else {
                addError("Expected '}' after module content");
            }
        }
        
        return moduleNode;
    }
    
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
            
            // 解析属性值（支持复合值）
            while (!isAtEnd() && !checkToken(TokenType::SEMICOLON) && !checkToken(TokenType::RIGHT_BRACE)) {
                if (currentToken().getType() == TokenType::STRING) {
                    value += currentToken().getValue();
                    nextToken();
                } else if (currentToken().getType() == TokenType::IDENTIFIER) {
                    value += currentToken().getValue();
                    nextToken();
                } else if (currentToken().getType() == TokenType::NUMBER) {
                    value += currentToken().getValue();
                    nextToken();
                } else if (currentToken().getType() == TokenType::HASH) {
                    value += currentToken().getValue();
                    nextToken();
                } else if (currentToken().getType() == TokenType::COLON || 
                           currentToken().getType() == TokenType::EQUALS) {
                    // 跳过意外的冒号或等号
                    nextToken();
                } else {
                    // 添加其他字符（如空格、连字符等）
                    value += currentToken().getValue();
                    nextToken();
                }
                
                // 跳过空格
                skipWhitespace();
            }
            
            // 跳过分号（如果存在）
            if (checkToken(TokenType::SEMICOLON)) {
                nextToken();
            }
        }
    }
    
    if (m_debugMode) {
        std::cout << "[CHTLParser] Parsed attribute: " << key << " = " << value << std::endl;
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
            
            // 解析属性值（支持复合值）
            while (!isAtEnd() && !checkToken(TokenType::SEMICOLON) && !checkToken(TokenType::RIGHT_BRACE)) {
                if (currentToken().getType() == TokenType::STRING) {
                    value += currentToken().getValue();
                    nextToken();
                } else if (currentToken().getType() == TokenType::IDENTIFIER) {
                    value += currentToken().getValue();
                    nextToken();
                } else if (currentToken().getType() == TokenType::NUMBER) {
                    value += currentToken().getValue();
                    nextToken();
                } else if (currentToken().getType() == TokenType::HASH) {
                    value += currentToken().getValue();
                    nextToken();
                } else if (currentToken().getType() == TokenType::COLON || 
                           currentToken().getType() == TokenType::EQUALS) {
                    // 跳过意外的冒号或等号
                    nextToken();
                } else {
                    // 添加其他字符（如空格、连字符等）
                    value += currentToken().getValue();
                    nextToken();
                }
                
                // 跳过空格
                skipWhitespace();
            }
            
            // 跳过分号（如果存在）
            if (checkToken(TokenType::SEMICOLON)) {
                nextToken();
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
    // 跳过空白字符
    while (!isAtEnd() && 
           (currentToken().getValue() == " " || 
            currentToken().getValue() == "\t" || 
            currentToken().getValue() == "\n" || 
            currentToken().getValue() == "\r" ||
            (currentToken().getType() == TokenType::IDENTIFIER && 
             std::all_of(currentToken().getValue().begin(), currentToken().getValue().end(), ::isspace)))) {
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