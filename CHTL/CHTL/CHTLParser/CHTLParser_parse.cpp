#include "CHTLParser.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {

std::shared_ptr<CHTLNode> CHTLParser::parse() {
    if (tokens.empty()) {
        addError("没有tokens可解析");
        return nullptr;
    }
    
    currentToken = 0;
    errors.clear();
    warnings.clear();
    
    return parseDocument();
}

std::shared_ptr<CHTLNode> CHTLParser::parse(const std::vector<CHTLToken>& tokens) {
    setTokens(tokens);
    return parse();
}

std::shared_ptr<CHTLNode> CHTLParser::parseDocument() {
    // 创建一个根元素来包含所有内容
    auto root = std::make_shared<ElementNode>("document");
    
    while (currentToken < tokens.size() && current().type != CHTLTokenType::EOF_TOKEN) {
        skipWhitespaceAndComments();
        
        if (currentToken >= tokens.size() || current().type == CHTLTokenType::EOF_TOKEN) {
            break;
        }
        
        std::shared_ptr<CHTLNode> node = nullptr;
        
        // 根据当前token类型决定解析方法
        switch (current().type) {
            case CHTLTokenType::TEXT:
                node = parseText();
                break;
            case CHTLTokenType::TEMPLATE:
                node = parseTemplate();
                break;
            case CHTLTokenType::CUSTOM:
                node = parseCustom();
                break;
            case CHTLTokenType::ORIGIN:
                node = parseOrigin();
                break;
            case CHTLTokenType::IMPORT:
                node = parseImport();
                break;
            case CHTLTokenType::NAMESPACE:
                node = parseNamespace();
                break;
            case CHTLTokenType::CONSTRAINT:
                node = parseConstraint();
                break;
            case CHTLTokenType::CONFIGURATION:
                node = parseConfiguration();
                break;
            case CHTLTokenType::USE:
                node = parseUse();
                break;
            case CHTLTokenType::IDENTIFIER:
                // 可能是HTML元素
                if (isHTMLElement(current().value)) {
                    node = parseElement();
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
        
        if (node) {
            root->addChild(node);
        }
    }
    
    return root;
}

std::shared_ptr<CHTLNode> CHTLParser::parseElement() {
    if (!match(CHTLTokenType::IDENTIFIER)) {
        addError("期望元素名称");
        return nullptr;
    }
    
    std::string tagName = current().value;
    size_t line = current().line;
    size_t column = current().column;
    advance();
    
    auto element = std::make_shared<ElementNode>(tagName, line, column);
    
    // 检查是否有子内容
    if (match(CHTLTokenType::LEFT_BRACE)) {
        consume(CHTLTokenType::LEFT_BRACE, "期望 '{'");
        
        while (currentToken < tokens.size() && current().type != CHTLTokenType::RIGHT_BRACE) {
            skipWhitespaceAndComments();
            
            if (current().type == CHTLTokenType::RIGHT_BRACE) {
                break;
            }
            
            // Debug output
            std::cout << "[DEBUG] 解析子节点: currentToken=" << currentToken 
                      << ", token=" << current().toString() << std::endl;
            
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
                case CHTLTokenType::IDENTIFIER:
                    // 检查是否是属性（后面跟着冒号或等号）
                    std::cout << "[DEBUG] 处理IDENTIFIER: " << current().value 
                              << ", peek(1)=" << peek(1).toString() << std::endl;
                    
                    if (peek(1).type == CHTLTokenType::COLON || peek(1).type == CHTLTokenType::EQUAL) {
                        // 解析属性
                        std::cout << "[DEBUG] 识别为属性" << std::endl;
                        std::string attrName = current().value;
                        advance();
                        
                        std::cout << "[DEBUG] 前进后当前token: " << current().toString() << std::endl;
                        
                        if (match(CHTLTokenType::COLON) || match(CHTLTokenType::EQUAL)) {
                            advance();
                            
                            std::cout << "[DEBUG] 前进到属性值: " << current().toString() << std::endl;
                            
                            std::string attrValue = parseAttributeValue();
                            element->addAttribute(attrName, attrValue);
                            
                            std::cout << "[DEBUG] 属性解析完成: " << attrName << "=" << attrValue 
                                      << ", 当前token: " << current().toString() << std::endl;
                            
                            if (match(CHTLTokenType::SEMICOLON)) {
                                advance();
                                std::cout << "[DEBUG] 跳过分号，当前token: " << current().toString() << std::endl;
                            }
                        } else {
                            addError("期望 ':' 或 '='");
                            advance();
                        }
                    } else if (isHTMLElement(current().value)) {
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
                element->addChild(child);
            }
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "期望 '}'");
    } else {
        // 自闭合标签
        element->isSelfClosing = true;
    }
    
    return element;
}

std::shared_ptr<CHTLNode> CHTLParser::parseText() {
    if (!match(CHTLTokenType::TEXT)) {
        addError("期望text关键字");
        return nullptr;
    }
    
    size_t line = current().line;
    size_t column = current().column;
    advance();
    
    std::string content;
    
    if (match(CHTLTokenType::LEFT_BRACE)) {
        consume(CHTLTokenType::LEFT_BRACE, "期望 '{'");
        
        while (currentToken < tokens.size() && current().type != CHTLTokenType::RIGHT_BRACE) {
            if (current().type == CHTLTokenType::STRING || 
                current().type == CHTLTokenType::UNQUOTED_LITERAL ||
                current().type == CHTLTokenType::IDENTIFIER) {
                content += current().value;
            }
            advance();
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "期望 '}'");
    } else if (match(CHTLTokenType::COLON) || match(CHTLTokenType::EQUAL)) {
        advance(); // 跳过 : 或 =
        
        if (current().type == CHTLTokenType::STRING || 
            current().type == CHTLTokenType::UNQUOTED_LITERAL) {
            content = current().value;
            advance();
        } else {
            addError("期望文本内容");
        }
        
        consume(CHTLTokenType::SEMICOLON, "期望 ';'");
    } else {
        addError("期望 '{' 或 ':'");
    }
    
    return std::make_shared<TextNode>(content, line, column);
}

std::shared_ptr<CHTLNode> CHTLParser::parseComment() {
    CHTLTokenType commentType = current().type;
    std::string content = current().value;
    size_t line = current().line;
    size_t column = current().column;
    advance();
    
    bool isGenerator = (commentType == CHTLTokenType::GENERATOR_COMMENT);
    return std::make_shared<CommentNode>(content, isGenerator, line, column);
}

std::shared_ptr<CHTLNode> CHTLParser::parseStyle() {
    if (!match(CHTLTokenType::STYLE)) {
        addError("期望style关键字");
        return nullptr;
    }
    
    size_t line = current().line;
    size_t column = current().column;
    advance();
    
    auto style = std::make_shared<StyleNode>("", true, line, column); // 局部样式
    
    if (match(CHTLTokenType::LEFT_BRACE)) {
        consume(CHTLTokenType::LEFT_BRACE, "期望 '{'");
        
        while (currentToken < tokens.size() && current().type != CHTLTokenType::RIGHT_BRACE) {
            skipWhitespaceAndComments();
            
            if (current().type == CHTLTokenType::RIGHT_BRACE) {
                break;
            }
            
            // 解析样式属性
            auto properties = parseStyleProperties();
            for (const auto& prop : properties) {
                style->addInlineStyle(prop.first, prop.second);
            }
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "期望 '}'");
    }
    
    return style;
}

std::shared_ptr<CHTLNode> CHTLParser::parseScript() {
    if (!match(CHTLTokenType::SCRIPT)) {
        addError("期望script关键字");
        return nullptr;
    }
    
    size_t line = current().line;
    size_t column = current().column;
    advance();
    
    std::string content;
    
    if (match(CHTLTokenType::LEFT_BRACE)) {
        consume(CHTLTokenType::LEFT_BRACE, "期望 '{'");
        
        while (currentToken < tokens.size() && current().type != CHTLTokenType::RIGHT_BRACE) {
            content += current().value + " ";
            advance();
        }
        
        consume(CHTLTokenType::RIGHT_BRACE, "期望 '}'");
    }
    
    return std::make_shared<ScriptNode>(content, true, line, column);
}

std::shared_ptr<CHTLNode> CHTLParser::parseTemplate() {
    if (!match(CHTLTokenType::TEMPLATE)) {
        addError("期望[Template]关键字");
        return nullptr;
    }
    
    size_t line = current().line;
    size_t column = current().column;
    advance();
    
    // 解析模板类型
    TemplateNode::TemplateType templateType = parseTemplateType();
    
    if (!match(CHTLTokenType::IDENTIFIER)) {
        addError("期望模板名称");
        return nullptr;
    }
    
    std::string name = current().value;
    advance();
    
    auto templateNode = std::make_shared<TemplateNode>(templateType, name, line, column);
    
    if (match(CHTLTokenType::LEFT_BRACE)) {
        consume(CHTLTokenType::LEFT_BRACE, "期望 '{'");
        
        while (currentToken < tokens.size() && current().type != CHTLTokenType::RIGHT_BRACE) {
            skipWhitespaceAndComments();
            
            if (current().type == CHTLTokenType::RIGHT_BRACE) {
                break;
            }
            
            // 根据模板类型解析内容
            switch (templateType) {
                case TemplateNode::TemplateType::STYLE:
                    // 解析样式属性
                    if (current().type == CHTLTokenType::IDENTIFIER) {
                        std::string propName = current().value;
                        advance();
                        
                        if (match(CHTLTokenType::COLON) || match(CHTLTokenType::EQUAL)) {
                            advance();
                            
                            if (current().type == CHTLTokenType::STRING || 
                                current().type == CHTLTokenType::UNQUOTED_LITERAL) {
                                templateNode->addProperty(propName, current().value);
                                advance();
                            }
                        }
                        
                        consume(CHTLTokenType::SEMICOLON, "期望 ';'");
                    }
                    break;
                case TemplateNode::TemplateType::ELEMENT:
                    // 解析子元素
                    if (isHTMLElement(current().value)) {
                        auto child = parseElement();
                        if (child) {
                            templateNode->addChild(child);
                        }
                    }
                    break;
                case TemplateNode::TemplateType::VAR:
                    // 解析变量
                    if (current().type == CHTLTokenType::IDENTIFIER) {
                        std::string varName = current().value;
                        advance();
                        
                        if (match(CHTLTokenType::COLON) || match(CHTLTokenType::EQUAL)) {
                            advance();
                            
                            if (current().type == CHTLTokenType::STRING || 
                                current().type == CHTLTokenType::UNQUOTED_LITERAL) {
                                templateNode->addProperty(varName, current().value);
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
    
    // 添加到模板存储
    addTemplate(templateNode);
    
    return templateNode;
}

} // namespace CHTL