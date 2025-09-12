#include "CHTLParser.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {

std::map<std::string, std::string> CHTLParser::parseAttributes() {
    std::map<std::string, std::string> attributes;
    
    while (currentToken < tokens.size() && 
           current().type != CHTLTokenType::LEFT_BRACE && 
           current().type != CHTLTokenType::EOF_TOKEN) {
        
        skipWhitespaceAndComments();
        
        if (current().type == CHTLTokenType::IDENTIFIER) {
            std::string attrName = current().value;
            advance();
            
            if (match(CHTLTokenType::COLON) || match(CHTLTokenType::EQUAL)) {
                advance();
                
                std::string attrValue = parseAttributeValue();
                attributes[attrName] = attrValue;
                
                if (match(CHTLTokenType::SEMICOLON)) {
                    advance();
                }
            } else {
                addError("期望 ':' 或 '='");
            }
        } else {
            break;
        }
    }
    
    return attributes;
}

std::string CHTLParser::parseAttributeValue() {
    if (current().type == CHTLTokenType::STRING || 
        current().type == CHTLTokenType::UNQUOTED_LITERAL ||
        current().type == CHTLTokenType::IDENTIFIER) {
        std::string value = current().value;
        advance();
        return value;
    } else if (current().type == CHTLTokenType::NUMBER) {
        std::string value = current().value;
        advance();
        return value;
    } else {
        addError("期望属性值");
        return "";
    }
}

std::map<std::string, std::string> CHTLParser::parseStyleProperties() {
    std::map<std::string, std::string> properties;
    
    while (currentToken < tokens.size() && 
           current().type != CHTLTokenType::RIGHT_BRACE && 
           current().type != CHTLTokenType::EOF_TOKEN) {
        
        skipWhitespaceAndComments();
        
        if (current().type == CHTLTokenType::RIGHT_BRACE) {
            break;
        }
        
        if (current().type == CHTLTokenType::IDENTIFIER) {
            std::string propName = current().value;
            advance();
            
            if (match(CHTLTokenType::COLON) || match(CHTLTokenType::EQUAL)) {
                advance();
                
                std::string propValue = parseStylePropertyValue();
                properties[propName] = propValue;
                
                if (match(CHTLTokenType::SEMICOLON)) {
                    advance();
                }
            } else {
                addError("期望 ':' 或 '='");
                advance(); // 避免无限循环
            }
        } else {
            addError("期望样式属性名");
            advance(); // 避免无限循环
        }
    }
    
    return properties;
}

std::string CHTLParser::parseStylePropertyValue() {
    std::string value;
    
    // 处理条件表达式
    if (current().type == CHTLTokenType::IDENTIFIER && 
        (peek().type == CHTLTokenType::QUESTION || 
         peek().type == CHTLTokenType::GREATER || 
         peek().type == CHTLTokenType::LESS)) {
        value = parseConditionalExpression();
    } else {
        // 解析复合CSS值（如 100px, 50%, 1.5em等）
        while (currentToken < tokens.size() && 
               current().type != CHTLTokenType::SEMICOLON && 
               current().type != CHTLTokenType::RIGHT_BRACE && 
               current().type != CHTLTokenType::EOF_TOKEN) {
            
            // 添加当前token的值
            if (current().type == CHTLTokenType::STRING || 
                current().type == CHTLTokenType::UNQUOTED_LITERAL ||
                current().type == CHTLTokenType::IDENTIFIER ||
                current().type == CHTLTokenType::NUMBER ||
                current().type == CHTLTokenType::HASH) {
                value += current().value;
                advance();
            } else {
                break;
            }
        }
        
        if (value.empty()) {
            addError("期望样式属性值");
            return "";
        }
    }
    
    return value;
}

