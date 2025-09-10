#include "CHTLParser.h"
#include <iostream>
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

void CHTLParser::addError(const std::string& message) {
    errors.push_back(message);
    if (debugMode) {
        std::cout << "错误: " << message << std::endl;
    }
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
    
    skipWhitespaceAndComments();
    
    // 检查是否为text块语法 text { }
    if (match(TokenType::LEFT_BRACE)) {
        advanceToken(); // 跳过 {
        
        skipWhitespaceAndComments();
        
        std::string textContent;
        
        // 解析文本内容
        while (!match(TokenType::RIGHT_BRACE) && currentTokenIndex < tokens.size()) {
            Token token = currentToken();
            
            if (token.type == TokenType::END_OF_FILE) {
                error("期望 '}' 但遇到文件结束");
                break;
            }
            
            // 收集文本内容
            if (token.type == TokenType::TEXT_CONTENT || 
                token.type == TokenType::UNQUOTED_LITERAL ||
                token.type == TokenType::SINGLE_QUOTED_LITERAL ||
                token.type == TokenType::DOUBLE_QUOTED_LITERAL) {
                textContent += token.value;
                advanceToken();
            } else if (token.type == TokenType::WHITESPACE || token.type == TokenType::NEWLINE) {
                textContent += token.value;
                advanceToken();
            } else {
                error("文本块中意外的token: " + token.getTypeName() + " (" + token.value + ")");
                advanceToken();
            }
            
            skipWhitespaceAndComments();
        }
        
        if (match(TokenType::RIGHT_BRACE)) {
            advanceToken(); // 跳过 }
        } else {
            error("期望 '}'");
        }
        
        auto textNode = std::make_shared<TextNode>(textContent);
        textNode->setPosition(textToken.line, textToken.column, textToken.position);
        
        return textNode;
    }
    // 检查是否为text属性语法 text: "content"
    else if (match(TokenType::COLON)) {
        advanceToken(); // 跳过 :
        
        skipWhitespaceAndComments();
        
        std::string textContent;
        
        // 解析属性值
        if (match(TokenType::UNQUOTED_LITERAL)) {
            textContent = currentToken().value;
            advanceToken();
        } else if (match(TokenType::SINGLE_QUOTED_LITERAL) || match(TokenType::DOUBLE_QUOTED_LITERAL)) {
            textContent = currentToken().value;
            advanceToken();
        } else {
            error("期望文本内容");
        }
        
        auto textNode = std::make_shared<TextNode>(textContent);
        textNode->setPosition(textToken.line, textToken.column, textToken.position);
        
        return textNode;
    }
    // 普通文本内容
    else {
        auto textNode = std::make_shared<TextNode>(textToken.value);
        textNode->setPosition(textToken.line, textToken.column, textToken.position);
        
        return textNode;
    }
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
    
    // 检查模板类型
    if (match(TokenType::TEMPLATE_STYLE)) {
        return parseStyleTemplate();
    } else if (match(TokenType::TEMPLATE_ELEMENT)) {
        return parseElementTemplate();
    } else if (match(TokenType::TEMPLATE_VAR)) {
        return parseVariableTemplate();
    } else {
        error("期望 @Style, @Element 或 @Var");
        return nullptr;
    }
}

