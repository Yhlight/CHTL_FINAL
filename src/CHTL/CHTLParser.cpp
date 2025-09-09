#include "CHTLParser.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/StyleNode.h"
#include "CHTLNode/ScriptNode.h"
#include <iostream>

namespace CHTL {

CHTLParser::CHTLParser() {}

std::unique_ptr<BaseNode> CHTLParser::parse(std::unique_ptr<TokenStream> tokens) {
    m_tokens = std::move(tokens);
    clearErrors();
    
    if (!m_tokens || m_tokens->empty()) {
        addError("没有令牌可解析");
        return nullptr;
    }
    
    try {
        return parseRoot();
    } catch (const std::exception& e) {
        addError("解析过程中发生异常: " + std::string(e.what()));
        return nullptr;
    }
}

std::unique_ptr<BaseNode> CHTLParser::parseRoot() {
    auto root = std::make_unique<BaseNode>(NodeType::ROOT);
    
    while (!isAtEnd()) {
        auto node = parseElement();
        if (node) {
            root->addChild(std::move(node));
        } else {
            break;
        }
    }
    
    return root;
}

std::unique_ptr<BaseNode> CHTLParser::parseElement() {
    Token* token = peek();
    if (!token) {
        return nullptr;
    }
    
    // 检查是否为HTML元素
    if (token->isType(TokenType::IDENTIFIER)) {
        std::string tagName = token->getValue();
        consume(TokenType::IDENTIFIER);
        
        auto element = std::make_unique<ElementNode>(tagName);
        
        // 解析属性
        while (peek() && !peek()->isType(TokenType::LEFT_BRACE)) {
            if (peek()->isType(TokenType::IDENTIFIER)) {
                std::string attrName = peek()->getValue();
                consume(TokenType::IDENTIFIER);
                
                if (peek() && (peek()->isType(TokenType::COLON) || peek()->isType(TokenType::EQUALS))) {
                    consume({TokenType::COLON, TokenType::EQUALS});
                    
                    std::string attrValue;
                    if (peek() && (peek()->isType(TokenType::STRING) || peek()->isType(TokenType::LITERAL))) {
                        attrValue = peek()->getValue();
                        consume({TokenType::STRING, TokenType::LITERAL});
                    }
                    
                    element->addAttribute(attrName, attrValue);
                }
            } else {
                consume();
            }
        }
        
        // 解析元素内容
        if (peek() && peek()->isType(TokenType::LEFT_BRACE)) {
            consume(TokenType::LEFT_BRACE);
            
            while (peek() && !peek()->isType(TokenType::RIGHT_BRACE)) {
                auto child = parseElement();
                if (child) {
                    element->addChild(std::move(child));
                } else {
                    break;
                }
            }
            
            if (peek() && peek()->isType(TokenType::RIGHT_BRACE)) {
                consume(TokenType::RIGHT_BRACE);
            }
        }
        
        return element;
    }
    
    // 检查是否为text节点
    if (token->isType(TokenType::TEXT)) {
        return parseText();
    }
    
    // 检查是否为style节点
    if (token->isType(TokenType::STYLE)) {
        return parseStyle();
    }
    
    // 检查是否为script节点
    if (token->isType(TokenType::SCRIPT)) {
        return parseScript();
    }
    
    // 检查是否为template节点
    if (token->isType(TokenType::TEMPLATE)) {
        return parseTemplate();
    }
    
    // 检查是否为custom节点
    if (token->isType(TokenType::CUSTOM)) {
        return parseCustom();
    }
    
    // 检查是否为origin节点
    if (token->isType(TokenType::ORIGIN)) {
        return parseOrigin();
    }
    
    // 检查是否为import节点
    if (token->isType(TokenType::IMPORT)) {
        return parseImport();
    }
    
    // 检查是否为configuration节点
    if (token->isType(TokenType::CONFIGURATION)) {
        return parseConfiguration();
    }
    
    // 检查是否为namespace节点
    if (token->isType(TokenType::NAMESPACE)) {
        return parseNamespace();
    }
    
    // 跳过未知令牌
    consume();
    return nullptr;
}

std::unique_ptr<BaseNode> CHTLParser::parseText() {
    if (!match(TokenType::TEXT)) {
        return nullptr;
    }
    
    consume(TokenType::TEXT);
    
    if (!match(TokenType::LEFT_BRACE)) {
        addError("期望 '{' 在 text 之后");
        return nullptr;
    }
    
    consume(TokenType::LEFT_BRACE);
    
    std::string content;
    while (peek() && !peek()->isType(TokenType::RIGHT_BRACE)) {
        if (peek()->isType(TokenType::STRING) || peek()->isType(TokenType::LITERAL)) {
            content += peek()->getValue();
            consume({TokenType::STRING, TokenType::LITERAL});
        } else {
            consume();
        }
    }
    
    if (!match(TokenType::RIGHT_BRACE)) {
        addError("期望 '}' 在 text 内容之后");
        return nullptr;
    }
    
    consume(TokenType::RIGHT_BRACE);
    
    auto textNode = std::make_unique<TextNode>(content);
    return textNode;
}

std::unique_ptr<BaseNode> CHTLParser::parseStyle() {
    if (!match(TokenType::STYLE)) {
        return nullptr;
    }
    
    consume(TokenType::STYLE);
    
    if (!match(TokenType::LEFT_BRACE)) {
        addError("期望 '{' 在 style 之后");
        return nullptr;
    }
    
    consume(TokenType::LEFT_BRACE);
    
    auto styleNode = std::make_unique<StyleNode>();
    
    // 简化的样式解析
    while (peek() && !peek()->isType(TokenType::RIGHT_BRACE)) {
        if (peek()->isType(TokenType::IDENTIFIER)) {
            std::string propertyName = peek()->getValue();
            consume(TokenType::IDENTIFIER);
            
            if (peek() && peek()->isType(TokenType::COLON)) {
                consume(TokenType::COLON);
                
                std::string propertyValue;
                if (peek() && (peek()->isType(TokenType::STRING) || peek()->isType(TokenType::LITERAL))) {
                    propertyValue = peek()->getValue();
                    consume({TokenType::STRING, TokenType::LITERAL});
                }
                
                styleNode->addInlineProperty(propertyName, propertyValue);
            }
        } else {
            consume();
        }
    }
    
    if (!match(TokenType::RIGHT_BRACE)) {
        addError("期望 '}' 在 style 内容之后");
        return nullptr;
    }
    
    consume(TokenType::RIGHT_BRACE);
    
    return styleNode;
}

std::unique_ptr<BaseNode> CHTLParser::parseScript() {
    if (!match(TokenType::SCRIPT)) {
        return nullptr;
    }
    
    consume(TokenType::SCRIPT);
    
    if (!match(TokenType::LEFT_BRACE)) {
        addError("期望 '{' 在 script 之后");
        return nullptr;
    }
    
    consume(TokenType::LEFT_BRACE);
    
    auto scriptNode = std::make_unique<ScriptNode>();
    
    // 简化的脚本解析
    std::string content;
    while (peek() && !peek()->isType(TokenType::RIGHT_BRACE)) {
        content += peek()->getValue();
        consume();
    }
    
    if (!match(TokenType::RIGHT_BRACE)) {
        addError("期望 '}' 在 script 内容之后");
        return nullptr;
    }
    
    consume(TokenType::RIGHT_BRACE);
    
    scriptNode->setContent(content);
    return scriptNode;
}

std::unique_ptr<BaseNode> CHTLParser::parseTemplate() {
    // 简化实现
    addError("模板解析尚未实现");
    return nullptr;
}

std::unique_ptr<BaseNode> CHTLParser::parseCustom() {
    // 简化实现
    addError("自定义解析尚未实现");
    return nullptr;
}

std::unique_ptr<BaseNode> CHTLParser::parseOrigin() {
    // 简化实现
    addError("原始嵌入解析尚未实现");
    return nullptr;
}

std::unique_ptr<BaseNode> CHTLParser::parseImport() {
    // 简化实现
    addError("导入解析尚未实现");
    return nullptr;
}

std::unique_ptr<BaseNode> CHTLParser::parseConfiguration() {
    // 简化实现
    addError("配置解析尚未实现");
    return nullptr;
}

std::unique_ptr<BaseNode> CHTLParser::parseNamespace() {
    // 简化实现
    addError("命名空间解析尚未实现");
    return nullptr;
}

bool CHTLParser::match(TokenType type) {
    return peek() && peek()->isType(type);
}

bool CHTLParser::match(const std::vector<TokenType>& types) {
    if (!peek()) return false;
    
    for (TokenType type : types) {
        if (peek()->isType(type)) {
            return true;
        }
    }
    return false;
}

Token* CHTLParser::consume(TokenType type) {
    if (!match(type)) {
        addError("期望令牌类型: " + Token::typeToString(type));
        return nullptr;
    }
    return m_tokens->next();
}

Token* CHTLParser::consume(const std::vector<TokenType>& types) {
    if (!match(types)) {
        addError("期望令牌类型之一");
        return nullptr;
    }
    return m_tokens->next();
}

Token* CHTLParser::peek() {
    return m_tokens->current();
}

Token* CHTLParser::peek(size_t offset) {
    return m_tokens->at(m_tokens->getPosition() + offset);
}

bool CHTLParser::isAtEnd() {
    return !m_tokens->hasMore() || (peek() && peek()->isType(TokenType::END_OF_FILE));
}

void CHTLParser::addError(const std::string& message) {
    std::string error = message;
    if (peek()) {
        error += " (行 " + std::to_string(peek()->getLine()) + 
                ", 列 " + std::to_string(peek()->getColumn()) + ")";
    }
    m_errors.push_back(error);
}

} // namespace CHTL