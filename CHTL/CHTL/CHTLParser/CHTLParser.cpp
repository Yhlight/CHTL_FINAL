#include "CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/TemplateNode.h"
#include "../CHTLNode/CustomNode.h"
#include <iostream>
#include <algorithm>

namespace CHTL {

CHTLParser::CHTLParser() : m_position(0) {
}

CHTLParser::~CHTLParser() {
}

std::shared_ptr<BaseNode> CHTLParser::parse(const std::vector<Token>& tokens) {
    m_tokens = tokens;
    m_position = 0;
    while (!m_nodeStack.empty()) {
        m_nodeStack.pop();
    }
    
    // 创建根节点
    auto root = std::make_shared<ElementNode>("root");
    m_nodeStack.push(root);
    
    while (!isAtEnd()) {
        skipWhitespace();
        if (isAtEnd()) break;
        
        const Token& token = current();
        
        switch (token.type) {
            case TokenType::IDENTIFIER:
                // 解析元素
                {
                    auto element = parseElement();
                    if (element) {
                        m_nodeStack.top()->addChild(element);
                    }
                }
                break;
                
            case TokenType::TEXT:
                // 解析文本节点
                {
                    auto textNode = parseText();
                    if (textNode) {
                        m_nodeStack.top()->addChild(textNode);
                    }
                }
                break;
                
            case TokenType::GENERATOR_COMMENT:
                // 解析注释
                {
                    auto comment = parseComment();
                    if (comment) {
                        m_nodeStack.top()->addChild(comment);
                    }
                }
                break;
                
            case TokenType::TEMPLATE:
                // 解析模板
                {
                    auto templateNode = parseTemplate();
                    if (templateNode) {
                        m_nodeStack.top()->addChild(templateNode);
                    }
                }
                break;
                
            case TokenType::CUSTOM:
                // 解析自定义
                {
                    auto customNode = parseCustom();
                    if (customNode) {
                        m_nodeStack.top()->addChild(customNode);
                    }
                }
                break;
                
            case TokenType::ORIGIN:
                // 解析原始嵌入
                {
                    auto originNode = parseOrigin();
                    if (originNode) {
                        m_nodeStack.top()->addChild(originNode);
                    }
                }
                break;
                
            case TokenType::IMPORT:
                // 解析导入
                {
                    auto importNode = parseImport();
                    if (importNode) {
                        m_nodeStack.top()->addChild(importNode);
                    }
                }
                break;
                
            case TokenType::CONFIGURATION:
                // 解析配置
                {
                    auto configNode = parseConfig();
                    if (configNode) {
                        m_nodeStack.top()->addChild(configNode);
                    }
                }
                break;
                
            case TokenType::NAMESPACE:
                // 解析命名空间
                {
                    auto namespaceNode = parseNamespace();
                    if (namespaceNode) {
                        m_nodeStack.top()->addChild(namespaceNode);
                    }
                }
                break;
                
            default:
                std::cerr << "Unexpected token: " << token.value << " at line " << token.line << std::endl;
                advance();
                break;
        }
    }
    
    return root;
}

std::shared_ptr<BaseNode> CHTLParser::parse(const std::string& code) {
    CHTLLexer lexer;
    auto tokens = lexer.tokenize(code);
    return parse(tokens);
}

std::shared_ptr<BaseNode> CHTLParser::parseElement() {
    if (!match(TokenType::IDENTIFIER)) {
        return nullptr;
    }
    
    const Token& token = current();
    std::string tagName = token.value;
    advance();
    
    auto element = std::make_shared<ElementNode>(tagName);
    element->setPosition(token.line, token.column);
    
    // 解析属性
    parseAttributes(element);
    
    // 检查是否有子内容
    if (consume(TokenType::LEFT_BRACE)) {
        m_nodeStack.push(element);
        
        // 解析子节点
        while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
            skipWhitespace();
            if (isAtEnd()) break;
            
            const Token& childToken = current();
            
            if (childToken.type == TokenType::IDENTIFIER) {
                // 子元素
                auto childElement = parseElement();
                if (childElement) {
                    element->addChild(childElement);
                }
            } else if (childToken.type == TokenType::TEXT) {
                // 文本节点
                auto textNode = parseText();
                if (textNode) {
                    element->addChild(textNode);
                }
            } else if (childToken.type == TokenType::STYLE) {
                // 样式块
                auto styleNode = parseStyle();
                if (styleNode) {
                    element->addChild(styleNode);
                }
            } else if (childToken.type == TokenType::SCRIPT) {
                // 脚本块
                auto scriptNode = parseScript();
                if (scriptNode) {
                    element->addChild(scriptNode);
                }
            } else {
                advance();
            }
        }
        
        m_nodeStack.pop();
        consume(TokenType::RIGHT_BRACE);
    }
    
    return element;
}