std::shared_ptr<BaseNode> CHTLParser::parseStyleTemplate() {
    if (!match(TokenType::TEMPLATE_STYLE)) {
        error("期望 @Style");
        return nullptr;
    }
    
    Token styleToken = currentToken();
    advanceToken(); // 跳过 @Style
    
    skipWhitespaceAndComments();
    
    if (!match(TokenType::IDENTIFIER)) {
        error("期望样式模板名称");
        return nullptr;
    }
    
    Token nameToken = currentToken();
    std::string templateName = nameToken.value;
    advanceToken();
    
    auto templateNode = std::make_shared<BaseNode>(NodeType::TEMPLATE, templateName);
    templateNode->setAttribute("type", "style");
    templateNode->setPosition(styleToken.line, styleToken.column, styleToken.position);
    
    skipWhitespaceAndComments();
    
    if (match(TokenType::LEFT_BRACE)) {
        advanceToken(); // 跳过 '{'
        
        skipWhitespaceAndComments();
        
        // 解析样式模板内容
        while (!match(TokenType::RIGHT_BRACE) && currentTokenIndex < tokens.size()) {
            Token token = currentToken();
            
            if (token.type == TokenType::END_OF_FILE) {
                error("期望 '}' 但遇到文件结束");
                break;
            }
            
            if (token.type == TokenType::IDENTIFIER) {
                // 解析样式属性
                std::string propertyName = token.value;
                advanceToken();
                
                skipWhitespaceAndComments();
                
                if (match(TokenType::COLON) || match(TokenType::EQUAL)) {
                    advanceToken(); // 跳过 : 或 =
                    
                    skipWhitespaceAndComments();
                    
                    std::string propertyValue = parseStylePropertyValue();
                    templateNode->setAttribute(propertyName, propertyValue);
                } else {
                    error("期望 : 或 =");
                }
            } else if (token.type == TokenType::CLASS_SELECTOR) {
                parseClassSelector(templateNode);
            } else if (token.type == TokenType::ID_SELECTOR) {
                parseIdSelector(templateNode);
            } else if (token.type == TokenType::CONTEXT_REF) {
                parseContextReference(templateNode);
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
    
    // 存储模板定义
    templates[templateName] = templateNode;
    
    return templateNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseElementTemplate() {
    if (!match(TokenType::TEMPLATE_ELEMENT)) {
        error("期望 @Element");
        return nullptr;
    }
    
    Token elementToken = currentToken();
    advanceToken(); // 跳过 @Element
    
    skipWhitespaceAndComments();
    
    if (!match(TokenType::IDENTIFIER)) {
        error("期望元素模板名称");
        return nullptr;
    }
    
    Token nameToken = currentToken();
    std::string templateName = nameToken.value;
    advanceToken();
    
    auto templateNode = std::make_shared<BaseNode>(NodeType::TEMPLATE, templateName);
    templateNode->setAttribute("type", "element");
    templateNode->setPosition(elementToken.line, elementToken.column, elementToken.position);
    
    skipWhitespaceAndComments();
    
    if (match(TokenType::LEFT_BRACE)) {
        advanceToken(); // 跳过 '{'
        
        skipWhitespaceAndComments();
        
        // 解析元素模板内容
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
            } else if (token.type == TokenType::STYLE) {
                child = parseStyleBlock();
            } else if (token.type == TokenType::SCRIPT) {
                child = parseScriptBlock();
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

std::shared_ptr<BaseNode> CHTLParser::parseVariableTemplate() {
    if (!match(TokenType::TEMPLATE_VAR)) {
        error("期望 @Var");
        return nullptr;
    }
    
    Token varToken = currentToken();
    advanceToken(); // 跳过 @Var
    
    skipWhitespaceAndComments();
    
    if (!match(TokenType::IDENTIFIER)) {
        error("期望变量模板名称");
        return nullptr;
    }
    
    Token nameToken = currentToken();
    std::string templateName = nameToken.value;
    advanceToken();
    
    auto templateNode = std::make_shared<BaseNode>(NodeType::TEMPLATE, templateName);
    templateNode->setAttribute("type", "variable");
    templateNode->setPosition(varToken.line, varToken.column, varToken.position);
    
    skipWhitespaceAndComments();
    
    if (match(TokenType::LEFT_BRACE)) {
        advanceToken(); // 跳过 '{'
        
        skipWhitespaceAndComments();
        
        // 解析变量模板内容
        while (!match(TokenType::RIGHT_BRACE) && currentTokenIndex < tokens.size()) {
            Token token = currentToken();
            
            if (token.type == TokenType::END_OF_FILE) {
                error("期望 '}' 但遇到文件结束");
                break;
            }
            
            if (token.type == TokenType::IDENTIFIER) {
                // 解析变量定义
                std::string varName = token.value;
                advanceToken();
                
                skipWhitespaceAndComments();
                
                if (match(TokenType::COLON) || match(TokenType::EQUAL)) {
                    advanceToken(); // 跳过 : 或 =
                    
                    skipWhitespaceAndComments();
                    
                    std::string varValue = parseAttributeValue();
                    templateNode->setAttribute(varName, varValue);
                } else {
                    error("期望 : 或 =");
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
    
    // 检查自定义类型
    if (match(TokenType::TEMPLATE_STYLE)) {
        return parseCustomStyle();
    } else if (match(TokenType::TEMPLATE_ELEMENT)) {
        return parseCustomElement();
    } else if (match(TokenType::TEMPLATE_VAR)) {
        return parseCustomVariable();
    } else {
        error("期望 @Style, @Element 或 @Var");
        return nullptr;
    }
}

std::shared_ptr<BaseNode> CHTLParser::parseCustomStyle() {
    if (!match(TokenType::TEMPLATE_STYLE)) {
        error("期望 @Style");
        return nullptr;
    }
    
    Token styleToken = currentToken();
    advanceToken(); // 跳过 @Style
    
    skipWhitespaceAndComments();
    
    if (!match(TokenType::IDENTIFIER)) {
        error("期望自定义样式名称");
        return nullptr;
    }
    
    Token nameToken = currentToken();
    std::string customName = nameToken.value;
    advanceToken();
    
    auto customNode = std::make_shared<BaseNode>(NodeType::CUSTOM, customName);
    customNode->setAttribute("type", "style");
    customNode->setPosition(styleToken.line, styleToken.column, styleToken.position);
    
    skipWhitespaceAndComments();
    
    if (match(TokenType::LEFT_BRACE)) {
        advanceToken(); // 跳过 '{'
        
        skipWhitespaceAndComments();
        
        // 解析自定义样式内容
        while (!match(TokenType::RIGHT_BRACE) && currentTokenIndex < tokens.size()) {
            Token token = currentToken();
            
            if (token.type == TokenType::END_OF_FILE) {
                error("期望 '}' 但遇到文件结束");
                break;
            }
            
            if (token.type == TokenType::IDENTIFIER) {
                // 解析样式属性
                std::string propertyName = token.value;
                advanceToken();
                
                skipWhitespaceAndComments();
                
                if (match(TokenType::COLON) || match(TokenType::EQUAL)) {
                    advanceToken(); // 跳过 : 或 =
                    
                    skipWhitespaceAndComments();
                    
                    std::string propertyValue = parseStylePropertyValue();
                    customNode->setAttribute(propertyName, propertyValue);
                } else {
                    error("期望 : 或 =");
                }
            } else if (token.type == TokenType::CLASS_SELECTOR) {
                parseClassSelector(customNode);
            } else if (token.type == TokenType::ID_SELECTOR) {
                parseIdSelector(customNode);
            } else if (token.type == TokenType::CONTEXT_REF) {
                parseContextReference(customNode);
            } else if (token.type == TokenType::DELETE) {
                parseDeleteOperation(customNode);
            } else if (token.type == TokenType::INSERT) {
                parseInsertOperation(customNode);
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
    
    // 存储自定义定义
    customs[customName] = customNode;
    
    return customNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseCustomElement() {
    if (!match(TokenType::TEMPLATE_ELEMENT)) {
        error("期望 @Element");
        return nullptr;
    }
    
    Token elementToken = currentToken();
    advanceToken(); // 跳过 @Element
    
    skipWhitespaceAndComments();
    
    if (!match(TokenType::IDENTIFIER)) {
        error("期望自定义元素名称");
        return nullptr;
    }
    
    Token nameToken = currentToken();
    std::string customName = nameToken.value;
    advanceToken();
    
    auto customNode = std::make_shared<BaseNode>(NodeType::CUSTOM, customName);
    customNode->setAttribute("type", "element");
    customNode->setPosition(elementToken.line, elementToken.column, elementToken.position);
    
    skipWhitespaceAndComments();
    
    if (match(TokenType::LEFT_BRACE)) {
        advanceToken(); // 跳过 '{'
        
        skipWhitespaceAndComments();
        
        // 解析自定义元素内容
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
            } else if (token.type == TokenType::STYLE) {
                child = parseStyleBlock();
            } else if (token.type == TokenType::SCRIPT) {
                child = parseScriptBlock();
            } else if (token.type == TokenType::DELETE) {
                parseDeleteOperation(customNode);
            } else if (token.type == TokenType::INSERT) {
                parseInsertOperation(customNode);
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

std::shared_ptr<BaseNode> CHTLParser::parseCustomVariable() {
    if (!match(TokenType::TEMPLATE_VAR)) {
        error("期望 @Var");
        return nullptr;
    }
    
    Token varToken = currentToken();
    advanceToken(); // 跳过 @Var
    
    skipWhitespaceAndComments();
    
    if (!match(TokenType::IDENTIFIER)) {
        error("期望自定义变量名称");
        return nullptr;
    }
    
    Token nameToken = currentToken();
    std::string customName = nameToken.value;
    advanceToken();
    
    auto customNode = std::make_shared<BaseNode>(NodeType::CUSTOM, customName);
    customNode->setAttribute("type", "variable");
    customNode->setPosition(varToken.line, varToken.column, varToken.position);
    
    skipWhitespaceAndComments();
    
    if (match(TokenType::LEFT_BRACE)) {
        advanceToken(); // 跳过 '{'
        
        skipWhitespaceAndComments();
        
        // 解析自定义变量内容
        while (!match(TokenType::RIGHT_BRACE) && currentTokenIndex < tokens.size()) {
            Token token = currentToken();
            
            if (token.type == TokenType::END_OF_FILE) {
                error("期望 '}' 但遇到文件结束");
                break;
            }
            
            if (token.type == TokenType::IDENTIFIER) {
                // 解析变量定义
                std::string varName = token.value;
                advanceToken();
                
                skipWhitespaceAndComments();
                
                if (match(TokenType::COLON) || match(TokenType::EQUAL)) {
                    advanceToken(); // 跳过 : 或 =
                    
                    skipWhitespaceAndComments();
                    
                    std::string varValue = parseAttributeValue();
                    customNode->setAttribute(varName, varValue);
                } else {
                    error("期望 : 或 =");
                }
            } else if (token.type == TokenType::DELETE) {
                parseDeleteOperation(customNode);
            } else if (token.type == TokenType::INSERT) {
                parseInsertOperation(customNode);
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
    
    // 存储自定义定义
    customs[customName] = customNode;
    
    return customNode;
}

void CHTLParser::parseDeleteOperation(std::shared_ptr<BaseNode> parent) {
    Token deleteToken = currentToken();
    advanceToken(); // 跳过 delete
    
    skipWhitespaceAndComments();
    
    if (!match(TokenType::IDENTIFIER)) {
        error("期望要删除的属性或元素名称");
        return;
    }
    
    Token nameToken = currentToken();
    std::string deleteName = nameToken.value;
    advanceToken();
    
    // 创建删除操作节点
    auto deleteNode = std::make_shared<BaseNode>(NodeType::ELEMENT, "delete");
    deleteNode->setAttribute("target", deleteName);
    deleteNode->setPosition(deleteToken.line, deleteToken.column, deleteToken.position);
    
    parent->addChild(deleteNode);
}

void CHTLParser::parseInsertOperation(std::shared_ptr<BaseNode> parent) {
    Token insertToken = currentToken();
    advanceToken(); // 跳过 insert
    
    skipWhitespaceAndComments();
    
    if (!match(TokenType::IDENTIFIER)) {
        error("期望要插入的元素名称");
        return;
    }
    
    Token nameToken = currentToken();
    std::string insertName = nameToken.value;
    advanceToken();
    
    // 创建插入操作节点
    auto insertNode = std::make_shared<BaseNode>(NodeType::ELEMENT, "insert");
    insertNode->setAttribute("target", insertName);
    insertNode->setPosition(insertToken.line, insertToken.column, insertToken.position);
    
    skipWhitespaceAndComments();
    
    if (match(TokenType::LEFT_BRACE)) {
        advanceToken(); // 跳过 '{'
        
        skipWhitespaceAndComments();
        
        // 解析插入内容
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
                insertNode->addChild(child);
            }
            
            skipWhitespaceAndComments();
        }
        
        if (match(TokenType::RIGHT_BRACE)) {
            advanceToken(); // 跳过 '}'
        } else {
            error("期望 '}'");
        }
    }
    
    parent->addChild(insertNode);
}

std::shared_ptr<BaseNode> CHTLParser::parseOrigin() {
    if (!match(TokenType::ORIGIN)) {
        error("期望 [Origin]");
        return nullptr;
    }
    
    advanceToken(); // 跳过 [Origin]
    
    skipWhitespaceAndComments();
    
    // 检查原始嵌入类型
    if (match(TokenType::TEMPLATE_STYLE)) {
        return parseOriginStyle();
    } else if (match(TokenType::TEMPLATE_ELEMENT)) {
        return parseOriginElement();
    } else if (match(TokenType::TEMPLATE_VAR)) {
        return parseOriginJavaScript();
    } else {
        error("期望 @Style, @Element 或 @Var");
        return nullptr;
    }
}

std::shared_ptr<BaseNode> CHTLParser::parseOriginStyle() {
    if (!match(TokenType::TEMPLATE_STYLE)) {
        error("期望 @Style");
        return nullptr;
    }
    
    Token styleToken = currentToken();
    advanceToken(); // 跳过 @Style
    
    skipWhitespaceAndComments();
    
    std::string originName;
    if (match(TokenType::IDENTIFIER)) {
        Token nameToken = currentToken();
        originName = nameToken.value;
        advanceToken();
    }
    
    auto originNode = std::make_shared<BaseNode>(NodeType::ORIGIN, originName);
    originNode->setAttribute("type", "style");
    originNode->setPosition(styleToken.line, styleToken.column, styleToken.position);
    
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

std::shared_ptr<BaseNode> CHTLParser::parseOriginElement() {
    if (!match(TokenType::TEMPLATE_ELEMENT)) {
        error("期望 @Element");
        return nullptr;
    }
    
    Token elementToken = currentToken();
    advanceToken(); // 跳过 @Element
    
    skipWhitespaceAndComments();
    
    std::string originName;
    if (match(TokenType::IDENTIFIER)) {
        Token nameToken = currentToken();
        originName = nameToken.value;
        advanceToken();
    }
    
    auto originNode = std::make_shared<BaseNode>(NodeType::ORIGIN, originName);
    originNode->setAttribute("type", "element");
    originNode->setPosition(elementToken.line, elementToken.column, elementToken.position);
    
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

std::shared_ptr<BaseNode> CHTLParser::parseOriginJavaScript() {
    if (!match(TokenType::TEMPLATE_VAR)) {
        error("期望 @Var");
        return nullptr;
    }
    
    Token jsToken = currentToken();
    advanceToken(); // 跳过 @Var
    
    skipWhitespaceAndComments();
    
    std::string originName;
    if (match(TokenType::IDENTIFIER)) {
        Token nameToken = currentToken();
        originName = nameToken.value;
        advanceToken();
    }
    
    auto originNode = std::make_shared<BaseNode>(NodeType::ORIGIN, originName);
    originNode->setAttribute("type", "javascript");
    originNode->setPosition(jsToken.line, jsToken.column, jsToken.position);
    
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
    
    // 检查导入类型
    if (match(TokenType::TEMPLATE_STYLE)) {
        return parseImportStyle();
    } else if (match(TokenType::TEMPLATE_ELEMENT)) {
        return parseImportElement();
    } else if (match(TokenType::TEMPLATE_VAR)) {
        return parseImportJavaScript();
    } else if (match(TokenType::IDENTIFIER)) {
        // 检查是否为CHTL导入
        Token typeToken = currentToken();
        if (typeToken.value == "CHTL") {
            return parseImportCHTL();
        } else {
            error("期望 @Style, @Element, @Var 或 CHTL");
            return nullptr;
        }
    } else {
        error("期望 @Style, @Element, @Var 或 CHTL");
        return nullptr;
    }
}

std::shared_ptr<BaseNode> CHTLParser::parseImportStyle() {
    if (!match(TokenType::TEMPLATE_STYLE)) {
        error("期望 @Style");
        return nullptr;
    }
    
    Token styleToken = currentToken();
    advanceToken(); // 跳过 @Style
    
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
    importNode->setAttribute("type", "style");
    importNode->setAttribute("path", filePath);
    importNode->setPosition(styleToken.line, styleToken.column, styleToken.position);
    
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

std::shared_ptr<BaseNode> CHTLParser::parseImportElement() {
    if (!match(TokenType::TEMPLATE_ELEMENT)) {
        error("期望 @Element");
        return nullptr;
    }
    
    Token elementToken = currentToken();
    advanceToken(); // 跳过 @Element
    
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
    importNode->setAttribute("type", "element");
    importNode->setAttribute("path", filePath);
    importNode->setPosition(elementToken.line, elementToken.column, elementToken.position);
    
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

std::shared_ptr<BaseNode> CHTLParser::parseImportJavaScript() {
    if (!match(TokenType::TEMPLATE_VAR)) {
        error("期望 @Var");
        return nullptr;
    }
    
    Token jsToken = currentToken();
    advanceToken(); // 跳过 @Var
    
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
    importNode->setAttribute("type", "javascript");
    importNode->setAttribute("path", filePath);
    importNode->setPosition(jsToken.line, jsToken.column, jsToken.position);
    
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

std::shared_ptr<BaseNode> CHTLParser::parseImportCHTL() {
    if (!match(TokenType::IDENTIFIER)) {
        error("期望 CHTL");
        return nullptr;
    }
    
    Token chtlToken = currentToken();
    if (chtlToken.value != "CHTL") {
        error("期望 CHTL");
        return nullptr;
    }
    
    advanceToken(); // 跳过 CHTL
    
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
    importNode->setAttribute("type", "chtl");
    importNode->setAttribute("path", filePath);
    importNode->setPosition(chtlToken.line, chtlToken.column, chtlToken.position);
    
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
            
            // 检查是否有属性值（支持CE对等式：:和=等价）
            if (match(TokenType::COLON) || match(TokenType::EQUAL) || match(TokenType::COLON_EQUAL)) {
                if (match(TokenType::COLON_EQUAL)) {
                    advanceToken(); // 跳过 :=
                } else {
                    advanceToken(); // 跳过 : 或 =
                }
                
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
    } else if (match(TokenType::UNQUOTED_LITERAL)) {
        value = currentToken().value;
        advanceToken();
    } else if (match(TokenType::SINGLE_QUOTED_LITERAL)) {
        value = currentToken().value;
        advanceToken();
    } else if (match(TokenType::DOUBLE_QUOTED_LITERAL)) {
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
        
        // 处理类选择器
        if (token.type == TokenType::CLASS_SELECTOR) {
            parseClassSelector(styleNode);
        }
        // 处理ID选择器
        else if (token.type == TokenType::ID_SELECTOR) {
            parseIdSelector(styleNode);
        }
        // 处理上下文引用
        else if (token.type == TokenType::CONTEXT_REF) {
            parseContextReference(styleNode);
        }
        // 处理普通属性
        else if (token.type == TokenType::IDENTIFIER) {
            std::string propertyName = token.value;
            advanceToken();
            
            skipWhitespaceAndComments();
            
            if (match(TokenType::COLON) || match(TokenType::EQUAL)) {
                advanceToken(); // 跳过 : 或 =
                
                skipWhitespaceAndComments();
                
                std::string propertyValue = parseStylePropertyValue();
                styleNode->setAttribute(propertyName, propertyValue);
            } else {
                error("期望 : 或 =");
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

// 局部样式块解析方法
void CHTLParser::parseClassSelector(std::shared_ptr<BaseNode> styleNode) {
    Token classToken = currentToken();
    std::string className = classToken.value;
    advanceToken();
    
    skipWhitespaceAndComments();
    
    if (match(TokenType::LEFT_BRACE)) {
        advanceToken(); // 跳过 {
        
        auto classRule = std::make_shared<BaseNode>(NodeType::ELEMENT, "class_rule");
        classRule->setAttribute("type", "class");
        classRule->setAttribute("selector", "." + className);
        
        // 解析类选择器内的属性
        while (!match(TokenType::RIGHT_BRACE) && currentTokenIndex < tokens.size()) {
            Token token = currentToken();
            
            if (token.type == TokenType::END_OF_FILE) {
                error("期望 '}' 但遇到文件结束");
                break;
            }
            
            if (token.type == TokenType::IDENTIFIER) {
                std::string propertyName = token.value;
                advanceToken();
                
                skipWhitespaceAndComments();
                
                if (match(TokenType::COLON) || match(TokenType::EQUAL)) {
                    advanceToken(); // 跳过 : 或 =
                    
                    skipWhitespaceAndComments();
                    
                    std::string propertyValue = parseStylePropertyValue();
                    classRule->setAttribute(propertyName, propertyValue);
                } else {
                    error("期望 : 或 =");
                }
            }
            
            skipWhitespaceAndComments();
            
            if (match(TokenType::SEMICOLON)) {
                advanceToken(); // 跳过 ;
            }
            
            skipWhitespaceAndComments();
        }
        
        if (match(TokenType::RIGHT_BRACE)) {
            advanceToken(); // 跳过 }
        } else {
            error("期望 '}'");
        }
        
        styleNode->addChild(classRule);
    }
}

void CHTLParser::parseIdSelector(std::shared_ptr<BaseNode> styleNode) {
    Token idToken = currentToken();
    std::string idName = idToken.value;
    advanceToken();
    
    skipWhitespaceAndComments();
    
    if (match(TokenType::LEFT_BRACE)) {
        advanceToken(); // 跳过 {
        
        auto idRule = std::make_shared<BaseNode>(NodeType::ELEMENT, "id_rule");
        idRule->setAttribute("type", "id");
        idRule->setAttribute("selector", "#" + idName);
        
        // 解析ID选择器内的属性
        while (!match(TokenType::RIGHT_BRACE) && currentTokenIndex < tokens.size()) {
            Token token = currentToken();
            
            if (token.type == TokenType::END_OF_FILE) {
                error("期望 '}' 但遇到文件结束");
                break;
            }
            
            if (token.type == TokenType::IDENTIFIER) {
                std::string propertyName = token.value;
                advanceToken();
                
                skipWhitespaceAndComments();
                
                if (match(TokenType::COLON) || match(TokenType::EQUAL)) {
                    advanceToken(); // 跳过 : 或 =
                    
                    skipWhitespaceAndComments();
                    
                    std::string propertyValue = parseStylePropertyValue();
                    idRule->setAttribute(propertyName, propertyValue);
                } else {
                    error("期望 : 或 =");
                }
            }
            
            skipWhitespaceAndComments();
            
            if (match(TokenType::SEMICOLON)) {
                advanceToken(); // 跳过 ;
            }
            
            skipWhitespaceAndComments();
        }
        
        if (match(TokenType::RIGHT_BRACE)) {
            advanceToken(); // 跳过 }
        } else {
            error("期望 '}'");
        }
        
        styleNode->addChild(idRule);
    }
}

void CHTLParser::parseContextReference(std::shared_ptr<BaseNode> styleNode) {
    Token contextToken = currentToken();
    advanceToken();
    
    skipWhitespaceAndComments();
    
    // 检查是否为伪类或伪元素
    if (match(TokenType::COLON)) {
        advanceToken(); // 跳过 :
        
        std::string pseudoName;
        while (currentTokenIndex < tokens.size() && 
               (isAlphaNumeric(currentToken().value[0]) || currentToken().value[0] == '-')) {
            pseudoName += currentToken().value;
            advanceToken();
        }
        
        if (match(TokenType::LEFT_BRACE)) {
            advanceToken(); // 跳过 {
            
            auto pseudoRule = std::make_shared<BaseNode>(NodeType::ELEMENT, "pseudo_rule");
            pseudoRule->setAttribute("type", "pseudo");
            pseudoRule->setAttribute("selector", "&:" + pseudoName);
            
            // 解析伪类/伪元素内的属性
            while (!match(TokenType::RIGHT_BRACE) && currentTokenIndex < tokens.size()) {
                Token token = currentToken();
                
                if (token.type == TokenType::END_OF_FILE) {
                    error("期望 '}' 但遇到文件结束");
                    break;
                }
                
                if (token.type == TokenType::IDENTIFIER) {
                    std::string propertyName = token.value;
                    advanceToken();
                    
                    skipWhitespaceAndComments();
                    
                    if (match(TokenType::COLON) || match(TokenType::EQUAL)) {
                        advanceToken(); // 跳过 : 或 =
                        
                        skipWhitespaceAndComments();
                        
                        std::string propertyValue = parseStylePropertyValue();
                        pseudoRule->setAttribute(propertyName, propertyValue);
                    } else {
                        error("期望 : 或 =");
                    }
                }
                
                skipWhitespaceAndComments();
                
                if (match(TokenType::SEMICOLON)) {
                    advanceToken(); // 跳过 ;
                }
                
                skipWhitespaceAndComments();
            }
            
            if (match(TokenType::RIGHT_BRACE)) {
                advanceToken(); // 跳过 }
            } else {
                error("期望 '}'");
            }
            
            styleNode->addChild(pseudoRule);
        }
    }
}

std::string CHTLParser::parseStylePropertyValue() {
    std::string value;
    
    // 解析属性值，支持表达式
    while (currentTokenIndex < tokens.size()) {
        Token token = currentToken();
        
        if (token.type == TokenType::SEMICOLON || 
            token.type == TokenType::RIGHT_BRACE ||
            token.type == TokenType::COMMA) {
            break;
        }
        
        // 处理属性引用
        if (token.type == TokenType::PROPERTY_REF) {
            value += token.value;
            advanceToken();
        }
        // 处理条件表达式
        else if (token.type == TokenType::CONDITIONAL_EXPR) {
            value += token.value;
            advanceToken();
        }
        // 处理算术运算符
        else if (token.type == TokenType::PLUS || token.type == TokenType::MINUS ||
                 token.type == TokenType::ASTERISK || token.type == TokenType::SLASH ||
                 token.type == TokenType::PERCENT || token.type == TokenType::POWER) {
            value += token.value;
            advanceToken();
        }
        // 处理比较运算符
        else if (token.type == TokenType::EQUAL_EQUAL || token.type == TokenType::NOT_EQUAL ||
                 token.type == TokenType::LESS || token.type == TokenType::GREATER ||
                 token.type == TokenType::LESS_EQUAL || token.type == TokenType::GREATER_EQUAL) {
            value += token.value;
            advanceToken();
        }
        // 处理逻辑运算符
        else if (token.type == TokenType::AND || token.type == TokenType::OR) {
            value += token.value;
            advanceToken();
        }
        // 处理其他值
        else {
            value += token.value;
            advanceToken();
        }
        
        skipWhitespaceAndComments();
    }
    
    return value;
}

bool CHTLParser::isAlphaNumeric(char c) const {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
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