#include "CHTLParser.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

CHTLParser::CHTLParser() 
    : currentTokenIndex(0), debugMode(false), strictMode(false) {
    rootNode = std::make_shared<BaseNode>(NodeType::ELEMENT, "document");
}

CHTLParser::CHTLParser(const TokenStream& tokens) 
    : tokens(tokens), currentTokenIndex(0), debugMode(false), strictMode(false) {
    rootNode = std::make_shared<BaseNode>(NodeType::ELEMENT, "document");
}

void CHTLParser::setTokens(const TokenStream& tokens) {
    this->tokens = tokens;
    currentTokenIndex = 0;
    errors.clear();
}

std::shared_ptr<BaseNode> CHTLParser::parse() {
    try {
        rootNode = parseDocument();
        
        if (debugMode) {
            std::cout << "解析完成，根节点类型: " << static_cast<int>(rootNode->getNodeType()) << std::endl;
        }
        
        return rootNode;
        
    } catch (const std::exception& e) {
        addError("解析过程中发生错误: " + std::string(e.what()));
        return rootNode;
    }
}

Token CHTLParser::currentToken() const {
    return tokens.peek(0);
}

Token CHTLParser::peekToken(size_t offset) const {
    return tokens.peek(offset);
}

void CHTLParser::advanceToken() {
    if (currentTokenIndex < tokens.size()) {
        currentTokenIndex++;
    }
}

bool CHTLParser::match(TokenType type) const {
    return currentToken().type == type;
}

bool CHTLParser::match(const std::string& value) const {
    return currentToken().value == value;
}

bool CHTLParser::match(TokenType type, const std::string& value) const {
    return currentToken().type == type && currentToken().value == value;
}

void CHTLParser::skipWhitespaceAndComments() {
    while (currentTokenIndex < tokens.size()) {
        Token token = currentToken();
        if (token.type == TokenType::WHITESPACE || 
            token.type == TokenType::NEWLINE ||
            token.type == TokenType::SINGLE_COMMENT ||
            token.type == TokenType::MULTI_COMMENT ||
            token.type == TokenType::GENERATOR_COMMENT) {
            advanceToken();
        } else {
            break;
        }
    }
}

void CHTLParser::error(const std::string& message) const {
    Token token = currentToken();
    error(message, token);
}

void CHTLParser::error(const std::string& message, const Token& token) const {
    std::ostringstream oss;
    oss << "第" << token.line << "行第" << token.column << "列: " << message;
    errors.push_back(oss.str());
}

std::shared_ptr<BaseNode> CHTLParser::parseDocument() {
    auto document = std::make_shared<BaseNode>(NodeType::ELEMENT, "document");
    
    skipWhitespaceAndComments();
    
    while (currentTokenIndex < tokens.size()) {
        Token token = currentToken();
        
        if (token.type == TokenType::END_OF_FILE) {
            break;
        }
        
        std::shared_ptr<BaseNode> node = nullptr;
        
        // 检查模板定义
        if (token.type == TokenType::TEMPLATE) {
            node = parseTemplate();
        }
        // 检查自定义定义
        else if (token.type == TokenType::CUSTOM) {
            node = parseCustom();
        }
        // 检查原始嵌入定义
        else if (token.type == TokenType::ORIGIN) {
            node = parseOrigin();
        }
        // 检查导入语句
        else if (token.type == TokenType::IMPORT) {
            node = parseImport();
        }
        // 检查命名空间定义
        else if (token.type == TokenType::NAMESPACE) {
            node = parseNamespace();
        }
        // 检查配置定义
        else if (token.type == TokenType::CONFIGURATION) {
            node = parseConfiguration();
        }
        // 检查use语句
        else if (token.type == TokenType::USE) {
            node = parseUse();
        }
        // 检查HTML元素
        else if (token.type == TokenType::IDENTIFIER) {
            node = parseElement();
        }
        // 检查文本节点
        else if (token.type == TokenType::TEXT) {
            node = parseText();
        }
        // 检查注释
        else if (token.type == TokenType::SINGLE_COMMENT || 
                 token.type == TokenType::MULTI_COMMENT ||
                 token.type == TokenType::GENERATOR_COMMENT) {
            node = parseComment();
        }
        else {
            error("意外的token: " + token.getTypeName() + " (" + token.value + ")");
            advanceToken();
            continue;
        }
        
        if (node) {
            document->addChild(node);
        }
        
        skipWhitespaceAndComments();
    }
    
    return document;
}