std::shared_ptr<BaseNode> CHTLParser::parseText() {
    if (!match(TokenType::TEXT)) {
        return nullptr;
    }
    
    const Token& token = current();
    auto textNode = std::make_shared<TextNode>(token.value);
    textNode->setPosition(token.line, token.column);
    advance();
    
    return textNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseComment() {
    if (!match(TokenType::GENERATOR_COMMENT)) {
        return nullptr;
    }
    
    const Token& token = current();
    auto commentNode = std::make_shared<TextNode>(token.value);
    commentNode->setPosition(token.line, token.column);
    advance();
    
    return commentNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseTemplate() {
    if (!consume(TokenType::TEMPLATE)) {
        return nullptr;
    }
    
    // 解析模板类型
    if (!match(TokenType::AT_STYLE) && !match(TokenType::AT_ELEMENT) && !match(TokenType::AT_VAR)) {
        return nullptr;
    }
    
    TemplateNode::TemplateType templateType;
    if (consume(TokenType::AT_STYLE)) {
        templateType = TemplateNode::TemplateType::STYLE;
    } else if (consume(TokenType::AT_ELEMENT)) {
        templateType = TemplateNode::TemplateType::ELEMENT;
    } else if (consume(TokenType::AT_VAR)) {
        templateType = TemplateNode::TemplateType::VAR;
    }
    
    // 解析模板名称
    if (!match(TokenType::IDENTIFIER)) {
        return nullptr;
    }
    
    std::string templateName = current().value;
    advance();
    
    auto templateNode = std::make_shared<TemplateNode>(templateType, templateName);
    templateNode->setPosition(current().line, current().column);
    
    // 解析模板内容
    if (consume(TokenType::LEFT_BRACE)) {
        while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
            skipWhitespace();
            if (isAtEnd()) break;
            
            const Token& token = current();
            
            if (token.type == TokenType::IDENTIFIER) {
                // 子元素
                auto childElement = parseElement();
                if (childElement) {
                    templateNode->addChild(childElement);
                }
            } else if (token.type == TokenType::TEXT) {
                // 文本节点
                auto textNode = parseText();
                if (textNode) {
                    templateNode->addChild(textNode);
                }
            } else {
                advance();
            }
        }
        
        consume(TokenType::RIGHT_BRACE);
    }
    
    return templateNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseCustom() {
    if (!consume(TokenType::CUSTOM)) {
        return nullptr;
    }
    
    // 解析自定义类型
    if (!match(TokenType::AT_STYLE) && !match(TokenType::AT_ELEMENT) && !match(TokenType::AT_VAR)) {
        return nullptr;
    }
    
    CustomNode::CustomType customType;
    if (consume(TokenType::AT_STYLE)) {
        customType = CustomNode::CustomType::STYLE;
    } else if (consume(TokenType::AT_ELEMENT)) {
        customType = CustomNode::CustomType::ELEMENT;
    } else if (consume(TokenType::AT_VAR)) {
        customType = CustomNode::CustomType::VAR;
    }
    
    // 解析自定义名称
    if (!match(TokenType::IDENTIFIER)) {
        return nullptr;
    }
    
    std::string customName = current().value;
    advance();
    
    auto customNode = std::make_shared<CustomNode>(customType, customName);
    customNode->setPosition(current().line, current().column);
    
    // 解析自定义内容
    if (consume(TokenType::LEFT_BRACE)) {
        while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
            skipWhitespace();
            if (isAtEnd()) break;
            
            const Token& token = current();
            
            if (token.type == TokenType::IDENTIFIER) {
                // 子元素
                auto childElement = parseElement();
                if (childElement) {
                    customNode->addChild(childElement);
                }
            } else if (token.type == TokenType::TEXT) {
                // 文本节点
                auto textNode = parseText();
                if (textNode) {
                    customNode->addChild(textNode);
                }
            } else {
                advance();
            }
        }
        
        consume(TokenType::RIGHT_BRACE);
    }
    
    return customNode;
}

std::shared_ptr<BaseNode> CHTLParser::parseStyle() {
    if (!consume(TokenType::STYLE)) {
        return nullptr;
    }
    
    auto styleNode = std::make_shared<StyleNode>();
    styleNode->setPosition(current().line, current().column);
    
    if (consume(TokenType::LEFT_BRACE)) {
        // 解析样式内容
        while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
            skipWhitespace();
            if (isAtEnd()) break;
            
            const Token& token = current();
            
            if (token.type == TokenType::IDENTIFIER) {
                // 解析样式属性
                std::string property = token.value;
                advance();
                
                if (consume(TokenType::COLON)) {
                    std::string value = parseAttributeValue();
                    styleNode->addStyleProperty(property, value);
                }
            } else if (token.value == ".") {
                // 类选择器
                advance();
                if (match(TokenType::IDENTIFIER)) {
                    std::string className = current().value;
                    advance();
                    styleNode->setStyleType(StyleNode::StyleType::CLASS);
                    styleNode->setSelector("." + className);
                    
                    // 解析类选择器内容
                    if (consume(TokenType::LEFT_BRACE)) {
                        parseStyleProperties(styleNode);
                        consume(TokenType::RIGHT_BRACE);
                    }
                }
            } else if (token.value == "#") {
                // ID选择器
                advance();
                if (match(TokenType::IDENTIFIER)) {
                    std::string idName = current().value;
                    advance();
                    styleNode->setStyleType(StyleNode::StyleType::ID);
                    styleNode->setSelector("#" + idName);
                    
                    // 解析ID选择器内容
                    if (consume(TokenType::LEFT_BRACE)) {
                        parseStyleProperties(styleNode);
                        consume(TokenType::RIGHT_BRACE);
                    }
                }
            } else if (token.value == "&") {
                // 上下文推导
                advance();
                if (match(TokenType::COLON)) {
                    std::string pseudo = token.value;
                    advance();
                    if (match(TokenType::IDENTIFIER)) {
                        pseudo += current().value;
                        advance();
                        styleNode->setStyleType(StyleNode::StyleType::PSEUDO_CLASS);
                        styleNode->setSelector("&" + pseudo);
                        
                        // 解析伪类内容
                        if (consume(TokenType::LEFT_BRACE)) {
                            parseStyleProperties(styleNode);
                            consume(TokenType::RIGHT_BRACE);
                        }
                    }
                }
            } else {
                advance();
            }
        }
        
        consume(TokenType::RIGHT_BRACE);
    }
    
    return styleNode;
}

