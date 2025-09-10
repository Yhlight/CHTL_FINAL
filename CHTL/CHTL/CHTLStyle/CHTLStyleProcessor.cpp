#include "CHTLStyleProcessor.h"
#include <sstream>
#include <algorithm>
#include <regex>
#include <cmath>

namespace CHTL {

CHTLStyleProcessor::CHTLStyleProcessor() 
    : debugMode(false), strictMode(false), autoAddClass(true), autoAddId(true) {
    propertyReference = std::make_unique<CHTLPropertyReference>();
}

std::string CHTLStyleProcessor::processStyleBlock(std::shared_ptr<StyleNode> styleNode, std::shared_ptr<ElementNode> element) {
    if (!styleNode || !element) {
        addError("样式节点或元素节点为空");
        return "";
    }
    
    try {
        std::ostringstream oss;
        
        // 收集元素属性
        collectElementProperties(element);
        
        // 处理内联样式
        if (!styleNode->getStyleProperties().empty()) {
            std::string inlineStyle = processInlineStyle(styleNode->getStyleProperties());
            if (!inlineStyle.empty()) {
                element->setStyle(inlineStyle);
            }
        }
        
        // 处理样式规则
        for (const auto& rule : styleNode->getStyleRules()) {
            std::string selector = rule->getAttribute("selector");
            if (!selector.empty()) {
                std::map<std::string, std::string> properties;
                for (const auto& attr : rule->getAttributes()) {
                    if (attr.first != "type" && attr.first != "selector") {
                        properties[attr.first] = attr.second;
                    }
                }
                
                std::string processedRule = processStyleRule(selector, properties);
                if (!processedRule.empty()) {
                    oss << processedRule << "\n";
                }
            }
        }
        
        // 自动化处理
        if (autoAddClass || autoAddId) {
            processAutoClassAndId(styleNode, element);
        }
        
        return oss.str();
        
    } catch (const std::exception& e) {
        addError("处理样式块时发生错误: " + std::string(e.what()));
        return "";
    }
}

std::string CHTLStyleProcessor::processGlobalStyleBlock(std::shared_ptr<StyleNode> styleNode) {
    if (!styleNode) {
        addError("样式节点为空");
        return "";
    }
    
    try {
        std::ostringstream oss;
        
        // 处理样式规则
        for (const auto& rule : styleNode->getStyleRules()) {
            std::string selector = rule->getAttribute("selector");
            if (!selector.empty()) {
                std::map<std::string, std::string> properties;
                for (const auto& attr : rule->getAttributes()) {
                    if (attr.first != "type" && attr.first != "selector") {
                        properties[attr.first] = attr.second;
                    }
                }
                
                std::string processedRule = processStyleRule(selector, properties);
                if (!processedRule.empty()) {
                    oss << processedRule << "\n";
                }
            }
        }
        
        return oss.str();
        
    } catch (const std::exception& e) {
        addError("处理全局样式块时发生错误: " + std::string(e.what()));
        return "";
    }
}

void CHTLStyleProcessor::collectElementProperties(std::shared_ptr<ElementNode> element) {
    if (!element) {
        return;
    }
    
    // 收集元素的基本属性
    elementProperties["tagName"] = element->getTagName();
    elementProperties["id"] = element->getId();
    elementProperties["class"] = element->getAttribute("class");
    
    // 收集样式属性
    std::string style = element->getStyle();
    if (!style.empty()) {
        std::istringstream iss(style);
        std::string property;
        
        while (std::getline(iss, property, ';')) {
            size_t colonPos = property.find(':');
            if (colonPos != std::string::npos) {
                std::string propName = property.substr(0, colonPos);
                std::string propValue = property.substr(colonPos + 1);
                
                // 去除空白
                propName.erase(0, propName.find_first_not_of(" \t"));
                propName.erase(propName.find_last_not_of(" \t") + 1);
                propValue.erase(0, propValue.find_first_not_of(" \t"));
                propValue.erase(propValue.find_last_not_of(" \t") + 1);
                
                if (!propName.empty() && !propValue.empty()) {
                    elementProperties[propName] = propValue;
                }
            }
        }
    }
    
    // 存储元素引用
    if (!element->getId().empty()) {
        elementMap[element->getId()] = element;
    }
    
    // 递归收集子元素属性
    for (const auto& child : element->getChildren()) {
        if (child->getNodeType() == NodeType::ELEMENT) {
            auto childElement = std::static_pointer_cast<ElementNode>(child);
            collectElementProperties(childElement);
        }
    }
}

void CHTLStyleProcessor::clearElementProperties() {
    elementProperties.clear();
    elementMap.clear();
}

std::string CHTLStyleProcessor::evaluateArithmeticExpression(const std::string& expression) const {
    // 简化的算术表达式求值
    std::vector<std::string> tokens = tokenizeExpression(expression);
    
    if (tokens.empty()) {
        return expression;
    }
    
    // 简单的算术运算处理
    double result = 0.0;
    std::string operation = "+";
    
    for (const auto& token : tokens) {
        if (isNumber(token)) {
            double value = std::stod(token);
            if (operation == "+") {
                result += value;
            } else if (operation == "-") {
                result -= value;
            } else if (operation == "*") {
                result *= value;
            } else if (operation == "/") {
                if (value != 0) {
                    result /= value;
                } else {
                    const_cast<CHTLStyleProcessor*>(this)->addError("除零错误");
                    return expression;
                }
            } else if (operation == "%") {
                result = std::fmod(result, value);
            } else if (operation == "**") {
                result = std::pow(result, value);
            }
        } else if (isOperator(token)) {
            operation = token;
        } else if (isIdentifier(token)) {
            // 处理属性引用
            std::string refValue = resolvePropertyReference(token);
            if (!refValue.empty() && isNumber(refValue)) {
                double value = std::stod(refValue);
                if (operation == "+") {
                    result += value;
                } else if (operation == "-") {
                    result -= value;
                } else if (operation == "*") {
                    result *= value;
                } else if (operation == "/") {
                    if (value != 0) {
                        result /= value;
                    } else {
                        const_cast<CHTLStyleProcessor*>(this)->addError("除零错误");
                        return expression;
                    }
                } else if (operation == "%") {
                    result = std::fmod(result, value);
                } else if (operation == "**") {
                    result = std::pow(result, value);
                }
            }
        }
    }
    
    return std::to_string(result);
}

std::string CHTLStyleProcessor::evaluateConditionalExpression(const std::string& expression) const {
    // 简化的条件表达式求值
    size_t questionPos = expression.find('?');
    if (questionPos == std::string::npos) {
        return expression;
    }
    
    std::string condition = expression.substr(0, questionPos);
    std::string remaining = expression.substr(questionPos + 1);
    
    // 去除空白
    condition.erase(0, condition.find_first_not_of(" \t"));
    condition.erase(condition.find_last_not_of(" \t") + 1);
    
    // 评估条件
    bool conditionResult = evaluateLogicalExpression(condition) == "true";
    
    // 查找冒号
    size_t colonPos = remaining.find(':');
    if (colonPos == std::string::npos) {
        return remaining;
    }
    
    std::string trueValue = remaining.substr(0, colonPos);
    std::string falseValue = remaining.substr(colonPos + 1);
    
    // 去除空白
    trueValue.erase(0, trueValue.find_first_not_of(" \t"));
    trueValue.erase(trueValue.find_last_not_of(" \t") + 1);
    falseValue.erase(0, falseValue.find_first_not_of(" \t"));
    falseValue.erase(falseValue.find_last_not_of(" \t") + 1);
    
    return conditionResult ? trueValue : falseValue;
}

std::string CHTLStyleProcessor::evaluateLogicalExpression(const std::string& expression) const {
    // 简化的逻辑表达式求值
    if (expression.find("&&") != std::string::npos) {
        size_t andPos = expression.find("&&");
        std::string left = expression.substr(0, andPos);
        std::string right = expression.substr(andPos + 2);
        
        left.erase(0, left.find_first_not_of(" \t"));
        left.erase(left.find_last_not_of(" \t") + 1);
        right.erase(0, right.find_first_not_of(" \t"));
        right.erase(right.find_last_not_of(" \t") + 1);
        
        bool leftResult = evaluateEqualityExpression(left) == "true";
        bool rightResult = evaluateEqualityExpression(right) == "true";
        
        return (leftResult && rightResult) ? "true" : "false";
    } else if (expression.find("||") != std::string::npos) {
        size_t orPos = expression.find("||");
        std::string left = expression.substr(0, orPos);
        std::string right = expression.substr(orPos + 2);
        
        left.erase(0, left.find_first_not_of(" \t"));
        left.erase(left.find_last_not_of(" \t") + 1);
        right.erase(0, right.find_first_not_of(" \t"));
        right.erase(right.find_last_not_of(" \t") + 1);
        
        bool leftResult = evaluateEqualityExpression(left) == "true";
        bool rightResult = evaluateEqualityExpression(right) == "true";
        
        return (leftResult || rightResult) ? "true" : "false";
    } else {
        return evaluateEqualityExpression(expression);
    }
}

std::string CHTLStyleProcessor::evaluateEqualityExpression(const std::string& expression) const {
    // 简化的相等性表达式求值
    if (expression.find("==") != std::string::npos) {
        size_t eqPos = expression.find("==");
        std::string left = expression.substr(0, eqPos);
        std::string right = expression.substr(eqPos + 2);
        
        left.erase(0, left.find_first_not_of(" \t"));
        left.erase(left.find_last_not_of(" \t") + 1);
        right.erase(0, right.find_first_not_of(" \t"));
        right.erase(right.find_last_not_of(" \t") + 1);
        
        return (left == right) ? "true" : "false";
    } else if (expression.find("!=") != std::string::npos) {
        size_t nePos = expression.find("!=");
        std::string left = expression.substr(0, nePos);
        std::string right = expression.substr(nePos + 2);
        
        left.erase(0, left.find_first_not_of(" \t"));
        left.erase(left.find_last_not_of(" \t") + 1);
        right.erase(0, right.find_first_not_of(" \t"));
        right.erase(right.find_last_not_of(" \t") + 1);
        
        return (left != right) ? "true" : "false";
    } else {
        return evaluateRelationalExpression(expression);
    }
}

std::string CHTLStyleProcessor::evaluateRelationalExpression(const std::string& expression) const {
    // 简化的关系表达式求值
    if (expression.find(">=") != std::string::npos) {
        size_t gePos = expression.find(">=");
        std::string left = expression.substr(0, gePos);
        std::string right = expression.substr(gePos + 2);
        
        left.erase(0, left.find_first_not_of(" \t"));
        left.erase(left.find_last_not_of(" \t") + 1);
        right.erase(0, right.find_first_not_of(" \t"));
        right.erase(right.find_last_not_of(" \t") + 1);
        
        if (isNumber(left) && isNumber(right)) {
            return (std::stod(left) >= std::stod(right)) ? "true" : "false";
        }
    } else if (expression.find("<=") != std::string::npos) {
        size_t lePos = expression.find("<=");
        std::string left = expression.substr(0, lePos);
        std::string right = expression.substr(lePos + 2);
        
        left.erase(0, left.find_first_not_of(" \t"));
        left.erase(left.find_last_not_of(" \t") + 1);
        right.erase(0, right.find_first_not_of(" \t"));
        right.erase(right.find_last_not_of(" \t") + 1);
        
        if (isNumber(left) && isNumber(right)) {
            return (std::stod(left) <= std::stod(right)) ? "true" : "false";
        }
    } else if (expression.find(">") != std::string::npos) {
        size_t gtPos = expression.find(">");
        std::string left = expression.substr(0, gtPos);
        std::string right = expression.substr(gtPos + 1);
        
        left.erase(0, left.find_first_not_of(" \t"));
        left.erase(left.find_last_not_of(" \t") + 1);
        right.erase(0, right.find_first_not_of(" \t"));
        right.erase(right.find_last_not_of(" \t") + 1);
        
        if (isNumber(left) && isNumber(right)) {
            return (std::stod(left) > std::stod(right)) ? "true" : "false";
        }
    } else if (expression.find("<") != std::string::npos) {
        size_t ltPos = expression.find("<");
        std::string left = expression.substr(0, ltPos);
        std::string right = expression.substr(ltPos + 1);
        
        left.erase(0, left.find_first_not_of(" \t"));
        left.erase(left.find_last_not_of(" \t") + 1);
        right.erase(0, right.find_first_not_of(" \t"));
        right.erase(right.find_last_not_of(" \t") + 1);
        
        if (isNumber(left) && isNumber(right)) {
            return (std::stod(left) < std::stod(right)) ? "true" : "false";
        }
    }
    
    return evaluateAdditiveExpression(expression);
}

std::string CHTLStyleProcessor::evaluateAdditiveExpression(const std::string& expression) const {
    return evaluateArithmeticExpression(expression);
}

std::string CHTLStyleProcessor::evaluateMultiplicativeExpression(const std::string& expression) const {
    return evaluateArithmeticExpression(expression);
}

std::string CHTLStyleProcessor::evaluateUnaryExpression(const std::string& expression) const {
    return evaluateArithmeticExpression(expression);
}

std::string CHTLStyleProcessor::evaluatePrimaryExpression(const std::string& expression) const {
    return evaluateArithmeticExpression(expression);
}

std::string CHTLStyleProcessor::resolvePropertyReference(const std::string& reference) const {
    // 简化的属性引用解析
    if (reference.find('.') != std::string::npos) {
        size_t dotPos = reference.find('.');
        std::string selector = reference.substr(0, dotPos);
        std::string property = reference.substr(dotPos + 1);
        
        return resolveElementProperty(selector, property);
    }
    
    // 直接属性引用
    auto it = elementProperties.find(reference);
    return (it != elementProperties.end()) ? it->second : "";
}

std::string CHTLStyleProcessor::resolveElementProperty(const std::string& selector, const std::string& property) const {
    // 简化的元素属性解析
    if (selector[0] == '#') {
        std::string id = selector.substr(1);
        auto it = elementMap.find(id);
        if (it != elementMap.end()) {
            return it->second->getAttribute(property);
        }
    } else if (selector[0] == '.') {
        std::string className = selector.substr(1);
        // 查找具有指定类名的元素
        for (const auto& pair : elementMap) {
            if (pair.second->hasClass(className)) {
                return pair.second->getAttribute(property);
            }
        }
    } else {
        // 标签选择器
        for (const auto& pair : elementMap) {
            if (pair.second->getTagName() == selector) {
                return pair.second->getAttribute(property);
            }
        }
    }
    
    return "";
}

std::string CHTLStyleProcessor::resolveSelector(const std::string& selector) const {
    // 简化的选择器解析
    if (selector[0] == '.') {
        return processClassSelector(selector.substr(1));
    } else if (selector[0] == '#') {
        return processIdSelector(selector.substr(1));
    } else {
        return processTagSelector(selector);
    }
}

std::string CHTLStyleProcessor::processClassSelector(const std::string& className) const {
    return "." + className;
}

std::string CHTLStyleProcessor::processIdSelector(const std::string& id) const {
    return "#" + id;
}

std::string CHTLStyleProcessor::processTagSelector(const std::string& tagName) const {
    return tagName;
}

std::string CHTLStyleProcessor::processDescendantSelector(const std::string& selector) const {
    return selector;
}

std::string CHTLStyleProcessor::processPseudoClassSelector(const std::string& pseudoClass) const {
    return ":" + pseudoClass;
}

std::string CHTLStyleProcessor::processPseudoElementSelector(const std::string& pseudoElement) const {
    return "::" + pseudoElement;
}

std::string CHTLStyleProcessor::processContextReference(const std::string& context) const {
    return context;
}

std::string CHTLStyleProcessor::processContextDerivation(const std::string& context, std::shared_ptr<ElementNode> element) const {
    // 简化的上下文推导
    if (context == "&") {
        if (!element->getId().empty()) {
            return "#" + element->getId();
        } else if (!element->getAttribute("class").empty()) {
            return "." + element->getAttribute("class");
        } else {
            return element->getTagName();
        }
    }
    
    return context;
}

std::string CHTLStyleProcessor::processAmpersandReference(const std::string& reference, std::shared_ptr<ElementNode> element) const {
    // 简化的&引用处理
    if (reference == "&") {
        return processContextDerivation("&", element);
    } else if (reference.find("&") != std::string::npos) {
        std::string result = reference;
        std::string context = processContextDerivation("&", element);
        size_t pos = 0;
        while ((pos = result.find("&", pos)) != std::string::npos) {
            result.replace(pos, 1, context);
            pos += context.length();
        }
        return result;
    }
    
    return reference;
}

void CHTLStyleProcessor::autoAddClassToElement(std::shared_ptr<ElementNode> element, const std::string& className) const {
    if (!element) {
        return;
    }
    
    element->addClass(className);
}

void CHTLStyleProcessor::autoAddIdToElement(std::shared_ptr<ElementNode> element, const std::string& id) const {
    if (!element) {
        return;
    }
    
    element->setId(id);
}

void CHTLStyleProcessor::processAutoClassAndId(std::shared_ptr<StyleNode> styleNode, std::shared_ptr<ElementNode> element) const {
    if (!styleNode || !element) {
        return;
    }
    
    // 处理类选择器自动化
    for (const auto& rule : styleNode->getStyleRules()) {
        std::string selector = rule->getAttribute("selector");
        if (selector[0] == '.' && element->getAttribute("class").empty()) {
            std::string className = selector.substr(1);
            autoAddClassToElement(element, className);
        } else if (selector[0] == '#' && element->getId().empty()) {
            std::string id = selector.substr(1);
            autoAddIdToElement(element, id);
        }
    }
}

std::string CHTLStyleProcessor::processStyleRule(const std::string& selector, const std::map<std::string, std::string>& properties) const {
    std::ostringstream oss;
    
    // 处理选择器
    std::string processedSelector = resolveSelector(selector);
    
    oss << processedSelector << " {\n";
    
    // 处理属性
    for (const auto& prop : properties) {
        std::string value = prop.second;
        
        // 处理表达式
        if (value.find('+') != std::string::npos || 
            value.find('-') != std::string::npos ||
            value.find('*') != std::string::npos ||
            value.find('/') != std::string::npos) {
            value = evaluateArithmeticExpression(value);
        } else if (value.find('?') != std::string::npos) {
            value = evaluateConditionalExpression(value);
        } else if (value.find('.') != std::string::npos) {
            value = resolvePropertyReference(value);
        }
        
        oss << "    " << prop.first << ": " << value << ";\n";
    }
    
    oss << "}";
    
    return oss.str();
}

std::string CHTLStyleProcessor::processInlineStyle(const std::map<std::string, std::string>& properties) const {
    std::ostringstream oss;
    bool first = true;
    
    for (const auto& prop : properties) {
        if (!first) oss << "; ";
        
        std::string value = prop.second;
        
        // 处理表达式
        if (value.find('+') != std::string::npos || 
            value.find('-') != std::string::npos ||
            value.find('*') != std::string::npos ||
            value.find('/') != std::string::npos) {
            value = evaluateArithmeticExpression(value);
        } else if (value.find('?') != std::string::npos) {
            value = evaluateConditionalExpression(value);
        } else if (value.find('.') != std::string::npos) {
            value = resolvePropertyReference(value);
        }
        
        oss << prop.first << ": " << value;
        first = false;
    }
    
    return oss.str();
}

std::vector<std::string> CHTLStyleProcessor::tokenizeExpression(const std::string& expression) const {
    std::vector<std::string> tokens;
    std::istringstream iss(expression);
    std::string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

bool CHTLStyleProcessor::isOperator(const std::string& token) const {
    return token == "+" || token == "-" || token == "*" || token == "/" || 
           token == "%" || token == "**" || token == "==" || token == "!=" ||
           token == ">" || token == "<" || token == ">=" || token == "<=" ||
           token == "&&" || token == "||";
}

bool CHTLStyleProcessor::isNumber(const std::string& token) const {
    try {
        std::stod(token);
        return true;
    } catch (...) {
        return false;
    }
}

bool CHTLStyleProcessor::isString(const std::string& token) const {
    return token.length() >= 2 && token[0] == '"' && token[token.length() - 1] == '"';
}

bool CHTLStyleProcessor::isIdentifier(const std::string& token) const {
    return !token.empty() && std::isalpha(token[0]) && 
           std::all_of(token.begin(), token.end(), [](char c) {
               return std::isalnum(c) || c == '_' || c == '.';
           });
}

void CHTLStyleProcessor::addError(const std::string& error) {
    errors.push_back(error);
}

void CHTLStyleProcessor::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

void CHTLStyleProcessor::clearMessages() {
    errors.clear();
    warnings.clear();
}

void CHTLStyleProcessor::reset() {
    clearElementProperties();
    if (propertyReference) {
        propertyReference->reset();
    }
    clearMessages();
}

bool CHTLStyleProcessor::validate() const {
    return errors.empty();
}

std::string CHTLStyleProcessor::getDebugInfo() const {
    std::ostringstream oss;
    oss << "CHTL样式处理器调试信息:\n";
    oss << "调试模式: " << (debugMode ? "开启" : "关闭") << "\n";
    oss << "严格模式: " << (strictMode ? "开启" : "关闭") << "\n";
    oss << "自动添加类: " << (autoAddClass ? "开启" : "关闭") << "\n";
    oss << "自动添加ID: " << (autoAddId ? "开启" : "关闭") << "\n";
    oss << "元素属性数: " << elementProperties.size() << "\n";
    oss << "元素映射数: " << elementMap.size() << "\n";
    oss << "错误数: " << errors.size() << "\n";
    oss << "警告数: " << warnings.size() << "\n";
    return oss.str();
}

} // namespace CHTL