std::shared_ptr<BaseNode> CHTLParser::parseElement() {
    if (!match(TokenType::IDENTIFIER)) {
        error("期望元素名称");
        return nullptr;
    }
    
    Token elementToken = currentToken();
    std::string elementName = elementToken.value;
    advanceToken();
    
    auto element = std::make_shared<ElementNode>(elementName);
    element->setPosition(elementToken.line, elementToken.column, elementToken.position);
    
    skipWhitespaceAndComments();
    
    // 解析属性
    parseAttributes(std::static_pointer_cast<ElementNode>(element));
    
    skipWhitespaceAndComments();
    
    // 检查是否有子内容
    if (match(TokenType::LEFT_BRACE)) {
        advanceToken(); // 跳过 '{'
        
        skipWhitespaceAndComments();
        
        while (!match(TokenType::RIGHT_BRACE) && currentTokenIndex < tokens.size()) {
            Token token = currentToken();
            
            if (token.type == TokenType::END_OF_FILE) {
                error("期望 '}' 但遇到文件结束");
                break;
            }
            
            std::shared_ptr<BaseNode> child = nullptr;
            
            // 检查子元素
            if (token.type == TokenType::IDENTIFIER) {
                child = parseElement();
            }
            // 检查文本节点
            else if (token.type == TokenType::TEXT) {
                child = parseText();
            }
            // 检查注释
            else if (token.type == TokenType::SINGLE_COMMENT || 
                     token.type == TokenType::MULTI_COMMENT ||
                     token.type == TokenType::GENERATOR_COMMENT) {
                child = parseComment();
            }
            // 检查样式块
            else if (token.type == TokenType::STYLE) {
                child = parseStyleBlock();
            }
            // 检查脚本块
            else if (token.type == TokenType::SCRIPT) {
                child = parseScriptBlock();
            }
            else {
                error("意外的token: " + token.getTypeName() + " (" + token.value + ")");
                advanceToken();
                continue;
            }
            
            if (child) {
                element->addChild(child);
            }
            
            skipWhitespaceAndComments();
        }
        
        if (match(TokenType::RIGHT_BRACE)) {
            advanceToken(); // 跳过 '}'
        } else {
            error("期望 '}'");
        }
    }
    
    return element;
}