void CHTLParser::parseCSSRule(std::shared_ptr<StyleNode> style) {
    std::cout << "[DEBUG] 开始解析CSS规则: " << current().toString() << std::endl;
    std::string selector;
    
    // 解析选择器
    if (current().type == CHTLTokenType::DOT) {
        // 类选择器 .class
        advance(); // 跳过 .
        if (current().type == CHTLTokenType::IDENTIFIER) {
            selector = "." + current().value;
            advance();
        } else {
            addError("期望类名");
            return;
        }
    } else if (current().type == CHTLTokenType::HASH) {
        // ID选择器 #id
        advance(); // 跳过 #
        if (current().type == CHTLTokenType::IDENTIFIER) {
            selector = "#" + current().value;
            advance();
        } else {
            addError("期望ID名");
            return;
        }
    } else if (current().type == CHTLTokenType::AMPERSAND) {
        // 上下文推导 &:hover, &::before
        advance(); // 跳过 &
        if (current().type == CHTLTokenType::COLON) {
            advance(); // 跳过 :
            if (current().type == CHTLTokenType::IDENTIFIER) {
                selector = "&:" + current().value;
                advance();
            } else {
                addError("期望伪类名");
                return;
            }
        } else if (current().type == CHTLTokenType::COLON && peek().type == CHTLTokenType::COLON) {
            // 伪元素选择器 &::before
            advance(); // 跳过第一个 :
            advance(); // 跳过第二个 :
            if (current().type == CHTLTokenType::IDENTIFIER) {
                selector = "&::" + current().value;
                advance();
            } else {
                addError("期望伪元素名");
                return;
            }
        } else {
            addError("期望 : 或 ::");
            return;
        }
    }
    
    // 解析选择器规则块
    if (match(CHTLTokenType::LEFT_BRACE)) {
        advance(); // 跳过 {
        
        // 解析规则内容
        auto properties = parseStyleProperties();
        
        // 将规则添加到样式中
        std::cout << "[DEBUG] 添加CSS规则: " << selector << std::endl;
        for (const auto& prop : properties) {
            std::cout << "[DEBUG]   属性: " << prop.first << " = " << prop.second << std::endl;
            style->addCSSRule(selector, prop.first, prop.second);
        }
        
        // 自动化类名/ID生成
        if (selector.length() > 0 && selector[0] == '.') {
            std::string className = selector.substr(1); // 去掉开头的 .
            std::cout << "[DEBUG] 自动添加类名: " << className << std::endl;
            style->addClassSelector(className);
        } else if (selector.length() > 0 && selector[0] == '#') {
            std::string idName = selector.substr(1); // 去掉开头的 #
            std::cout << "[DEBUG] 自动添加ID: " << idName << std::endl;
            style->addIdSelector(idName);
        }
        
        if (match(CHTLTokenType::RIGHT_BRACE)) {
            advance(); // 跳过 }
        } else {
            addError("期望 '}'");
        }
    } else {
        addError("期望 '{'");
    }
}

std::string CHTLParser::parseConditionalExpression() {
    std::string expression;
    
    // 解析条件部分
    if (current().type == CHTLTokenType::IDENTIFIER) {
        expression += current().value;
        advance();
        
        // 解析比较操作符
        if (current().type == CHTLTokenType::GREATER || 
            current().type == CHTLTokenType::LESS || 
            current().type == CHTLTokenType::GREATER_EQUAL || 
            current().type == CHTLTokenType::LESS_EQUAL || 
            current().type == CHTLTokenType::EQUAL_EQUAL || 
            current().type == CHTLTokenType::NOT_EQUAL) {
            expression += " " + current().value + " ";
            advance();
            
            // 解析右侧值
            if (current().type == CHTLTokenType::NUMBER || 
                current().type == CHTLTokenType::STRING || 
                current().type == CHTLTokenType::UNQUOTED_LITERAL) {
                expression += current().value;
                advance();
            }
        }
        
        // 解析 ? 操作符
        if (current().type == CHTLTokenType::QUESTION) {
            expression += " ? ";
            advance();
            
            // 解析真值
            if (current().type == CHTLTokenType::STRING || 
                current().type == CHTLTokenType::UNQUOTED_LITERAL) {
                expression += current().value;
                advance();
            }
            
            // 解析 : 操作符
            if (current().type == CHTLTokenType::COLON) {
                expression += " : ";
                advance();
                
                // 解析假值
                if (current().type == CHTLTokenType::STRING || 
                    current().type == CHTLTokenType::UNQUOTED_LITERAL) {
                    expression += current().value;
                    advance();
                }
            }
        }
    }
    
    return expression;
}

std::string CHTLParser::parseExpression() {
    std::string expression;
    
    // 简单的表达式解析
    while (currentToken < tokens.size() && 
           current().type != CHTLTokenType::SEMICOLON && 
           current().type != CHTLTokenType::RIGHT_BRACE && 
           current().type != CHTLTokenType::EOF_TOKEN) {
        
        if (current().type == CHTLTokenType::IDENTIFIER || 
            current().type == CHTLTokenType::STRING || 
            current().type == CHTLTokenType::UNQUOTED_LITERAL || 
            current().type == CHTLTokenType::NUMBER ||
            current().type == CHTLTokenType::PLUS || 
            current().type == CHTLTokenType::MINUS || 
            current().type == CHTLTokenType::MULTIPLY || 
            current().type == CHTLTokenType::DIVIDE || 
            current().type == CHTLTokenType::MODULO || 
            current().type == CHTLTokenType::POWER ||
            current().type == CHTLTokenType::AND || 
            current().type == CHTLTokenType::OR ||
            current().type == CHTLTokenType::GREATER || 
            current().type == CHTLTokenType::LESS || 
            current().type == CHTLTokenType::GREATER_EQUAL || 
            current().type == CHTLTokenType::LESS_EQUAL || 
            current().type == CHTLTokenType::EQUAL_EQUAL || 
            current().type == CHTLTokenType::NOT_EQUAL) {
            expression += current().value + " ";
            advance();
        } else {
            break;
        }
    }
    
    return expression;
}

std::string CHTLParser::parseArithmeticExpression() {
    return parseExpression();
}

std::string CHTLParser::parseLogicalExpression() {
    return parseExpression();
}

