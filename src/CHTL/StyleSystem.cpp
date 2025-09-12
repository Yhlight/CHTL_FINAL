#include "StyleSystem.h"
#include <sstream>
#include <regex>
#include <algorithm>
#include <cmath>

namespace CHTL {

// PropertyCalculator方法实现
std::string PropertyCalculator::calculate(const std::string& expression, 
                                        const std::map<std::string, std::string>& context) {
    std::string result = expression;
    
    // 处理属性引用
    std::regex refRegex(R"((\w+\.\w+))");
    std::smatch matches;
    
    while (std::regex_search(result, matches, refRegex)) {
        std::string reference = matches[1].str();
        std::string value = PropertyReferenceParser::parse(reference, context);
        result = std::regex_replace(result, std::regex(reference), value);
    }
    
    // 处理算术运算
    if (result.find('+') != std::string::npos || 
        result.find('-') != std::string::npos || 
        result.find('*') != std::string::npos || 
        result.find('/') != std::string::npos || 
        result.find('%') != std::string::npos || 
        result.find("**") != std::string::npos) {
        
        double value = parseArithmeticExpression(result);
        auto [num, unit] = extractValueAndUnit(result);
        result = std::to_string(value) + unit;
    }
    
    return result;
}

double PropertyCalculator::parseArithmeticExpression(const std::string& expression) {
    // 简化的算术表达式解析
    // 实际实现需要更复杂的表达式解析器
    
    std::string expr = expression;
    
    // 处理幂运算
    std::regex powerRegex(R"((\d+(?:\.\d+)?)\s*\*\*\s*(\d+(?:\.\d+)?))");
    std::smatch powerMatches;
    while (std::regex_search(expr, powerMatches, powerRegex)) {
        double base = std::stod(powerMatches[1].str());
        double exp = std::stod(powerMatches[2].str());
        double result = std::pow(base, exp);
        expr = std::regex_replace(expr, powerRegex, std::to_string(result));
    }
    
    // 处理乘除模运算
    std::regex mulDivModRegex(R"((\d+(?:\.\d+)?)\s*([*/%])\s*(\d+(?:\.\d+)?))");
    std::smatch mulDivModMatches;
    while (std::regex_search(expr, mulDivModMatches, mulDivModRegex)) {
        double left = std::stod(mulDivModMatches[1].str());
        std::string op = mulDivModMatches[2].str();
        double right = std::stod(mulDivModMatches[3].str());
        
        double result = 0;
        if (op == "*") result = left * right;
        else if (op == "/") result = left / right;
        else if (op == "%") result = std::fmod(left, right);
        
        expr = std::regex_replace(expr, mulDivModRegex, std::to_string(result));
    }
    
    // 处理加减运算
    std::regex addSubRegex(R"((\d+(?:\.\d+)?)\s*([+-])\s*(\d+(?:\.\d+)?))");
    std::smatch addSubMatches;
    while (std::regex_search(expr, addSubMatches, addSubRegex)) {
        double left = std::stod(addSubMatches[1].str());
        std::string op = addSubMatches[2].str();
        double right = std::stod(addSubMatches[3].str());
        
        double result = 0;
        if (op == "+") result = left + right;
        else if (op == "-") result = left - right;
        
        expr = std::regex_replace(expr, addSubRegex, std::to_string(result));
    }
    
    return std::stod(expr);
}

bool PropertyCalculator::validateUnitCompatibility(const std::string& unit1, const std::string& unit2) {
    // 简化的单位兼容性检查
    if (unit1 == unit2) return true;
    
    // 长度单位兼容性
    std::vector<std::string> lengthUnits = {"px", "em", "rem", "%", "vw", "vh", "vmin", "vmax"};
    bool unit1IsLength = std::find(lengthUnits.begin(), lengthUnits.end(), unit1) != lengthUnits.end();
    bool unit2IsLength = std::find(lengthUnits.begin(), lengthUnits.end(), unit2) != lengthUnits.end();
    
    if (unit1IsLength && unit2IsLength) return true;
    
    // 角度单位兼容性
    std::vector<std::string> angleUnits = {"deg", "rad", "grad", "turn"};
    bool unit1IsAngle = std::find(angleUnits.begin(), angleUnits.end(), unit1) != angleUnits.end();
    bool unit2IsAngle = std::find(angleUnits.begin(), angleUnits.end(), unit2) != angleUnits.end();
    
    if (unit1IsAngle && unit2IsAngle) return true;
    
    return false;
}

std::pair<double, std::string> PropertyCalculator::extractValueAndUnit(const std::string& value) {
    std::regex valueRegex(R"((\d+(?:\.\d+)?)\s*(\w*))");
    std::smatch matches;
    
    if (std::regex_search(value, matches, valueRegex)) {
        double num = std::stod(matches[1].str());
        std::string unit = matches[2].str();
        return {num, unit};
    }
    
    return {0.0, ""};
}

// ConditionalExpressionParser方法实现
std::string ConditionalExpressionParser::parse(const std::string& expression, 
                                             const std::map<std::string, std::string>& context) {
    // 解析条件表达式: condition ? trueValue : falseValue
    std::regex condRegex(R"((.+?)\s*\?\s*(.+?)\s*:\s*(.+))");
    std::smatch matches;
    
    if (std::regex_search(expression, matches, condRegex)) {
        std::string condition = matches[1].str();
        std::string trueValue = matches[2].str();
        std::string falseValue = matches[3].str();
        
        if (evaluateCondition(condition, context)) {
            return trueValue;
        } else {
            return falseValue;
        }
    }
    
    return expression;
}

std::string ConditionalExpressionParser::parseChain(const std::vector<std::string>& expressions, 
                                                  const std::map<std::string, std::string>& context) {
    for (const auto& expr : expressions) {
        std::string result = parse(expr, context);
        if (!result.empty() && result != expr) {
            return result;
        }
    }
    
    return expressions.back(); // 返回最后一个表达式作为默认值
}

std::string ConditionalExpressionParser::parseOptional(const std::vector<std::string>& expressions, 
                                                     const std::map<std::string, std::string>& context) {
    for (const auto& expr : expressions) {
        std::string result = parse(expr, context);
        if (!result.empty()) {
            return result;
        }
    }
    
    return ""; // 没有匹配的条件
}

bool ConditionalExpressionParser::evaluateCondition(const std::string& condition, 
                                                  const std::map<std::string, std::string>& context) {
    // 简化的条件评估
    // 实际实现需要更复杂的条件解析器
    
    // 处理比较运算符
    std::regex compRegex(R"((\w+)\s*([><=!]+)\s*(\w+))");
    std::smatch matches;
    
    if (std::regex_search(condition, matches, compRegex)) {
        std::string left = matches[1].str();
        std::string op = matches[2].str();
        std::string right = matches[3].str();
        
        // 从上下文中获取值
        std::string leftValue = context.count(left) ? context.at(left) : left;
        std::string rightValue = context.count(right) ? context.at(right) : right;
        
        // 转换为数值进行比较
        try {
            double leftNum = std::stod(leftValue);
            double rightNum = std::stod(rightValue);
            
            if (op == ">") return leftNum > rightNum;
            else if (op == "<") return leftNum < rightNum;
            else if (op == ">=") return leftNum >= rightNum;
            else if (op == "<=") return leftNum <= rightNum;
            else if (op == "==") return leftNum == rightNum;
            else if (op == "!=") return leftNum != rightNum;
        } catch (...) {
            // 字符串比较
            if (op == "==") return leftValue == rightValue;
            else if (op == "!=") return leftValue != rightValue;
        }
    }
    
    // 处理逻辑运算符
    if (condition.find("&&") != std::string::npos) {
        std::vector<std::string> parts;
        std::stringstream ss(condition);
        std::string part;
        while (std::getline(ss, part, '&')) {
            if (!part.empty()) {
                parts.push_back(part);
            }
        }
        
        bool result = true;
        for (const auto& p : parts) {
            result = result && evaluateCondition(p, context);
        }
        return result;
    }
    
    if (condition.find("||") != std::string::npos) {
        std::vector<std::string> parts;
        std::stringstream ss(condition);
        std::string part;
        while (std::getline(ss, part, '|')) {
            if (!part.empty()) {
                parts.push_back(part);
            }
        }
        
        bool result = false;
        for (const auto& p : parts) {
            result = result || evaluateCondition(p, context);
        }
        return result;
    }
    
    return false;
}

// PropertyReferenceParser方法实现
std::string PropertyReferenceParser::parse(const std::string& reference, 
                                         const std::map<std::string, std::string>& context) {
    // 解析选择器.属性格式
    size_t dotPos = reference.find('.');
    if (dotPos == std::string::npos) return reference;
    
    std::string selector = reference.substr(0, dotPos);
    std::string property = reference.substr(dotPos + 1);
    
    return getPropertyValue(selector, property, context);
}

std::string PropertyReferenceParser::parseSelector(const std::string& selector) {
    // 解析选择器
    return selector;
}

std::string PropertyReferenceParser::parseProperty(const std::string& property) {
    // 解析属性名
    return property;
}

std::string PropertyReferenceParser::resolveSelector(const std::string& selector, 
                                                   const std::map<std::string, std::string>& context) {
    // 解析选择器
    return selector;
}

std::string PropertyReferenceParser::getPropertyValue(const std::string& selector, 
                                                    const std::string& property,
                                                    const std::map<std::string, std::string>& context) {
    // 从上下文中获取属性值
    std::string key = selector + "." + property;
    return context.count(key) ? context.at(key) : "0px";
}

// ReactiveValueProcessor方法实现
std::string ReactiveValueProcessor::process(const std::string& value, 
                                          const std::map<std::string, std::string>& context) {
    if (!isReactiveValue(value)) return value;
    
    std::string variableName = extractVariableName(value);
    return context.count(variableName) ? context.at(variableName) : variableName;
}

std::string ReactiveValueProcessor::extractVariableName(const std::string& value) {
    // 提取$变量名$中的变量名
    std::regex reactiveRegex(R"(\$(\w+)\$)");
    std::smatch matches;
    
    if (std::regex_search(value, matches, reactiveRegex)) {
        return matches[1].str();
    }
    
    return value;
}

std::string ReactiveValueProcessor::generateReactiveListener(const std::string& variableName) {
    return "// 响应式监听: " + variableName;
}

bool ReactiveValueProcessor::isReactiveValue(const std::string& value) {
    return value.find('$') != std::string::npos && 
           value.find('$', value.find('$') + 1) != std::string::npos;
}

// LocalStyleProcessor方法实现
std::string LocalStyleProcessor::process(const StyleNode* styleNode, 
                                       const ElementNode* elementNode) {
    if (!styleNode) return "";
    
    std::stringstream css;
    
    // 处理内联样式
    std::string inlineStyles = processInlineStyles(parseStyleProperties(styleNode));
    if (!inlineStyles.empty()) {
        css << "style=\"" << inlineStyles << "\"";
    }
    
    // 处理类选择器
    // 处理ID选择器
    // 处理伪类选择器
    // 处理伪元素选择器
    
    return css.str();
}

std::string LocalStyleProcessor::processInlineStyles(const std::vector<StyleProperty>& properties) {
    std::stringstream styles;
    
    for (const auto& prop : properties) {
        if (prop.type == StylePropertyType::INLINE) {
            styles << prop.name << ": " << prop.value << "; ";
        }
    }
    
    return styles.str();
}

std::string LocalStyleProcessor::processClassSelector(const std::vector<StyleProperty>& properties, 
                                                    const std::string& className) {
    std::stringstream css;
    
    css << "." << className << " {\n";
    for (const auto& prop : properties) {
        css << "    " << prop.name << ": " << prop.value << ";\n";
    }
    css << "}\n";
    
    return css.str();
}

std::string LocalStyleProcessor::processIdSelector(const std::vector<StyleProperty>& properties, 
                                                 const std::string& idName) {
    std::stringstream css;
    
    css << "#" << idName << " {\n";
    for (const auto& prop : properties) {
        css << "    " << prop.name << ": " << prop.value << ";\n";
    }
    css << "}\n";
    
    return css.str();
}

std::string LocalStyleProcessor::processPseudoClassSelector(const std::vector<StyleProperty>& properties, 
                                                         const std::string& pseudoClass,
                                                         const std::string& baseSelector) {
    std::stringstream css;
    
    css << baseSelector << ":" << pseudoClass << " {\n";
    for (const auto& prop : properties) {
        css << "    " << prop.name << ": " << prop.value << ";\n";
    }
    css << "}\n";
    
    return css.str();
}

std::string LocalStyleProcessor::processPseudoElementSelector(const std::vector<StyleProperty>& properties, 
                                                           const std::string& pseudoElement,
                                                           const std::string& baseSelector) {
    std::stringstream css;
    
    css << baseSelector << "::" << pseudoElement << " {\n";
    for (const auto& prop : properties) {
        css << "    " << prop.name << ": " << prop.value << ";\n";
    }
    css << "}\n";
    
    return css.str();
}

std::string LocalStyleProcessor::processContextDerivation(const std::vector<StyleProperty>& properties, 
                                                        const std::map<std::string, std::string>& context) {
    // 处理上下文推导
    return "";
}

// GlobalStyleProcessor方法实现
std::string GlobalStyleProcessor::process(const StyleNode* styleNode) {
    if (!styleNode) return "";
    
    std::stringstream css;
    
    for (const auto& prop : styleNode->getProperties()) {
        css << prop.first << ": " << prop.second << ";\n";
    }
    
    return css.str();
}

std::string GlobalStyleProcessor::processTemplateVariables(const std::vector<StyleProperty>& properties, 
                                                         const std::map<std::string, std::string>& variables) {
    std::stringstream css;
    
    for (const auto& prop : properties) {
        std::string value = replaceVariables(prop.value, variables);
        css << prop.name << ": " << value << ";\n";
    }
    
    return css.str();
}

std::string GlobalStyleProcessor::processCustomVariables(const std::vector<StyleProperty>& properties, 
                                                       const std::map<std::string, std::string>& variables) {
    return processTemplateVariables(properties, variables);
}

std::string GlobalStyleProcessor::replaceVariables(const std::string& value, 
                                                 const std::map<std::string, std::string>& variables) {
    std::string result = value;
    
    for (const auto& var : variables) {
        std::string placeholder = "{" + var.first + "}";
        size_t pos = 0;
        while ((pos = result.find(placeholder, pos)) != std::string::npos) {
            result.replace(pos, placeholder.length(), var.second);
            pos += var.second.length();
        }
    }
    
    return result;
}

// StyleSystem方法实现
StyleSystem::StyleSystem() {}

StyleSystem::~StyleSystem() {}

std::string StyleSystem::processStyleNode(const StyleNode* styleNode, 
                                        const ElementNode* elementNode) {
    if (!styleNode) return "";
    
    if (elementNode) {
        // 局部样式块处理
        return LocalStyleProcessor::process(styleNode, elementNode);
    } else {
        // 全局样式块处理
        std::string result = GlobalStyleProcessor::process(styleNode);
        addGlobalStyle(result);
        return result;
    }
}

std::string StyleSystem::processPropertyCalculation(const std::string& expression, 
                                                   const std::map<std::string, std::string>& context) {
    return PropertyCalculator::calculate(expression, context);
}

std::string StyleSystem::processConditionalExpression(const std::string& expression, 
                                                     const std::map<std::string, std::string>& context) {
    return ConditionalExpressionParser::parse(expression, context);
}

std::string StyleSystem::processPropertyReference(const std::string& reference, 
                                                 const std::map<std::string, std::string>& context) {
    return PropertyReferenceParser::parse(reference, context);
}

std::string StyleSystem::processReactiveValue(const std::string& value, 
                                            const std::map<std::string, std::string>& context) {
    return ReactiveValueProcessor::process(value, context);
}

void StyleSystem::addGlobalStyle(const std::string& css) {
    globalStyles += css + "\n";
}

std::string StyleSystem::getGlobalStyles() const {
    return globalStyles;
}

void StyleSystem::clearGlobalStyles() {
    globalStyles.clear();
}

std::vector<StyleProperty> StyleSystem::parseStyleProperties(const StyleNode* styleNode) {
    std::vector<StyleProperty> properties;
    
    for (const auto& prop : styleNode->getProperties()) {
        properties.emplace_back(prop.first, prop.second, StylePropertyType::INLINE);
    }
    
    return properties;
}

std::string StyleSystem::processPropertyValue(const std::string& value, 
                                            const std::map<std::string, std::string>& context) {
    std::string result = value;
    
    // 处理响应式值
    result = processReactiveValue(result, context);
    
    // 处理属性引用
    result = processPropertyReference(result, context);
    
    // 处理属性运算
    result = processPropertyCalculation(result, context);
    
    return result;
}

bool StyleSystem::validateStyleProperty(const StyleProperty& property) {
    // 简化的属性验证
    return !property.name.empty() && !property.value.empty();
}

std::string StyleSystem::generateCSS(const std::vector<StyleProperty>& properties, 
                                    const std::string& selector) {
    std::stringstream css;
    
    if (!selector.empty()) {
        css << selector << " {\n";
    }
    
    for (const auto& prop : properties) {
        css << "    " << prop.name << ": " << prop.value << ";\n";
    }
    
    if (!selector.empty()) {
        css << "}\n";
    }
    
    return css.str();
}

} // namespace CHTL