std::shared_ptr<BaseNode> CHTLParser::parseText() {
    if (!match(TokenType::TEXT)) {
        error("期望文本内容");
        return nullptr;
    }
    
    Token textToken = currentToken();
    advanceToken();
    
    auto textNode = std::make_shared<TextNode>(textToken.value);
    textNode->setPosition(textToken.line, textToken.column, textToken.position);
    
    return textNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseComment() {
    Token commentToken = currentToken();
    advanceToken();
    
    auto commentNode = std::make_shared<BaseNode>(NodeType::COMMENT, "comment", commentToken.value);
    commentNode->setPosition(commentToken.line, commentToken.column, commentToken.position);
    
    return commentNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseTemplate() {
    if (!match(TokenType::TEMPLATE)) {
        error("期望 [Template]");
        return nullptr;
    }
    
    advanceToken(); // 跳过 [Template]
    
    skipWhitespaceAndComments();
    
    if (!match(TokenType::AT)) {
        error("期望 @ 符号");
        return nullptr;
    }
    
    advanceToken(); // 跳过 @
    
    if (!match(TokenType::IDENTIFIER)) {
        error("期望模板类型");
        return nullptr;
    }
    
    Token typeToken = currentToken();
    std::string templateType = typeToken.value;
    advanceToken();
    
    skipWhitespaceAndComments();
    
    if (!match(TokenType::IDENTIFIER)) {
        error("期望模板名称");
        return nullptr;
    }
    
    Token nameToken = currentToken();
    std::string templateName = nameToken.value;
    advanceToken();
    
    auto templateNode = std::make_shared<BaseNode>(NodeType::TEMPLATE, templateName);
    templateNode->setAttribute("type", templateType);
    templateNode->setPosition(typeToken.line, typeToken.column, typeToken.position);
    
    skipWhitespaceAndComments();
    
    if (match(TokenType::LEFT_BRACE)) {
        advanceToken(); // 跳过 '{'
        
        skipWhitespaceAndComments();
        
        while (!match(TokenType::RIGHT_BRACE) && currentTokenIndex < tokens.size()) {
            Token token = currentToken();
            
            if (token.type == TokenType::END_OF_FILE) {
                error("期望 '}' 但遇到文件结束");
                break;
            }
            
            std::shared_ptr<BaseNode> child = nullptr;
            
            if (token.type == TokenType::IDENTIFIER) {
                child = parseElement();
            } else if (token.type == TokenType::TEXT) {
                child = parseText();
            } else {
                error("意外的token: " + token.getTypeName() + " (" + token.value + ")");
                advanceToken();
                continue;
            }
            
            if (child) {
                templateNode->addChild(child);
            }
            
            skipWhitespaceAndComments();
        }
        
        if (match(TokenType::RIGHT_BRACE)) {
            advanceToken(); // 跳过 '}'
        } else {
            error("期望 '}'");
        }
    }
    
    // 存储模板定义
    templates[templateName] = templateNode;
    
    return templateNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseCustom() {
    if (!match(TokenType::CUSTOM)) {
        error("期望 [Custom]");
        return nullptr;
    }
    
    advanceToken(); // 跳过 [Custom]
    
    skipWhitespaceAndComments();
    
    if (!match(TokenType::AT)) {
        error("期望 @ 符号");
        return nullptr;
    }
    
    advanceToken(); // 跳过 @
    
    if (!match(TokenType::IDENTIFIER)) {
        error("期望自定义类型");
        return nullptr;
    }
    
    Token typeToken = currentToken();
    std::string customType = typeToken.value;
    advanceToken();
    
    skipWhitespaceAndComments();
    
    if (!match(TokenType::IDENTIFIER)) {
        error("期望自定义名称");
        return nullptr;
    }
    
    Token nameToken = currentToken();
    std::string customName = nameToken.value;
    advanceToken();
    
    auto customNode = std::make_shared<BaseNode>(NodeType::CUSTOM, customName);
    customNode->setAttribute("type", customType);
    customNode->setPosition(typeToken.line, typeToken.column, typeToken.position);
    
    skipWhitespaceAndComments();
    
    if (match(TokenType::LEFT_BRACE)) {
        advanceToken(); // 跳过 '{'
        
        skipWhitespaceAndComments();
        
        while (!match(TokenType::RIGHT_BRACE) && currentTokenIndex < tokens.size()) {
            Token token = currentToken();
            
            if (token.type == TokenType::END_OF_FILE) {
                error("期望 '}' 但遇到文件结束");
                break;
            }
            
            std::shared_ptr<BaseNode> child = nullptr;
            
            if (token.type == TokenType::IDENTIFIER) {
                child = parseElement();
            } else if (token.type == TokenType::TEXT) {
                child = parseText();
            } else {
                error("意外的token: " + token.getTypeName() + " (" + token.value + ")");
                advanceToken();
                continue;
            }
            
            if (child) {
                customNode->addChild(child);
            }
            
            skipWhitespaceAndComments();
        }
        
        if (match(TokenType::RIGHT_BRACE)) {
            advanceToken(); // 跳过 '}'
        } else {
            error("期望 '}'");
        }
    }
    
    // 存储自定义定义
    customs[customName] = customNode;
    
    return customNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseOrigin() {
    if (!match(TokenType::ORIGIN)) {
        error("期望 [Origin]");
        return nullptr;
    }
    
    advanceToken(); // 跳过 [Origin]
    
    skipWhitespaceAndComments();
    
    if (!match(TokenType::AT)) {
        error("期望 @ 符号");
        return nullptr;
    }
    
    advanceToken(); // 跳过 @
    
    if (!match(TokenType::IDENTIFIER)) {
        error("期望原始嵌入类型");
        return nullptr;
    }
    
    Token typeToken = currentToken();
    std::string originType = typeToken.value;
    advanceToken();
    
    skipWhitespaceAndComments();
    
    std::string originName;
    if (match(TokenType::IDENTIFIER)) {
        Token nameToken = currentToken();
        originName = nameToken.value;
        advanceToken();
    }
    
    auto originNode = std::make_shared<BaseNode>(NodeType::ORIGIN, originName);
    originNode->setAttribute("type", originType);
    originNode->setPosition(typeToken.line, typeToken.column, typeToken.position);
    
    skipWhitespaceAndComments();
    
    if (match(TokenType::LEFT_BRACE)) {
        advanceToken(); // 跳过 '{'
        
        std::string content;
        while (!match(TokenType::RIGHT_BRACE) && currentTokenIndex < tokens.size()) {
            Token token = currentToken();
            if (token.type == TokenType::END_OF_FILE) {
                error("期望 '}' 但遇到文件结束");
                break;
            }
            content += token.value + " ";
            advanceToken();
        }
        
        if (match(TokenType::RIGHT_BRACE)) {
            advanceToken(); // 跳过 '}'
        } else {
            error("期望 '}'");
        }
        
        originNode->setValue(content);
    }
    
    return originNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseImport() {
    if (!match(TokenType::IMPORT)) {
        error("期望 [Import]");
        return nullptr;
    }
    
    advanceToken(); // 跳过 [Import]
    
    skipWhitespaceAndComments();
    
    if (!match(TokenType::AT)) {
        error("期望 @ 符号");
        return nullptr;
    }
    
    advanceToken(); // 跳过 @
    
    if (!match(TokenType::IDENTIFIER)) {
        error("期望导入类型");
        return nullptr;
    }
    
    Token typeToken = currentToken();
    std::string importType = typeToken.value;
    advanceToken();
    
    skipWhitespaceAndComments();
    
    if (!match(TokenType::FROM)) {
        error("期望 from 关键字");
        return nullptr;
    }
    
    advanceToken(); // 跳过 from
    
    skipWhitespaceAndComments();
    
    if (!match(TokenType::STRING) && !match(TokenType::LITERAL)) {
        error("期望文件路径");
        return nullptr;
    }
    
    Token pathToken = currentToken();
    std::string filePath = pathToken.value;
    advanceToken();
    
    auto importNode = std::make_shared<BaseNode>(NodeType::IMPORT, "import");
    importNode->setAttribute("type", importType);
    importNode->setAttribute("path", filePath);
    importNode->setPosition(typeToken.line, typeToken.column, typeToken.position);
    
    skipWhitespaceAndComments();
    
    if (match(TokenType::AS)) {
        advanceToken(); // 跳过 as
        
        skipWhitespaceAndComments();
        
        if (match(TokenType::IDENTIFIER)) {
            Token aliasToken = currentToken();
            importNode->setAttribute("alias", aliasToken.value);
            advanceToken();
        } else {
            error("期望别名");
        }
    }
    
    return importNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseNamespace() {
    if (!match(TokenType::NAMESPACE)) {
        error("期望 [Namespace]");
        return nullptr;
    }
    
    advanceToken(); // 跳过 [Namespace]
    
    skipWhitespaceAndComments();
    
    if (!match(TokenType::IDENTIFIER)) {
        error("期望命名空间名称");
        return nullptr;
    }
    
    Token nameToken = currentToken();
    std::string namespaceName = nameToken.value;
    advanceToken();
    
    auto namespaceNode = std::make_shared<BaseNode>(NodeType::NAMESPACE, namespaceName);
    namespaceNode->setPosition(nameToken.line, nameToken.column, nameToken.position);
    
    skipWhitespaceAndComments();
    
    if (match(TokenType::LEFT_BRACE)) {
        advanceToken(); // 跳过 '{'
        
        skipWhitespaceAndComments();
        
        while (!match(TokenType::RIGHT_BRACE) && currentTokenIndex < tokens.size()) {
            Token token = currentToken();
            
            if (token.type == TokenType::END_OF_FILE) {
                error("期望 '}' 但遇到文件结束");
                break;
            }
            
            std::shared_ptr<BaseNode> child = nullptr;
            
            if (token.type == TokenType::TEMPLATE) {
                child = parseTemplate();
            } else if (token.type == TokenType::CUSTOM) {
                child = parseCustom();
            } else if (token.type == TokenType::ORIGIN) {
                child = parseOrigin();
            } else if (token.type == TokenType::IMPORT) {
                child = parseImport();
            } else if (token.type == TokenType::NAMESPACE) {
                child = parseNamespace();
            } else if (token.type == TokenType::CONFIGURATION) {
                child = parseConfiguration();
            } else {
                error("意外的token: " + token.getTypeName() + " (" + token.value + ")");
                advanceToken();
                continue;
            }
            
            if (child) {
                namespaceNode->addChild(child);
            }
            
            skipWhitespaceAndComments();
        }
        
        if (match(TokenType::RIGHT_BRACE)) {
            advanceToken(); // 跳过 '}'
        } else {
            error("期望 '}'");
        }
    }
    
    return namespaceNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseConfiguration() {
    if (!match(TokenType::CONFIGURATION)) {
        error("期望 [Configuration]");
        return nullptr;
    }
    
    advanceToken(); // 跳过 [Configuration]
    
    skipWhitespaceAndComments();
    
    std::string configName;
    if (match(TokenType::AT)) {
        advanceToken(); // 跳过 @
        
        if (match(TokenType::IDENTIFIER)) {
            Token nameToken = currentToken();
            configName = nameToken.value;
            advanceToken();
        } else {
            error("期望配置名称");
        }
    }
    
    auto configNode = std::make_shared<BaseNode>(NodeType::CONFIGURATION, configName);
    
    skipWhitespaceAndComments();
    
    if (match(TokenType::LEFT_BRACE)) {
        advanceToken(); // 跳过 '{'
        
        skipWhitespaceAndComments();
        
        while (!match(TokenType::RIGHT_BRACE) && currentTokenIndex < tokens.size()) {
            Token token = currentToken();
            
            if (token.type == TokenType::END_OF_FILE) {
                error("期望 '}' 但遇到文件结束");
                break;
            }
            
            if (token.type == TokenType::IDENTIFIER) {
                std::string key = token.value;
                advanceToken();
                
                skipWhitespaceAndComments();
                
                if (match(TokenType::EQUAL) || match(TokenType::COLON)) {
                    advanceToken(); // 跳过 = 或 :
                    
                    skipWhitespaceAndComments();
                    
                    if (match(TokenType::STRING) || match(TokenType::LITERAL) || match(TokenType::NUMBER)) {
                        Token valueToken = currentToken();
                        configNode->setAttribute(key, valueToken.value);
                        advanceToken();
                    } else {
                        error("期望配置值");
                    }
                } else {
                    error("期望 = 或 :");
                }
            } else {
                error("意外的token: " + token.getTypeName() + " (" + token.value + ")");
                advanceToken();
            }
            
            skipWhitespaceAndComments();
            
            if (match(TokenType::SEMICOLON)) {
                advanceToken(); // 跳过 ;
            }
            
            skipWhitespaceAndComments();
        }
        
        if (match(TokenType::RIGHT_BRACE)) {
            advanceToken(); // 跳过 '}'
        } else {
            error("期望 '}'");
        }
    }
    
    return configNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseUse() {
    if (!match(TokenType::USE)) {
        error("期望 use");
        return nullptr;
    }
    
    advanceToken(); // 跳过 use
    
    skipWhitespaceAndComments();
    
    if (!match(TokenType::IDENTIFIER)) {
        error("期望使用项");
        return nullptr;
    }
    
    Token useToken = currentToken();
    std::string useItem = useToken.value;
    advanceToken();
    
    auto useNode = std::make_shared<BaseNode>(NodeType::OPERATOR, "use");
    useNode->setValue(useItem);
    useNode->setPosition(useToken.line, useToken.column, useToken.position);
    
    return useNode;
}

void CHTLParser::parseAttributes(std::shared_ptr<ElementNode> element) {
    while (currentTokenIndex < tokens.size()) {
        Token token = currentToken();
        
        // 如果遇到花括号，说明属性解析结束
        if (token.type == TokenType::LEFT_BRACE) {
            break;
        }
        
        // 如果遇到分号，跳过
        if (token.type == TokenType::SEMICOLON) {
            advanceToken();
            skipWhitespaceAndComments();
            continue;
        }
        
        // 解析属性
        if (token.type == TokenType::IDENTIFIER) {
            std::string attrName = token.value;
            advanceToken();
            
            skipWhitespaceAndComments();
            
            // 检查是否有属性值
            if (match(TokenType::COLON) || match(TokenType::EQUAL)) {
                advanceToken(); // 跳过 : 或 =
                
                skipWhitespaceAndComments();
                
                std::string attrValue = parseAttributeValue();
                element->setAttribute(attrName, attrValue);
            } else {
                // 无修饰字面量属性
                element->setAttribute(attrName, attrName);
            }
        } else {
            break;
        }
        
        skipWhitespaceAndComments();
    }
}

std::string CHTLParser::parseAttributeValue() {
    std::string value;
    
    if (match(TokenType::STRING)) {
        value = currentToken().value;
        advanceToken();
    } else if (match(TokenType::LITERAL)) {
        value = currentToken().value;
        advanceToken();
    } else if (match(TokenType::NUMBER)) {
        value = currentToken().value;
        advanceToken();
    } else {
        error("期望属性值");
    }
    
    return value;
}

std::shared_ptr<BaseNode> CHTLParser::parseStyleBlock() {
    if (!match(TokenType::STYLE)) {
        error("期望 style");
        return nullptr;
    }
    
    Token styleToken = currentToken();
    advanceToken();
    
    auto styleNode = std::make_shared<BaseNode>(NodeType::STYLE, "style");
    styleNode->setPosition(styleToken.line, styleToken.column, styleToken.position);
    
    skipWhitespaceAndComments();
    
    if (match(TokenType::LEFT_BRACE)) {
        advanceToken(); // 跳过 '{'
        
        parseStyleProperties(styleNode);
        
        if (match(TokenType::RIGHT_BRACE)) {
            advanceToken(); // 跳过 '}'
        } else {
            error("期望 '}'");
        }
    }
    
    return styleNode;
}

void CHTLParser::parseStyleProperties(std::shared_ptr<BaseNode> styleNode) {
    while (currentTokenIndex < tokens.size()) {
        Token token = currentToken();
        
        if (token.type == TokenType::RIGHT_BRACE) {
            break;
        }
        
        if (token.type == TokenType::IDENTIFIER) {
            std::string propertyName = token.value;
            advanceToken();
            
            skipWhitespaceAndComments();
            
            if (match(TokenType::COLON)) {
                advanceToken(); // 跳过 :
                
                skipWhitespaceAndComments();
                
                std::string propertyValue = parseAttributeValue();
                styleNode->setAttribute(propertyName, propertyValue);
            } else {
                error("期望 :");
            }
        } else {
            break;
        }
        
        skipWhitespaceAndComments();
        
        if (match(TokenType::SEMICOLON)) {
            advanceToken(); // 跳过 ;
        }
        
        skipWhitespaceAndComments();
    }
}

std::shared_ptr<BaseNode> CHTLParser::parseScriptBlock() {
    if (!match(TokenType::SCRIPT)) {
        error("期望 script");
        return nullptr;
    }
    
    Token scriptToken = currentToken();
    advanceToken();
    
    auto scriptNode = std::make_shared<BaseNode>(NodeType::SCRIPT, "script");
    scriptNode->setPosition(scriptToken.line, scriptToken.column, scriptToken.position);
    
    skipWhitespaceAndComments();
    
    if (match(TokenType::LEFT_BRACE)) {
        advanceToken(); // 跳过 '{'
        
        std::string scriptContent;
        while (!match(TokenType::RIGHT_BRACE) && currentTokenIndex < tokens.size()) {
            Token token = currentToken();
            if (token.type == TokenType::END_OF_FILE) {
                error("期望 '}' 但遇到文件结束");
                break;
            }
            scriptContent += token.value + " ";
            advanceToken();
        }
        
        if (match(TokenType::RIGHT_BRACE)) {
            advanceToken(); // 跳过 '}'
        } else {
            error("期望 '}'");
        }
        
        scriptNode->setValue(scriptContent);
    }
    
    return scriptNode;
}

std::string CHTLParser::parseExpression() {
    // 简化的表达式解析
    std::string expression;
    
    while (currentTokenIndex < tokens.size()) {
        Token token = currentToken();
        
        if (token.type == TokenType::SEMICOLON || 
            token.type == TokenType::RIGHT_BRACE ||
            token.type == TokenType::COMMA) {
            break;
        }
        
        expression += token.value + " ";
        advanceToken();
    }
    
    return expression;
}

std::string CHTLParser::parseConditionalExpression() {
    return parseExpression();
}

std::string CHTLParser::parseLogicalExpression() {
    return parseExpression();
}

std::string CHTLParser::parseEqualityExpression() {
    return parseExpression();
}

std::string CHTLParser::parseRelationalExpression() {
    return parseExpression();
}

std::string CHTLParser::parseAdditiveExpression() {
    return parseExpression();
}

std::string CHTLParser::parseMultiplicativeExpression() {
    return parseExpression();
}

std::string CHTLParser::parseUnaryExpression() {
    return parseExpression();
}

std::string CHTLParser::parsePrimaryExpression() {
    return parseExpression();
}

std::string CHTLParser::parseSelector() {
    return parseExpression();
}

std::string CHTLParser::parsePath() {
    return parseExpression();
}

void CHTLParser::reset() {
    currentTokenIndex = 0;
    errors.clear();
    rootNode = std::make_shared<BaseNode>(NodeType::ELEMENT, "document");
}

bool CHTLParser::validate() const {
    return errors.empty();
}

std::vector<std::string> CHTLParser::getErrors() const {
    return errors;
}

} // namespace CHTL