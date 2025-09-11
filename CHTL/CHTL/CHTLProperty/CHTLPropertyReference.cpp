#include "CHTLPropertyReference.h"
#include <sstream>
#include <algorithm>
#include <regex>
#include <cmath>
#include <functional>

namespace CHTL {

CHTLPropertyReference::CHTLPropertyReference() 
    : debugMode(false), strictMode(false), enableChaining(true), enableOptionalChaining(true) {
}

void CHTLPropertyReference::registerElement(const std::string& id, std::shared_ptr<ElementNode> element) {
    if (element) {
        elementMap[id] = element;
        
        // 收集元素属性
        std::map<std::string, std::string> properties;
        properties["tagName"] = element->getTagName();
        properties["id"] = element->getId();
        properties["class"] = element->getAttribute("class");
        properties["style"] = element->getStyle();
        
        // 收集所有属性
        for (const auto& attr : element->getAttributes()) {
            properties[attr.first] = attr.second;
        }
        
        elementProperties[id] = properties;
    }
}

void CHTLPropertyReference::registerStyle(const std::string& selector, std::shared_ptr<StyleNode> style) {
    if (style) {
        styleMap[selector] = style;
        
        // 收集样式属性
        std::map<std::string, std::string> properties;
        for (const auto& prop : style->getStyleProperties()) {
            properties[prop.first] = prop.second;
        }
        
        styleProperties[selector] = properties;
    }
}

void CHTLPropertyReference::unregisterElement(const std::string& id) {
    elementMap.erase(id);
    elementProperties.erase(id);
}

void CHTLPropertyReference::unregisterStyle(const std::string& selector) {
    styleMap.erase(selector);
    styleProperties.erase(selector);
}

std::string CHTLPropertyReference::resolvePropertyReference(const std::string& reference) const {
    if (reference.empty()) {
        return "";
    }
    
    try {
        // 检查是否是链式调用
        if (enableChaining && reference.find('.') != std::string::npos) {
            return resolveChainedCall(reference);
        }
        
        // 检查是否是可选链式调用
        if (enableOptionalChaining && reference.find("?.") != std::string::npos) {
            return resolveOptionalChaining(reference);
        }
        
        // 检查是否是表达式
        if (reference.find('+') != std::string::npos || 
            reference.find('-') != std::string::npos ||
            reference.find('*') != std::string::npos ||
            reference.find('/') != std::string::npos ||
            reference.find('?') != std::string::npos) {
            return evaluateExpression(reference);
        }
        
        // 直接属性引用
        return reference;
        
    } catch (const std::exception& e) {
        const_cast<CHTLPropertyReference*>(this)->addError("解析属性引用时发生错误: " + std::string(e.what()));
        return reference;
    }
}

std::string CHTLPropertyReference::resolveElementProperty(const std::string& selector, const std::string& property) const {
    if (selector.empty() || property.empty()) {
        return "";
    }
    
    try {
        // 查找元素
        auto element = findElement(selector);
        if (!element) {
            if (strictMode) {
                const_cast<CHTLPropertyReference*>(this)->addError("未找到选择器对应的元素: " + selector);
            }
            return "";
        }
        
        // 获取属性值
        std::string value = element->getAttribute(property);
        if (value.empty()) {
            // 尝试获取样式属性
            value = element->getStyleProperty(property);
        }
        
        return value;
        
    } catch (const std::exception& e) {
        const_cast<CHTLPropertyReference*>(this)->addError("解析元素属性时发生错误: " + std::string(e.what()));
        return "";
    }
}

std::string CHTLPropertyReference::resolveStyleProperty(const std::string& selector, const std::string& property) const {
    if (selector.empty() || property.empty()) {
        return "";
    }
    
    try {
        // 查找样式
        auto style = findStyle(selector);
        if (!style) {
            if (strictMode) {
                const_cast<CHTLPropertyReference*>(this)->addError("未找到选择器对应的样式: " + selector);
            }
            return "";
        }
        
        // 获取样式属性
        return style->getStyleProperty(property);
        
    } catch (const std::exception& e) {
        const_cast<CHTLPropertyReference*>(this)->addError("解析样式属性时发生错误: " + std::string(e.what()));
        return "";
    }
}

std::string CHTLPropertyReference::resolveComputedProperty(const std::string& selector, const std::string& property) const {
    if (selector.empty() || property.empty()) {
        return "";
    }
    
    try {
        // 首先尝试从元素获取
        std::string value = resolveElementProperty(selector, property);
        if (!value.empty()) {
            return value;
        }
        
        // 然后尝试从样式获取
        value = resolveStyleProperty(selector, property);
        if (!value.empty()) {
            return value;
        }
        
        // 最后尝试从计算属性获取
        auto it = elementProperties.find(selector);
        if (it != elementProperties.end()) {
            auto propIt = it->second.find(property);
            if (propIt != it->second.end()) {
                return propIt->second;
            }
        }
        
        return "";
        
    } catch (const std::exception& e) {
        const_cast<CHTLPropertyReference*>(this)->addError("解析计算属性时发生错误: " + std::string(e.what()));
        return "";
    }
}

std::string CHTLPropertyReference::resolveChainedCall(const std::string& chain) const {
    if (chain.empty()) {
        return "";
    }
    
    try {
        std::vector<std::string> parts;
        std::istringstream iss(chain);
        std::string part;
        
        while (std::getline(iss, part, '.')) {
            parts.push_back(part);
        }
        
        if (parts.empty()) {
            return chain;
        }
        
        std::string result = parts[0];
        
        for (size_t i = 1; i < parts.size(); ++i) {
            const std::string& part = parts[i];
            
            // 检查是否是方法调用
            if (part.find('(') != std::string::npos) {
                size_t parenPos = part.find('(');
                std::string method = part.substr(0, parenPos);
                std::string args = part.substr(parenPos + 1);
                
                // 移除右括号
                if (!args.empty() && args.back() == ')') {
                    args.pop_back();
                }
                
                result = processMethodCall(method, args);
            } else {
                // 属性访问
                result = resolvePropertyReference(result + "." + part);
            }
        }
        
        return result;
        
    } catch (const std::exception& e) {
        const_cast<CHTLPropertyReference*>(this)->addError("解析链式调用时发生错误: " + std::string(e.what()));
        return chain;
    }
}

std::string CHTLPropertyReference::resolveOptionalChaining(const std::string& chain) const {
    if (chain.empty()) {
        return "";
    }
    
    try {
        std::vector<std::string> parts;
        std::istringstream iss(chain);
        std::string part;
        
        while (std::getline(iss, part, '?')) {
            if (!part.empty()) {
                parts.push_back(part);
            }
        }
        
        if (parts.empty()) {
            return chain;
        }
        
        std::string result = parts[0];
        
        for (size_t i = 1; i < parts.size(); ++i) {
            const std::string& part = parts[i];
            
            // 检查是否是方法调用
            if (part.find('(') != std::string::npos) {
                size_t parenPos = part.find('(');
                std::string method = part.substr(0, parenPos);
                std::string args = part.substr(parenPos + 1);
                
                // 移除右括号
                if (!args.empty() && args.back() == ')') {
                    args.pop_back();
                }
                
                result = processMethodCall(method, args);
            } else {
                // 属性访问
                std::string temp = resolvePropertyReference(result + "." + part);
                if (temp.empty()) {
                    return ""; // 可选链式调用，如果属性不存在则返回空
                }
                result = temp;
            }
        }
        
        return result;
        
    } catch (const std::exception& e) {
        const_cast<CHTLPropertyReference*>(this)->addError("解析可选链式调用时发生错误: " + std::string(e.what()));
        return chain;
    }
}

std::string CHTLPropertyReference::evaluateExpression(const std::string& expression) const {
    if (expression.empty()) {
        return "";
    }
    
    try {
        // 检查是否是条件表达式
        if (expression.find('?') != std::string::npos) {
            return evaluateConditionalExpression(expression);
        }
        
        // 检查是否是逻辑表达式
        if (expression.find("&&") != std::string::npos || 
            expression.find("||") != std::string::npos) {
            return evaluateLogicalExpression(expression);
        }
        
        // 检查是否是算术表达式
        if (expression.find('+') != std::string::npos || 
            expression.find('-') != std::string::npos ||
            expression.find('*') != std::string::npos ||
            expression.find('/') != std::string::npos) {
            return evaluateArithmeticExpression(expression);
        }
        
        return expression;
        
    } catch (const std::exception& e) {
        const_cast<CHTLPropertyReference*>(this)->addError("求值表达式时发生错误: " + std::string(e.what()));
        return expression;
    }
}

std::string CHTLPropertyReference::evaluateArithmeticExpression(const std::string& expression) const {
    std::vector<std::string> tokens = tokenizeExpression(expression);
    
    if (tokens.empty()) {
        return expression;
    }
    
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
                    const_cast<CHTLPropertyReference*>(this)->addError("除零错误");
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
                        const_cast<CHTLPropertyReference*>(this)->addError("除零错误");
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

std::string CHTLPropertyReference::evaluateConditionalExpression(const std::string& expression) const {
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

std::string CHTLPropertyReference::evaluateLogicalExpression(const std::string& expression) const {
    if (expression.find("&&") != std::string::npos) {
        size_t andPos = expression.find("&&");
        std::string left = expression.substr(0, andPos);
        std::string right = expression.substr(andPos + 2);
        
        left.erase(0, left.find_first_not_of(" \t"));
        left.erase(left.find_last_not_of(" \t") + 1);
        right.erase(0, right.find_first_not_of(" \t"));
        right.erase(right.find_last_not_of(" \t") + 1);
        
        bool leftResult = evaluateLogicalExpression(left) == "true";
        bool rightResult = evaluateLogicalExpression(right) == "true";
        
        return (leftResult && rightResult) ? "true" : "false";
    } else if (expression.find("||") != std::string::npos) {
        size_t orPos = expression.find("||");
        std::string left = expression.substr(0, orPos);
        std::string right = expression.substr(orPos + 2);
        
        left.erase(0, left.find_first_not_of(" \t"));
        left.erase(left.find_last_not_of(" \t") + 1);
        right.erase(0, right.find_first_not_of(" \t"));
        right.erase(right.find_last_not_of(" \t") + 1);
        
        bool leftResult = evaluateLogicalExpression(left) == "true";
        bool rightResult = evaluateLogicalExpression(right) == "true";
        
        return (leftResult || rightResult) ? "true" : "false";
    } else {
        // 检查相等性
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
            // 检查关系
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
        }
    }
    
    return expression;
}

std::string CHTLPropertyReference::processMethodCall(const std::string& method, const std::string& args) const {
    if (method.empty()) {
        return "";
    }
    
    try {
        // 解析参数
        std::vector<std::string> arguments;
        if (!args.empty()) {
            std::istringstream iss(args);
            std::string arg;
            while (std::getline(iss, arg, ',')) {
                arg.erase(0, arg.find_first_not_of(" \t"));
                arg.erase(arg.find_last_not_of(" \t") + 1);
                arguments.push_back(arg);
            }
        }
        
        // 处理不同的方法
        if (method == "toString") {
            return convertToString(arguments.empty() ? "" : arguments[0]);
        } else if (method == "toNumber") {
            return std::to_string(convertToNumber(arguments.empty() ? "" : arguments[0]));
        } else if (method == "toBoolean") {
            return convertToBoolean(arguments.empty() ? "" : arguments[0]) ? "true" : "false";
        } else if (method == "toUpperCase") {
            std::string str = arguments.empty() ? "" : arguments[0];
            std::transform(str.begin(), str.end(), str.begin(), ::toupper);
            return str;
        } else if (method == "toLowerCase") {
            std::string str = arguments.empty() ? "" : arguments[0];
            std::transform(str.begin(), str.end(), str.begin(), ::tolower);
            return str;
        } else if (method == "trim") {
            std::string str = arguments.empty() ? "" : arguments[0];
            str.erase(0, str.find_first_not_of(" \t"));
            str.erase(str.find_last_not_of(" \t") + 1);
            return str;
        } else if (method == "substring") {
            if (arguments.size() >= 2) {
                std::string str = arguments[0];
                int start = std::stoi(arguments[1]);
                int end = arguments.size() >= 3 ? std::stoi(arguments[2]) : str.length();
                return str.substr(start, end - start);
            }
        } else if (method == "length") {
            return std::to_string(arguments.empty() ? 0 : arguments[0].length());
        } else if (method == "indexOf") {
            if (arguments.size() >= 2) {
                std::string str = arguments[0];
                std::string search = arguments[1];
                size_t pos = str.find(search);
                return pos != std::string::npos ? std::to_string(pos) : "-1";
            }
        } else if (method == "replace") {
            if (arguments.size() >= 3) {
                std::string str = arguments[0];
                std::string search = arguments[1];
                std::string replace = arguments[2];
                size_t pos = 0;
                while ((pos = str.find(search, pos)) != std::string::npos) {
                    str.replace(pos, search.length(), replace);
                    pos += replace.length();
                }
                return str;
            }
        }
        
        // 数学函数
        if (method == "abs") {
            if (!arguments.empty()) {
                return std::to_string(std::abs(convertToNumber(arguments[0])));
            }
        } else if (method == "ceil") {
            if (!arguments.empty()) {
                return std::to_string(std::ceil(convertToNumber(arguments[0])));
            }
        } else if (method == "floor") {
            if (!arguments.empty()) {
                return std::to_string(std::floor(convertToNumber(arguments[0])));
            }
        } else if (method == "round") {
            if (!arguments.empty()) {
                return std::to_string(std::round(convertToNumber(arguments[0])));
            }
        } else if (method == "sqrt") {
            if (!arguments.empty()) {
                return std::to_string(std::sqrt(convertToNumber(arguments[0])));
            }
        } else if (method == "pow") {
            if (arguments.size() >= 2) {
                return std::to_string(std::pow(convertToNumber(arguments[0]), convertToNumber(arguments[1])));
            }
        } else if (method == "min") {
            if (!arguments.empty()) {
                double minVal = convertToNumber(arguments[0]);
                for (size_t i = 1; i < arguments.size(); ++i) {
                    minVal = std::min(minVal, convertToNumber(arguments[i]));
                }
                return std::to_string(minVal);
            }
        } else if (method == "max") {
            if (!arguments.empty()) {
                double maxVal = convertToNumber(arguments[0]);
                for (size_t i = 1; i < arguments.size(); ++i) {
                    maxVal = std::max(maxVal, convertToNumber(arguments[i]));
                }
                return std::to_string(maxVal);
            }
        }
        
        return "";
        
    } catch (const std::exception& e) {
        const_cast<CHTLPropertyReference*>(this)->addError("处理方法调用时发生错误: " + std::string(e.what()));
        return "";
    }
}

std::shared_ptr<ElementNode> CHTLPropertyReference::findElement(const std::string& selector) const {
    if (selector.empty()) {
        return nullptr;
    }
    
    // 直接ID查找
    if (selector[0] == '#') {
        std::string id = selector.substr(1);
        auto it = elementMap.find(id);
        return (it != elementMap.end()) ? it->second : nullptr;
    }
    
    // 遍历所有元素查找匹配的
    for (const auto& pair : elementMap) {
        if (matchesElement(selector, pair.second)) {
            return pair.second;
        }
    }
    
    return nullptr;
}

std::vector<std::shared_ptr<ElementNode>> CHTLPropertyReference::findElements(const std::string& selector) const {
    std::vector<std::shared_ptr<ElementNode>> elements;
    
    if (selector.empty()) {
        return elements;
    }
    
    for (const auto& pair : elementMap) {
        if (matchesElement(selector, pair.second)) {
            elements.push_back(pair.second);
        }
    }
    
    return elements;
}

std::shared_ptr<StyleNode> CHTLPropertyReference::findStyle(const std::string& selector) const {
    if (selector.empty()) {
        return nullptr;
    }
    
    auto it = styleMap.find(selector);
    return (it != styleMap.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<StyleNode>> CHTLPropertyReference::findStyles(const std::string& selector) const {
    std::vector<std::shared_ptr<StyleNode>> styles;
    
    if (selector.empty()) {
        return styles;
    }
    
    for (const auto& pair : styleMap) {
        if (pair.first.find(selector) != std::string::npos) {
            styles.push_back(pair.second);
        }
    }
    
    return styles;
}

bool CHTLPropertyReference::matchesElement(const std::string& selector, std::shared_ptr<ElementNode> element) const {
    if (!element) {
        return false;
    }
    
    if (isIdSelector(selector)) {
        return matchesIdSelector(selector.substr(1), element);
    } else if (isClassSelector(selector)) {
        return matchesClassSelector(selector.substr(1), element);
    } else if (isTagSelector(selector)) {
        return matchesTagSelector(selector, element);
    } else if (isAttributeSelector(selector)) {
        return matchesAttributeSelector(selector, element);
    } else if (isPseudoSelector(selector)) {
        return matchesPseudoSelector(selector, element);
    }
    
    return false;
}

bool CHTLPropertyReference::matchesIdSelector(const std::string& id, std::shared_ptr<ElementNode> element) const {
    return element->getId() == id;
}

bool CHTLPropertyReference::matchesClassSelector(const std::string& className, std::shared_ptr<ElementNode> element) const {
    return element->hasClass(className);
}

bool CHTLPropertyReference::matchesTagSelector(const std::string& tagName, std::shared_ptr<ElementNode> element) const {
    return element->getTagName() == tagName;
}

bool CHTLPropertyReference::matchesAttributeSelector(const std::string& attribute, std::shared_ptr<ElementNode> element) const {
    // 简化的属性选择器匹配
    if (attribute[0] == '[' && attribute.back() == ']') {
        std::string attr = attribute.substr(1, attribute.length() - 2);
        return element->hasAttribute(attr);
    }
    return false;
}

bool CHTLPropertyReference::matchesPseudoSelector(const std::string& pseudo, std::shared_ptr<ElementNode> element) const {
    // 简化的伪选择器匹配
    if (pseudo[0] == ':') {
        // 这里可以实现更复杂的伪选择器逻辑
        return true;
    }
    return false;
}

bool CHTLPropertyReference::isIdSelector(const std::string& selector) const {
    return selector[0] == '#';
}

bool CHTLPropertyReference::isClassSelector(const std::string& selector) const {
    return selector[0] == '.';
}

bool CHTLPropertyReference::isTagSelector(const std::string& selector) const {
    return std::isalpha(selector[0]) && selector.find_first_of(".#[:") == std::string::npos;
}

bool CHTLPropertyReference::isAttributeSelector(const std::string& selector) const {
    return selector[0] == '[' && selector.back() == ']';
}

bool CHTLPropertyReference::isPseudoSelector(const std::string& selector) const {
    return selector[0] == ':';
}

bool CHTLPropertyReference::isDescendantSelector(const std::string& selector) const {
    return selector.find(' ') != std::string::npos;
}

bool CHTLPropertyReference::isChildSelector(const std::string& selector) const {
    return selector.find('>') != std::string::npos;
}

bool CHTLPropertyReference::isSiblingSelector(const std::string& selector) const {
    return selector.find('+') != std::string::npos || selector.find('~') != std::string::npos;
}

std::vector<std::string> CHTLPropertyReference::tokenizeExpression(const std::string& expression) const {
    std::vector<std::string> tokens;
    std::istringstream iss(expression);
    std::string token;
    
    while (iss >> token) {
        tokens.push_back(token);
    }
    
    return tokens;
}

bool CHTLPropertyReference::isOperator(const std::string& token) const {
    return token == "+" || token == "-" || token == "*" || token == "/" || 
           token == "%" || token == "**" || token == "==" || token == "!=" ||
           token == ">" || token == "<" || token == ">=" || token == "<=" ||
           token == "&&" || token == "||";
}

bool CHTLPropertyReference::isNumber(const std::string& token) const {
    try {
        std::stod(token);
        return true;
    } catch (...) {
        return false;
    }
}

bool CHTLPropertyReference::isString(const std::string& token) const {
    return token.length() >= 2 && token[0] == '"' && token[token.length() - 1] == '"';
}

bool CHTLPropertyReference::isIdentifier(const std::string& token) const {
    return !token.empty() && std::isalpha(token[0]) && 
           std::all_of(token.begin(), token.end(), [](char c) {
               return std::isalnum(c) || c == '_' || c == '.';
           });
}

bool CHTLPropertyReference::isFunction(const std::string& token) const {
    return token.find('(') != std::string::npos;
}

std::string CHTLPropertyReference::convertToString(const std::string& value) const {
    return value;
}

double CHTLPropertyReference::convertToNumber(const std::string& value) const {
    try {
        return std::stod(value);
    } catch (...) {
        return 0.0;
    }
}

bool CHTLPropertyReference::convertToBoolean(const std::string& value) const {
    return value == "true" || value == "1" || value == "yes" || value == "on";
}

void CHTLPropertyReference::setElementProperty(const std::string& selector, const std::string& property, const std::string& value) {
    auto element = findElement(selector);
    if (element) {
        element->setAttribute(property, value);
        
        // 更新属性映射
        auto it = elementProperties.find(selector);
        if (it != elementProperties.end()) {
            it->second[property] = value;
        }
    }
}

void CHTLPropertyReference::setStyleProperty(const std::string& selector, const std::string& property, const std::string& value) {
    auto style = findStyle(selector);
    if (style) {
        style->setStyleProperty(property, value);
        
        // 更新属性映射
        auto it = styleProperties.find(selector);
        if (it != styleProperties.end()) {
            it->second[property] = value;
        }
    }
}

std::string CHTLPropertyReference::getElementProperty(const std::string& selector, const std::string& property) const {
    return resolveElementProperty(selector, property);
}

std::string CHTLPropertyReference::getStyleProperty(const std::string& selector, const std::string& property) const {
    return resolveStyleProperty(selector, property);
}

void CHTLPropertyReference::setElementProperties(const std::string& selector, const std::map<std::string, std::string>& properties) {
    for (const auto& prop : properties) {
        setElementProperty(selector, prop.first, prop.second);
    }
}

void CHTLPropertyReference::setStyleProperties(const std::string& selector, const std::map<std::string, std::string>& properties) {
    for (const auto& prop : properties) {
        setStyleProperty(selector, prop.first, prop.second);
    }
}

std::map<std::string, std::string> CHTLPropertyReference::getAllElementProperties(const std::string& selector) const {
    auto it = elementProperties.find(selector);
    return (it != elementProperties.end()) ? it->second : std::map<std::string, std::string>();
}

std::map<std::string, std::string> CHTLPropertyReference::getAllStyleProperties(const std::string& selector) const {
    auto it = styleProperties.find(selector);
    return (it != styleProperties.end()) ? it->second : std::map<std::string, std::string>();
}

bool CHTLPropertyReference::hasElementProperty(const std::string& selector, const std::string& property) const {
    auto it = elementProperties.find(selector);
    return (it != elementProperties.end()) && (it->second.find(property) != it->second.end());
}

bool CHTLPropertyReference::hasStyleProperty(const std::string& selector, const std::string& property) const {
    auto it = styleProperties.find(selector);
    return (it != styleProperties.end()) && (it->second.find(property) != it->second.end());
}

void CHTLPropertyReference::removeElementProperty(const std::string& selector, const std::string& property) {
    auto element = findElement(selector);
    if (element) {
        element->removeAttribute(property);
        
        // 更新属性映射
        auto it = elementProperties.find(selector);
        if (it != elementProperties.end()) {
            it->second.erase(property);
        }
    }
}

void CHTLPropertyReference::removeStyleProperty(const std::string& selector, const std::string& property) {
    auto style = findStyle(selector);
    if (style) {
        style->removeStyleProperty(property);
        
        // 更新属性映射
        auto it = styleProperties.find(selector);
        if (it != styleProperties.end()) {
            it->second.erase(property);
        }
    }
}

void CHTLPropertyReference::clearElementProperties(const std::string& selector) {
    auto element = findElement(selector);
    if (element) {
        element->clearAttributes();
        
        // 更新属性映射
        elementProperties[selector].clear();
    }
}

void CHTLPropertyReference::clearStyleProperties(const std::string& selector) {
    auto style = findStyle(selector);
    if (style) {
        // 清除样式属性
        auto properties = style->getStyleProperties();
        for (const auto& prop : properties) {
            style->removeStyleProperty(prop.first);
        }
        
        // 更新属性映射
        styleProperties[selector].clear();
    }
}

void CHTLPropertyReference::addError(const std::string& error) {
    errors.push_back(error);
}

void CHTLPropertyReference::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

void CHTLPropertyReference::clearMessages() {
    errors.clear();
    warnings.clear();
}

void CHTLPropertyReference::reset() {
    elementMap.clear();
    elementProperties.clear();
    styleMap.clear();
    styleProperties.clear();
    clearMessages();
}

bool CHTLPropertyReference::validate() const {
    return errors.empty();
}

std::string CHTLPropertyReference::getDebugInfo() const {
    std::ostringstream oss;
    oss << "CHTL属性引用处理器调试信息:\n";
    oss << "调试模式: " << (debugMode ? "开启" : "关闭") << "\n";
    oss << "严格模式: " << (strictMode ? "开启" : "关闭") << "\n";
    oss << "链式调用: " << (enableChaining ? "开启" : "关闭") << "\n";
    oss << "可选链式调用: " << (enableOptionalChaining ? "开启" : "关闭") << "\n";
    oss << "元素映射数: " << elementMap.size() << "\n";
    oss << "样式映射数: " << styleMap.size() << "\n";
    oss << "错误数: " << errors.size() << "\n";
    oss << "警告数: " << warnings.size() << "\n";
    return oss.str();
}

} // namespace CHTL