TemplateNode::TemplateType CHTLParser::parseTemplateType() {
    if (current().type == CHTLTokenType::TEMPLATE_STYLE) {
        advance();
        return TemplateNode::TemplateType::STYLE;
    } else if (current().type == CHTLTokenType::TEMPLATE_ELEMENT) {
        advance();
        return TemplateNode::TemplateType::ELEMENT;
    } else if (current().type == CHTLTokenType::TEMPLATE_VAR) {
        advance();
        return TemplateNode::TemplateType::VAR;
    } else {
        addError("期望模板类型");
        return TemplateNode::TemplateType::STYLE;
    }
}

CustomNode::CustomType CHTLParser::parseCustomType() {
    if (current().type == CHTLTokenType::TEMPLATE_STYLE) {
        advance();
        return CustomNode::CustomType::STYLE;
    } else if (current().type == CHTLTokenType::TEMPLATE_ELEMENT) {
        advance();
        return CustomNode::CustomType::ELEMENT;
    } else if (current().type == CHTLTokenType::TEMPLATE_VAR) {
        advance();
        return CustomNode::CustomType::VAR;
    } else {
        addError("期望自定义类型");
        return CustomNode::CustomType::STYLE;
    }
}

OriginNode::OriginType CHTLParser::parseOriginType() {
    if (current().value == "@Html") {
        advance();
        return OriginNode::OriginType::HTML;
    } else if (current().value == "@Style") {
        advance();
        return OriginNode::OriginType::CSS;
    } else if (current().value == "@JavaScript") {
        advance();
        return OriginNode::OriginType::JAVASCRIPT;
    } else {
        addError("期望原始嵌入类型");
        return OriginNode::OriginType::TEXT;
    }
}

ImportNode::ImportType CHTLParser::parseImportType() {
    if (current().type == CHTLTokenType::IDENTIFIER) {
        std::string type = current().value;
        advance();
        
        if (type == "html") {
            return ImportNode::ImportType::HTML;
        } else if (type == "css") {
            return ImportNode::ImportType::CSS;
        } else if (type == "javascript") {
            return ImportNode::ImportType::JAVASCRIPT;
        } else if (type == "chtl") {
            return ImportNode::ImportType::CHTL;
        }
    }
    
    addError("期望导入类型");
    return ImportNode::ImportType::HTML;
}

ConstraintNode::ConstraintType CHTLParser::parseConstraintType() {
    if (current().type == CHTLTokenType::IDENTIFIER) {
        std::string type = current().value;
        advance();
        
        if (type == "precise") {
            return ConstraintNode::ConstraintType::PRECISE;
        } else if (type == "type") {
            return ConstraintNode::ConstraintType::TYPE;
        } else if (type == "global") {
            return ConstraintNode::ConstraintType::GLOBAL;
        }
    }
    
    addError("期望约束类型");
    return ConstraintNode::ConstraintType::PRECISE;
}

void CHTLParser::parseTemplateUsage(std::shared_ptr<StyleNode> style) {
    // 解析模板使用，如 @Style DefaultText;
    if (current().type == CHTLTokenType::TEMPLATE_STYLE ||
        current().type == CHTLTokenType::TEMPLATE_ELEMENT ||
        current().type == CHTLTokenType::TEMPLATE_VAR) {
        
        std::string templateType = current().value;
        advance();
        
        if (current().type == CHTLTokenType::IDENTIFIER) {
            std::string templateName = current().value;
            advance();
            
            // 添加到样式的模板使用列表
            style->addTemplateUsage(templateType, templateName);
            
            // 期望分号
            if (match(CHTLTokenType::SEMICOLON)) {
                advance();
            } else {
                addError("期望 ';'");
            }
        } else {
            addError("期望模板名称");
        }
    } else {
        addError("期望模板使用");
    }
}

void StyleNode::addTemplateUsage(const std::string& templateType, const std::string& templateName) {
    templateUsages.push_back({templateType, templateName});
}

std::shared_ptr<CHTLNode> CHTLParser::parseTemplateUsage() {
    // 解析模板使用，如 @Style DefaultText; 或 @Element Button;
    if (current().type == CHTLTokenType::TEMPLATE_STYLE ||
        current().type == CHTLTokenType::TEMPLATE_ELEMENT ||
        current().type == CHTLTokenType::TEMPLATE_VAR) {
        
        std::string templateType = current().value;
        advance();
        
        if (current().type == CHTLTokenType::IDENTIFIER) {
            std::string templateName = current().value;
            advance();
            
            // 创建一个模板使用节点
            auto templateUsage = std::make_shared<ElementNode>("template-usage");
            templateUsage->addAttribute("type", templateType);
            templateUsage->addAttribute("name", templateName);
            
            // 期望分号
            if (match(CHTLTokenType::SEMICOLON)) {
                advance();
            } else {
                addError("期望 ';'");
            }
            
            return templateUsage;
        } else {
            addError("期望模板名称");
            return nullptr;
        }
    } else {
        addError("期望模板使用");
        return nullptr;
    }
}

} // namespace CHTL