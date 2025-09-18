#include "CHTLParser.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
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
    // 解析模板的简化实现
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseCustom() {
    // 解析自定义的简化实现
    return nullptr;
}

std::shared_ptr<BaseNode> CHTLParser::parseStyle() {
    // 解析样式块的简化实现
    return nullptr;
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