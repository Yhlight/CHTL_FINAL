#include "CHTLParser.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {

std::shared_ptr<CHTLNode> CHTLParser::parseCustom() {
    if (!match(CHTLTokenType::CUSTOM)) {
        addError("期望[Custom]关键字");
        return nullptr;
    }
    
    size_t line = current().line;
    size_t column = current().column;
    advance();
    
    // 解析自定义类型
    CustomNode::CustomType customType = parseCustomType();
    
    if (!match(CHTLTokenType::IDENTIFIER)) {
        addError("期望自定义名称");
        return nullptr;
    }
    
    std::string name = current().value;
    advance();
    
    auto customNode = std::make_shared<CustomNode>(customType, name, line, column);
    
    if (match(CHTLTokenType::LEFT_BRACE)) {
        consume(CHTLTokenType::LEFT_BRACE, "期望 '{'");
        
        while (currentToken < tokens.size() && current().type != CHTLTokenType::RIGHT_BRACE) {
            skipWhitespaceAndComments();
            
            if (current().type == CHTLTokenType::RIGHT_BRACE) {
                break;
            }
            
            // 根据自定义类型解析内容
            switch (customType) {
                case CustomNode::CustomType::STYLE:
                    // 解析样式属性
                    if (current().type == CHTLTokenType::IDENTIFIER) {
                        std::string propName = current().value;
                        advance();
                        
                        if (match(CHTLTokenType::COLON) || match(CHTLTokenType::EQUAL)) {
                            advance();
                            
                            if (current().type == CHTLTokenType::STRING || 
                                current().type == CHTLTokenType::UNQUOTED_LITERAL) {
                                customNode->addProperty(propName, current().value);
                                advance();
                            }
                        }
                        
                        consume(CHTLTokenType::SEMICOLON, "期望 ';'");
                    }
                    break;
                case CustomNode::CustomType::ELEMENT:
                    // 解析子元素
                    if (isHTMLElement(current().value)) {
                        auto child = parseElement();
                        if (child) {
                            customNode->addChild(child);
                        }
                    }
                    break;
                case CustomNode::CustomType::VAR:
                    // 解析变量
                    if (current().type == CHTLTokenType::IDENTIFIER) {
                        std::string varName = current().value;
                        advance();
                        
                        if (match(CHTLTokenType::COLON) || match(CHTLTokenType::EQUAL)) {
                            advance();
                            
                            if (current().type == CHTLTokenType::STRING || 
                                current().type == CHTLTokenType::UNQUOTED_LITERAL) {
                                customNode->addProperty(varName, current().value);
                                advance();
                            }
                        }
                        
                        consume(CHTLTokenType::SEMICOLON, "期望 ';'");
                    }
                    break;
            }
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "期望 '}'");
    }
    
    // 添加到自定义存储
    addCustom(customNode);
    
    return customNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseOrigin() {
    if (!match(CHTLTokenType::ORIGIN)) {
        addError("期望原始嵌入关键字");
        return nullptr;
    }
    
    size_t line = current().line;
    size_t column = current().column;
    OriginNode::OriginType originType = parseOriginType();
    
    std::string content;
    std::string name;
    
    if (match(CHTLTokenType::LEFT_BRACE)) {
        consume(CHTLTokenType::LEFT_BRACE, "期望 '{'");
        
        while (currentToken < tokens.size() && current().type != CHTLTokenType::RIGHT_BRACE) {
            content += current().value + " ";
            advance();
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "期望 '}'");
    } else if (current().type == CHTLTokenType::STRING || 
               current().type == CHTLTokenType::UNQUOTED_LITERAL) {
        content = current().value;
        advance();
    }
    
    auto originNode = std::make_shared<OriginNode>(originType, content, line, column);
    originNode->name = name;
    
    return originNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseImport() {
    if (!match(CHTLTokenType::IMPORT)) {
        addError("期望import关键字");
        return nullptr;
    }
    
    size_t line = current().line;
    size_t column = current().column;
    advance();
    
    ImportNode::ImportType importType = parseImportType();
    
    if (!match(CHTLTokenType::STRING) && !match(CHTLTokenType::UNQUOTED_LITERAL)) {
        addError("期望导入路径");
        return nullptr;
    }
    
    std::string path = current().value;
    advance();
    
    auto importNode = std::make_shared<ImportNode>(importType, path, line, column);
    
    // 解析别名
    if (match(CHTLTokenType::IDENTIFIER) && current().value == "as") {
        advance();
        
        if (match(CHTLTokenType::IDENTIFIER)) {
            importNode->setAlias(current().value);
            advance();
        }
    }
    
    // 解析目标
    if (match(CHTLTokenType::LEFT_BRACE)) {
        consume(CHTLTokenType::LEFT_BRACE, "期望 '{'");
        
        while (currentToken < tokens.size() && current().type != CHTLTokenType::RIGHT_BRACE) {
            skipWhitespaceAndComments();
            
            if (current().type == CHTLTokenType::RIGHT_BRACE) {
                break;
            }
            
            if (current().type == CHTLTokenType::IDENTIFIER) {
                importNode->addTarget(current().value);
                advance();
            }
            
            if (match(CHTLTokenType::COMMA)) {
                advance();
            }
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "期望 '}'");
    }
    
    return importNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseNamespace() {
    if (!match(CHTLTokenType::NAMESPACE)) {
        addError("期望namespace关键字");
        return nullptr;
    }
    
    size_t line = current().line;
    size_t column = current().column;
    advance();
    
    if (!match(CHTLTokenType::IDENTIFIER)) {
        addError("期望命名空间名称");
        return nullptr;
    }
    
    std::string name = current().value;
    advance();
    
    auto namespaceNode = std::make_shared<NamespaceNode>(name, line, column);
    
    if (match(CHTLTokenType::LEFT_BRACE)) {
        consume(CHTLTokenType::LEFT_BRACE, "期望 '{'");
        
        while (currentToken < tokens.size() && current().type != CHTLTokenType::RIGHT_BRACE) {
            skipWhitespaceAndComments();
            
            if (current().type == CHTLTokenType::RIGHT_BRACE) {
                break;
            }
            
            std::shared_ptr<CHTLNode> child = nullptr;
            
            switch (current().type) {
                case CHTLTokenType::TEXT:
                    child = parseText();
                    break;
                case CHTLTokenType::STYLE:
                    child = parseStyle();
                    break;
                case CHTLTokenType::SCRIPT:
                    child = parseScript();
                    break;
                case CHTLTokenType::TEMPLATE:
                    child = parseTemplate();
                    break;
                case CHTLTokenType::CUSTOM:
                    child = parseCustom();
                    break;
                case CHTLTokenType::ORIGIN:
                    child = parseOrigin();
                    break;
                case CHTLTokenType::IMPORT:
                    child = parseImport();
                    break;
                case CHTLTokenType::IDENTIFIER:
                    if (isHTMLElement(current().value)) {
                        child = parseElement();
                    } else {
                        addError("未知的标识符: " + current().value);
                        advance();
                    }
                    break;
                default:
                    addError("意外的token: " + current().value);
                    advance();
                    break;
            }
            
            if (child) {
                namespaceNode->addChild(child);
            }
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "期望 '}'");
    }
    
    return namespaceNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseConstraint() {
    if (!match(CHTLTokenType::CONSTRAINT)) {
        addError("期望constraint关键字");
        return nullptr;
    }
    
    size_t line = current().line;
    size_t column = current().column;
    advance();
    
    ConstraintNode::ConstraintType constraintType = parseConstraintType();
    
    if (!match(CHTLTokenType::IDENTIFIER)) {
        addError("期望约束目标");
        return nullptr;
    }
    
    std::string target = current().value;
    advance();
    
    auto constraintNode = std::make_shared<ConstraintNode>(constraintType, target, line, column);
    
    if (match(CHTLTokenType::LEFT_BRACE)) {
        consume(CHTLTokenType::LEFT_BRACE, "期望 '{'");
        
        std::string condition;
        while (currentToken < tokens.size() && current().type != CHTLTokenType::RIGHT_BRACE) {
            condition += current().value + " ";
            advance();
        }
        
        constraintNode->setCondition(condition);
        consume(CHTLTokenType::RIGHT_BRACE, "期望 '}'");
    }
    
    return constraintNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseConfiguration() {
    if (!match(CHTLTokenType::CONFIGURATION)) {
        addError("期望[Configuration]关键字");
        return nullptr;
    }
    
    size_t line = current().line;
    size_t column = current().column;
    advance();
    
    std::string name = "default";
    if (match(CHTLTokenType::IDENTIFIER)) {
        name = current().value;
        advance();
    }
    
    auto configNode = std::make_shared<ConfigurationNode>(name, line, column);
    
    if (match(CHTLTokenType::LEFT_BRACE)) {
        consume(CHTLTokenType::LEFT_BRACE, "期望 '{'");
        
        while (currentToken < tokens.size() && current().type != CHTLTokenType::RIGHT_BRACE) {
            skipWhitespaceAndComments();
            
            if (current().type == CHTLTokenType::RIGHT_BRACE) {
                break;
            }
            
            if (current().type == CHTLTokenType::IDENTIFIER) {
                std::string propName = current().value;
                advance();
                
                if (match(CHTLTokenType::EQUAL)) {
                    advance();
                    
                    if (current().type == CHTLTokenType::STRING || 
                        current().type == CHTLTokenType::UNQUOTED_LITERAL ||
                        current().type == CHTLTokenType::NUMBER) {
                        configNode->addProperty(propName, current().value);
                        advance();
                    }
                }
                
                consume(CHTLTokenType::SEMICOLON, "期望 ';'");
            }
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "期望 '}'");
    }
    
    // 添加到配置存储
    addConfiguration(configNode);
    
    return configNode;
}

std::shared_ptr<CHTLNode> CHTLParser::parseUse() {
    if (!match(CHTLTokenType::USE)) {
        addError("期望use关键字");
        return nullptr;
    }
    
    size_t line = current().line;
    size_t column = current().column;
    advance();
    
    if (!match(CHTLTokenType::IDENTIFIER)) {
        addError("期望use目标");
        return nullptr;
    }
    
    std::string target = current().value;
    advance();
    
    auto useNode = std::make_shared<UseNode>(target, line, column);
    
    // 解析参数
    if (match(CHTLTokenType::LEFT_BRACE)) {
        consume(CHTLTokenType::LEFT_BRACE, "期望 '{'");
        
        while (currentToken < tokens.size() && current().type != CHTLTokenType::RIGHT_BRACE) {
            skipWhitespaceAndComments();
            
            if (current().type == CHTLTokenType::RIGHT_BRACE) {
                break;
            }
            
            if (current().type == CHTLTokenType::STRING || 
                current().type == CHTLTokenType::UNQUOTED_LITERAL ||
                current().type == CHTLTokenType::NUMBER) {
                useNode->addParameter(current().value);
                advance();
            }
            
            if (match(CHTLTokenType::COMMA)) {
                advance();
            }
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "期望 '}'");
    }
    
    return useNode;
}

} // namespace CHTL