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
                        // 处理属性值中的表达式和引用
                        std::string processedValue = processPropertyValue(attr.second);
                        properties[attr.first] = processedValue;
                    }
                }
                
                std::string processedRule = processStyleRule(selector, properties);
                if (!processedRule.empty()) {
                    oss << processedRule << "\n";
                }
            }
        }
        
        // 处理类选择器和ID选择器
        for (const auto& child : styleNode->getChildren()) {
            if (child->getNodeType() == NodeType::ELEMENT) {
                std::string ruleType = child->getAttribute("type");
                if (ruleType == "class" || ruleType == "id" || ruleType == "pseudo") {
                    std::string selector = child->getAttribute("selector");
                    std::map<std::string, std::string> properties;
                    
                    for (const auto& attr : child->getAttributes()) {
                        if (attr.first != "type" && attr.first != "selector") {
                            // 处理属性值中的表达式和引用
                            std::string processedValue = processPropertyValue(attr.second);
                            properties[attr.first] = processedValue;
                        }
                    }
                    
                    std::string processedRule = processStyleRule(selector, properties);
                    if (!processedRule.empty()) {
                        oss << processedRule << "\n";
                    }
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

// 新增的局部样式块处理方法
std::string CHTLStyleProcessor::processPropertyValue(const std::string& value) const {
    if (value.empty()) {
        return value;
    }
    
    std::string result = value;
    
    // 处理属性引用 (selector.property)
    result = processPropertyReferences(result);
    
    // 处理链式调用
    result = processChainedCalls(result);
    
    // 处理可选链式调用
    result = processOptionalChaining(result);
    
    // 处理函数调用
    result = processFunctionCalls(result);
    
    // 处理算术表达式
    result = processArithmeticExpressions(result);
    
    // 处理条件表达式
    result = processConditionalExpressions(result);
    
    // 处理逻辑表达式
    result = processLogicalExpressions(result);
    
    // 处理属性条件表达式
    result = processAttributeConditionalExpressions(result);
    
    // 处理引用属性条件表达式
    result = processReferencePropertyConditionalExpressions(result);
    
    // 处理单位转换
    result = processUnitConversions(result);
    
    // 处理颜色转换
    result = processColorConversions(result);
    
    return result;
}

std::string CHTLStyleProcessor::processPropertyReferences(const std::string& value) const {
    std::string result = value;
    
    // 查找属性引用模式 (selector.property)
    std::regex propertyRefRegex(R"(([a-zA-Z_][a-zA-Z0-9_]*\.[a-zA-Z_][a-zA-Z0-9_]*))");
    std::smatch match;
    
    while (std::regex_search(result, match, propertyRefRegex)) {
        std::string fullMatch = match[0].str();
        std::string selector = match[1].str();
        std::string property = match[2].str();
        
        // 解析选择器和属性
        size_t dotPos = selector.find('.');
        if (dotPos != std::string::npos) {
            std::string selectorName = selector.substr(0, dotPos);
            std::string propertyName = selector.substr(dotPos + 1);
            
            // 查找属性值
            std::string propertyValue = propertyReference->resolveElementProperty(selectorName, propertyName);
            if (propertyValue.empty()) {
                propertyValue = propertyReference->resolveStyleProperty(selectorName, propertyName);
            }
            
            if (!propertyValue.empty()) {
                result.replace(match.position(), match.length(), propertyValue);
            } else {
                // 如果找不到属性，保留原值
                result.replace(match.position(), match.length(), fullMatch);
            }
        }
    }
    
    return result;
}

std::string CHTLStyleProcessor::processArithmeticExpressions(const std::string& value) const {
    std::string result = value;
    
    // 简单的算术表达式处理
    // 支持 + - * / % ** 运算符
    
    // 处理幂运算 (**)
    std::regex powerRegex(R"((\d+(?:\.\d+)?)\s*\*\*\s*(\d+(?:\.\d+)?))");
    std::smatch match;
    
    while (std::regex_search(result, match, powerRegex)) {
        double base = std::stod(match[1].str());
        double exponent = std::stod(match[2].str());
        double resultValue = std::pow(base, exponent);
        
        std::ostringstream oss;
        oss << resultValue;
        result.replace(match.position(), match.length(), oss.str());
    }
    
    // 处理乘除模运算 (* / %)
    std::regex mulDivModRegex(R"((\d+(?:\.\d+)?)\s*([*/%])\s*(\d+(?:\.\d+)?))");
    
    while (std::regex_search(result, match, mulDivModRegex)) {
        double left = std::stod(match[1].str());
        std::string op = match[2].str();
        double right = std::stod(match[3].str());
        
        double resultValue = 0;
        if (op == "*") {
            resultValue = left * right;
        } else if (op == "/") {
            if (right != 0) {
                resultValue = left / right;
            } else {
                resultValue = 0; // 除零处理
            }
        } else if (op == "%") {
            resultValue = std::fmod(left, right);
        }
        
        std::ostringstream oss;
        oss << resultValue;
        result.replace(match.position(), match.length(), oss.str());
    }
    
    // 处理加减运算 (+ -)
    std::regex addSubRegex(R"((\d+(?:\.\d+)?)\s*([+-])\s*(\d+(?:\.\d+)?))");
    
    while (std::regex_search(result, match, addSubRegex)) {
        double left = std::stod(match[1].str());
        std::string op = match[2].str();
        double right = std::stod(match[3].str());
        
        double resultValue = 0;
        if (op == "+") {
            resultValue = left + right;
        } else if (op == "-") {
            resultValue = left - right;
        }
        
        std::ostringstream oss;
        oss << resultValue;
        result.replace(match.position(), match.length(), oss.str());
    }
    
    return result;
}

std::string CHTLStyleProcessor::processConditionalExpressions(const std::string& value) const {
    std::string result = value;
    
    // 处理条件表达式 (condition ? trueValue : falseValue)
    std::regex conditionalRegex(R"(([^?]+)\s*\?\s*([^:]+)\s*:\s*([^?]+))");
    std::smatch match;
    
    while (std::regex_search(result, match, conditionalRegex)) {
        std::string condition = match[1].str();
        std::string trueValue = match[2].str();
        std::string falseValue = match[3].str();
        
        // 简单的条件判断（这里可以扩展更复杂的逻辑）
        bool conditionResult = evaluateCondition(condition);
        
        std::string selectedValue = conditionResult ? trueValue : falseValue;
        result.replace(match.position(), match.length(), selectedValue);
    }
    
    return result;
}

std::string CHTLStyleProcessor::processLogicalExpressions(const std::string& value) const {
    std::string result = value;
    
    // 处理逻辑表达式 (&& ||)
    std::regex logicalRegex(R"(([^&|]+)\s*(&&|\|\|)\s*([^&|]+))");
    std::smatch match;
    
    while (std::regex_search(result, match, logicalRegex)) {
        std::string left = match[1].str();
        std::string op = match[2].str();
        std::string right = match[3].str();
        
        bool leftResult = evaluateCondition(left);
        bool rightResult = evaluateCondition(right);
        
        bool resultValue = false;
        if (op == "&&") {
            resultValue = leftResult && rightResult;
        } else if (op == "||") {
            resultValue = leftResult || rightResult;
        }
        
        std::ostringstream oss;
        oss << (resultValue ? "true" : "false");
        result.replace(match.position(), match.length(), oss.str());
    }
    
    return result;
}

bool CHTLStyleProcessor::evaluateCondition(const std::string& condition) const {
    // 简单的条件求值
    // 这里可以扩展更复杂的条件逻辑
    
    // 去除空白
    std::string trimmed = condition;
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
    
    // 检查是否为数字
    if (std::all_of(trimmed.begin(), trimmed.end(), ::isdigit)) {
        return std::stoi(trimmed) != 0;
    }
    
    // 检查是否为布尔值
    if (trimmed == "true" || trimmed == "1") {
        return true;
    }
    if (trimmed == "false" || trimmed == "0") {
        return false;
    }
    
    // 检查比较表达式
    std::regex comparisonRegex(R"((\d+(?:\.\d+)?)\s*(==|!=|<|>|<=|>=)\s*(\d+(?:\.\d+)?))");
    std::smatch match;
    
    if (std::regex_search(trimmed, match, comparisonRegex)) {
        double left = std::stod(match[1].str());
        std::string op = match[2].str();
        double right = std::stod(match[3].str());
        
        if (op == "==") return left == right;
        if (op == "!=") return left != right;
        if (op == "<") return left < right;
        if (op == ">") return left > right;
        if (op == "<=") return left <= right;
        if (op == ">=") return left >= right;
    }
    
    // 默认返回false
    return false;
}

// 增强的局部样式块功能实现
std::string CHTLStyleProcessor::processChainedCalls(const std::string& value) const {
    std::string result = value;
    
    // 处理链式调用 (element.property.method())
    std::regex chainedCallRegex(R"(([a-zA-Z_][a-zA-Z0-9_]*\.[a-zA-Z_][a-zA-Z0-9_]*\.[a-zA-Z_][a-zA-Z0-9_]*\([^)]*\)))");
    std::smatch match;
    
    while (std::regex_search(result, match, chainedCallRegex)) {
        std::string fullMatch = match[0].str();
        std::string processedCall = processChainedCall(fullMatch);
        result.replace(match.position(), match.length(), processedCall);
    }
    
    return result;
}

std::string CHTLStyleProcessor::processOptionalChaining(const std::string& value) const {
    std::string result = value;
    
    // 处理可选链式调用 (element?.property?.method())
    std::regex optionalChainingRegex(R"(([a-zA-Z_][a-zA-Z0-9_]*\?\.([a-zA-Z_][a-zA-Z0-9_]*\?\.)*[a-zA-Z_][a-zA-Z0-9_]*))");
    std::smatch match;
    
    while (std::regex_search(result, match, optionalChainingRegex)) {
        std::string fullMatch = match[0].str();
        std::string processedCall = processOptionalChaining(fullMatch);
        result.replace(match.position(), match.length(), processedCall);
    }
    
    return result;
}

std::string CHTLStyleProcessor::processFunctionCalls(const std::string& value) const {
    std::string result = value;
    
    // 处理函数调用 (function(args))
    std::regex functionCallRegex(R"(([a-zA-Z_][a-zA-Z0-9_]*)\s*\(([^)]*)\))");
    std::smatch match;
    
    while (std::regex_search(result, match, functionCallRegex)) {
        std::string functionName = match[1].str();
        std::string args = match[2].str();
        
        // 解析参数
        std::vector<std::string> arguments = parseFunctionCall(args);
        
        // 处理不同类型的函数
        std::string processedCall;
        if (functionName.find("math.") == 0) {
            processedCall = processMathFunction(functionName.substr(5), arguments);
        } else if (functionName.find("string.") == 0) {
            processedCall = processStringFunction(functionName.substr(7), arguments);
        } else if (functionName.find("color.") == 0) {
            processedCall = processColorFunction(functionName.substr(6), arguments);
        } else if (functionName.find("unit.") == 0) {
            processedCall = processUnitFunction(functionName.substr(5), arguments);
        } else {
            // 默认函数处理
            processedCall = fullMatch;
        }
        
        result.replace(match.position(), match.length(), processedCall);
    }
    
    return result;
}

std::string CHTLStyleProcessor::processUnitConversions(const std::string& value) const {
    std::string result = value;
    
    // 处理单位转换 (value unit -> targetUnit)
    std::regex unitConversionRegex(R"((\d+(?:\.\d+)?)\s*([a-zA-Z%]+)\s*->\s*([a-zA-Z%]+))");
    std::smatch match;
    
    while (std::regex_search(result, match, unitConversionRegex)) {
        double value = std::stod(match[1].str());
        std::string fromUnit = match[2].str();
        std::string toUnit = match[3].str();
        
        std::string convertedValue = convertUnit(std::to_string(value), fromUnit, toUnit);
        result.replace(match.position(), match.length(), convertedValue);
    }
    
    return result;
}

std::string CHTLStyleProcessor::processColorConversions(const std::string& value) const {
    std::string result = value;
    
    // 处理颜色转换 (color -> format)
    std::regex colorConversionRegex(R"((#[0-9a-fA-F]{3,6}|rgb\([^)]+\)|hsl\([^)]+\))\s*->\s*([a-zA-Z]+))");
    std::smatch match;
    
    while (std::regex_search(result, match, colorConversionRegex)) {
        std::string color = match[1].str();
        std::string format = match[2].str();
        
        std::string convertedColor = convertColorFormat(color);
        result.replace(match.position(), match.length(), convertedColor);
    }
    
    return result;
}

std::string CHTLStyleProcessor::processAttributeConditionalExpressions(const std::string& value) const {
    std::string result = value;
    
    // 处理属性条件表达式 (attr == value ? trueValue : falseValue)
    std::regex attributeConditionalRegex(R"(([a-zA-Z_][a-zA-Z0-9_]*)\s*(==|!=|<|>|<=|>=)\s*([^?]+)\s*\?\s*([^:]+)\s*:\s*([^?]+))");
    std::smatch match;
    
    while (std::regex_search(result, match, attributeConditionalRegex)) {
        std::string attribute = match[1].str();
        std::string operator_ = match[2].str();
        std::string compareValue = match[3].str();
        std::string trueValue = match[4].str();
        std::string falseValue = match[5].str();
        
        // 获取属性值
        std::string attributeValue = elementProperties.count(attribute) ? elementProperties.at(attribute) : "";
        
        // 比较属性值
        bool conditionResult = false;
        if (operator_ == "==") {
            conditionResult = (attributeValue == compareValue);
        } else if (operator_ == "!=") {
            conditionResult = (attributeValue != compareValue);
        } else if (operator_ == "<") {
            conditionResult = (std::stod(attributeValue) < std::stod(compareValue));
        } else if (operator_ == ">") {
            conditionResult = (std::stod(attributeValue) > std::stod(compareValue));
        } else if (operator_ == "<=") {
            conditionResult = (std::stod(attributeValue) <= std::stod(compareValue));
        } else if (operator_ == ">=") {
            conditionResult = (std::stod(attributeValue) >= std::stod(compareValue));
        }
        
        std::string selectedValue = conditionResult ? trueValue : falseValue;
        result.replace(match.position(), match.length(), selectedValue);
    }
    
    return result;
}

std::string CHTLStyleProcessor::processReferencePropertyConditionalExpressions(const std::string& value) const {
    std::string result = value;
    
    // 处理引用属性条件表达式 (selector.property == value ? trueValue : falseValue)
    std::regex referencePropertyConditionalRegex(R"(([a-zA-Z_][a-zA-Z0-9_]*\.[a-zA-Z_][a-zA-Z0-9_]*)\s*(==|!=|<|>|<=|>=)\s*([^?]+)\s*\?\s*([^:]+)\s*:\s*([^?]+))");
    std::smatch match;
    
    while (std::regex_search(result, match, referencePropertyConditionalRegex)) {
        std::string reference = match[1].str();
        std::string operator_ = match[2].str();
        std::string compareValue = match[3].str();
        std::string trueValue = match[4].str();
        std::string falseValue = match[5].str();
        
        // 解析引用
        std::string propertyValue = resolvePropertyReference(reference);
        
        // 比较属性值
        bool conditionResult = false;
        if (operator_ == "==") {
            conditionResult = (propertyValue == compareValue);
        } else if (operator_ == "!=") {
            conditionResult = (propertyValue != compareValue);
        } else if (operator_ == "<") {
            conditionResult = (std::stod(propertyValue) < std::stod(compareValue));
        } else if (operator_ == ">") {
            conditionResult = (std::stod(propertyValue) > std::stod(compareValue));
        } else if (operator_ == "<=") {
            conditionResult = (std::stod(propertyValue) <= std::stod(compareValue));
        } else if (operator_ == ">=") {
            conditionResult = (std::stod(propertyValue) >= std::stod(compareValue));
        }
        
        std::string selectedValue = conditionResult ? trueValue : falseValue;
        result.replace(match.position(), match.length(), selectedValue);
    }
    
    return result;
}

// 函数处理实现
std::string CHTLStyleProcessor::processMathFunction(const std::string& function, const std::vector<std::string>& args) const {
    if (args.empty()) {
        return "0";
    }
    
    if (function == "abs") {
        return std::to_string(std::abs(std::stod(args[0])));
    } else if (function == "ceil") {
        return std::to_string(std::ceil(std::stod(args[0])));
    } else if (function == "floor") {
        return std::to_string(std::floor(std::stod(args[0])));
    } else if (function == "round") {
        return std::to_string(std::round(std::stod(args[0])));
    } else if (function == "max" && args.size() >= 2) {
        return std::to_string(std::max(std::stod(args[0]), std::stod(args[1])));
    } else if (function == "min" && args.size() >= 2) {
        return std::to_string(std::min(std::stod(args[0]), std::stod(args[1])));
    } else if (function == "pow" && args.size() >= 2) {
        return std::to_string(std::pow(std::stod(args[0]), std::stod(args[1])));
    } else if (function == "sqrt") {
        return std::to_string(std::sqrt(std::stod(args[0])));
    }
    
    return "0";
}

std::string CHTLStyleProcessor::processStringFunction(const std::string& function, const std::vector<std::string>& args) const {
    if (args.empty()) {
        return "";
    }
    
    if (function == "upper") {
        std::string result = args[0];
        std::transform(result.begin(), result.end(), result.begin(), ::toupper);
        return result;
    } else if (function == "lower") {
        std::string result = args[0];
        std::transform(result.begin(), result.end(), result.begin(), ::tolower);
        return result;
    } else if (function == "length") {
        return std::to_string(args[0].length());
    } else if (function == "substr" && args.size() >= 2) {
        size_t start = std::stoul(args[1]);
        if (args.size() >= 3) {
            size_t length = std::stoul(args[2]);
            return args[0].substr(start, length);
        } else {
            return args[0].substr(start);
        }
    }
    
    return args[0];
}

std::string CHTLStyleProcessor::processColorFunction(const std::string& function, const std::vector<std::string>& args) const {
    if (args.empty()) {
        return "#000000";
    }
    
    if (function == "rgb" && args.size() >= 3) {
        int r = std::stoi(args[0]);
        int g = std::stoi(args[1]);
        int b = std::stoi(args[2]);
        std::ostringstream oss;
        oss << "rgb(" << r << "," << g << "," << b << ")";
        return oss.str();
    } else if (function == "hsl" && args.size() >= 3) {
        int h = std::stoi(args[0]);
        int s = std::stoi(args[1]);
        int l = std::stoi(args[2]);
        std::ostringstream oss;
        oss << "hsl(" << h << "," << s << "%," << l << "%)";
        return oss.str();
    } else if (function == "hex") {
        return args[0];
    }
    
    return args[0];
}

std::string CHTLStyleProcessor::processUnitFunction(const std::string& function, const std::vector<std::string>& args) const {
    if (args.empty()) {
        return "0px";
    }
    
    if (function == "px" && args.size() >= 2) {
        double value = std::stod(args[0]);
        std::string fromUnit = args[1];
        return convertUnit(std::to_string(value), fromUnit, "px");
    } else if (function == "em" && args.size() >= 2) {
        double value = std::stod(args[0]);
        std::string fromUnit = args[1];
        return convertUnit(std::to_string(value), fromUnit, "em");
    } else if (function == "rem" && args.size() >= 2) {
        double value = std::stod(args[0]);
        std::string fromUnit = args[1];
        return convertUnit(std::to_string(value), fromUnit, "rem");
    } else if (function == "percent" && args.size() >= 2) {
        double value = std::stod(args[0]);
        std::string fromUnit = args[1];
        return convertUnit(std::to_string(value), fromUnit, "%");
    }
    
    return args[0];
}

// 表达式解析增强
std::vector<std::string> CHTLStyleProcessor::parseFunctionCall(const std::string& expression) const {
    std::vector<std::string> args;
    std::istringstream iss(expression);
    std::string arg;
    
    while (std::getline(iss, arg, ',')) {
        // 去除空白
        arg.erase(0, arg.find_first_not_of(" \t"));
        arg.erase(arg.find_last_not_of(" \t") + 1);
        args.push_back(arg);
    }
    
    return args;
}

std::vector<std::string> CHTLStyleProcessor::parseChainedCall(const std::string& expression) const {
    std::vector<std::string> chain;
    std::istringstream iss(expression);
    std::string part;
    
    while (std::getline(iss, part, '.')) {
        // 去除空白
        part.erase(0, part.find_first_not_of(" \t"));
        part.erase(part.find_last_not_of(" \t") + 1);
        chain.push_back(part);
    }
    
    return chain;
}

std::vector<std::string> CHTLStyleProcessor::parseOptionalChaining(const std::string& expression) const {
    std::vector<std::string> chain;
    std::istringstream iss(expression);
    std::string part;
    
    while (std::getline(iss, part, '?')) {
        // 去除空白和点
        part.erase(0, part.find_first_not_of(" \t."));
        part.erase(part.find_last_not_of(" \t.") + 1);
        if (!part.empty()) {
            chain.push_back(part);
        }
    }
    
    return chain;
}

} // namespace CHTL