void CHTLParser::parseStyleProperties(std::shared_ptr<BaseNode> styleNode) {
    while (!isAtEnd() && !match(TokenType::RIGHT_BRACE)) {
        skipWhitespace();
        if (isAtEnd()) break;
        
        const Token& token = current();
        
        if (token.type == TokenType::IDENTIFIER) {
            std::string property = token.value;
            advance();
            
            if (consume(TokenType::COLON)) {
                std::string value = parseAttributeValue();
                auto styleNodePtr = std::static_pointer_cast<StyleNode>(styleNode);
                styleNodePtr->addStyleProperty(property, value);
            }
        } else {
            advance();
        }
    }
}

std::shared_ptr<BaseNode> CHTLParser::parseScript() {
    // 解析脚本块的简化实现
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseOrigin() {
    // 解析原始嵌入的简化实现
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseImport() {
    // 解析导入的简化实现
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseConfig() {
    // 解析配置的简化实现
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseNamespace() {
    // 解析命名空间的简化实现
    return nullptr;
}

void CHTLParser::parseAttributes(std::shared_ptr<BaseNode> element) {
    while (!isAtEnd() && !match(TokenType::LEFT_BRACE) && !match(TokenType::RIGHT_BRACE)) {
        skipWhitespace();
        if (isAtEnd()) break;
        
        const Token& token = current();
        
        if (token.type == TokenType::IDENTIFIER) {
            std::string attrName = token.value;
            advance();
            
            if (consume(TokenType::COLON) || consume(TokenType::EQUALS)) {
                std::string attrValue = parseAttributeValue();
                element->setAttribute(attrName, attrValue);
            }
        } else {
            advance();
        }
    }
}

std::string CHTLParser::parseAttributeValue() {
    if (match(TokenType::STRING)) {
        const Token& token = current();
        advance();
        return token.value;
    } else if (match(TokenType::LITERAL)) {
        const Token& token = current();
        advance();
        return token.value;
    } else if (match(TokenType::NUMBER)) {
        const Token& token = current();
        advance();
        return token.value;
    } else if (match(TokenType::IDENTIFIER)) {
        const Token& token = current();
        advance();
        return token.value;
    } else {
        return "";
    }
}

std::string CHTLParser::parseLiteral() {
    if (match(TokenType::LITERAL)) {
        const Token& token = current();
        advance();
        return token.value;
    }
    return "";
}

std::string CHTLParser::parseString() {
    if (match(TokenType::STRING)) {
        const Token& token = current();
        advance();
        return token.value;
    }
    return "";
}

std::string CHTLParser::parseExpression() {
    // 解析表达式的简化实现
    return "";
}

const Token& CHTLParser::current() const {
    if (m_position >= m_tokens.size()) {
        static const Token eof(TokenType::END_OF_FILE, "", 0, 0, 0);
        return eof;
    }
    return m_tokens[m_position];
}

const Token& CHTLParser::peek() const {
    if (m_position + 1 >= m_tokens.size()) {
        static const Token eof(TokenType::END_OF_FILE, "", 0, 0, 0);
        return eof;
    }
    return m_tokens[m_position + 1];
}

void CHTLParser::advance() {
    if (m_position < m_tokens.size()) {
        m_position++;
    }
}

bool CHTLParser::isAtEnd() const {
    return m_position >= m_tokens.size() || current().type == TokenType::END_OF_FILE;
}

bool CHTLParser::match(TokenType type) const {
    return current().type == type;
}

bool CHTLParser::consume(TokenType type) {
    if (match(type)) {
        advance();
        return true;
    }
    return false;
}

void CHTLParser::skipWhitespace() {
    while (!isAtEnd() && (current().type == TokenType::UNKNOWN || 
                         current().value.empty() || 
                         std::all_of(current().value.begin(), current().value.end(), ::isspace))) {
        advance();
    }
}

} // namespace CHTL