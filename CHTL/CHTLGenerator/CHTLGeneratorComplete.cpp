#include "CHTLGeneratorComplete.h"
#include <sstream>
#include <regex>
#include <algorithm>
#include <cmath>

namespace CHTL {

CHTLGeneratorComplete::CHTLGeneratorComplete(std::shared_ptr<CHTLContext> context)
    : context_(context) {
    if (!context_) {
        context_ = std::make_shared<CHTLContext>();
    }
}

CHTLGeneratorComplete::~CHTLGeneratorComplete() {
}

std::string CHTLGeneratorComplete::generate(std::shared_ptr<CHTLBaseNode> ast) {
    if (!ast) {
        return "";
    }
    
    // 使用上下文信息进行更智能的代码生成
    switch (ast->getType()) {
        case CHTLBaseNode::NodeType::ELEMENT:
            return generateElement(ast);
        case CHTLBaseNode::NodeType::TEXT:
            return generateText(ast);
        case CHTLBaseNode::NodeType::COMMENT:
            return generateComment(ast);
        case CHTLBaseNode::NodeType::STYLE:
            return generateLocalStyleBlock(ast);
        case CHTLBaseNode::NodeType::SCRIPT:
            return generateScript(ast);
        case CHTLBaseNode::NodeType::TEMPLATE:
            return generateTemplate(ast);
        case CHTLBaseNode::NodeType::CUSTOM:
            return generateCustom(ast);
        case CHTLBaseNode::NodeType::ORIGIN:
            return generateOrigin(ast);
        case CHTLBaseNode::NodeType::IMPORT:
            return generateImport(ast);
        case CHTLBaseNode::NodeType::NAMESPACE:
            return generateNamespace(ast);
        case CHTLBaseNode::NodeType::CONFIGURATION:
            return generateConfiguration(ast);
        case CHTLBaseNode::NodeType::DELETE:
        case CHTLBaseNode::NodeType::INSERT:
        case CHTLBaseNode::NodeType::USE:
            return generateOperator(ast);
        case CHTLBaseNode::NodeType::SELECTOR:
            return generateSelector(ast);
        case CHTLBaseNode::NodeType::RESPONSIVE_VALUE:
            return generateResponsiveValue(ast);
        case CHTLBaseNode::NodeType::GENERATOR_COMMENT:
            return generateGeneratorComment(ast);
        case CHTLBaseNode::NodeType::EXPRESSION:
            return generateExpression(ast);
        case CHTLBaseNode::NodeType::BLOCK:
            return generateBlock(ast);
        default:
            return CHTLGenerator::generate(ast);
    }
}

std::string CHTLGeneratorComplete::generateDocument(const std::vector<std::shared_ptr<CHTLBaseNode>>& statements) {
    html_output_.clear();
    css_output_.clear();
    js_output_.clear();
    
    std::stringstream html_ss;
    std::stringstream css_ss;
    std::stringstream js_ss;
    
    // 生成HTML文档结构
    if (generate_html_) {
        html_ss << "<!DOCTYPE html>\n<html>\n<head>\n";
        html_ss << "    <meta charset=\"UTF-8\">\n";
        html_ss << "    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n";
        html_ss << "    <title>CHTL Generated Document</title>\n";
        
        if (generate_css_) {
            html_ss << "    <style>\n";
            html_ss << "        /* CHTL Generated CSS */\n";
            html_ss << "    </style>\n";
        }
        
        html_ss << "</head>\n<body>\n";
    }
    
    // 处理所有语句
    for (const auto& statement : statements) {
        std::string generated = generate(statement);
        
        if (generate_html_) {
            html_ss << generated << "\n";
        }
    }
    
    if (generate_html_) {
        html_ss << "</body>\n</html>";
    }
    
    html_output_ = html_ss.str();
    css_output_ = css_ss.str();
    js_output_ = js_ss.str();
    
    // 应用优化
    if (minify_output_) {
        html_output_ = minifyHTML(html_output_);
        css_output_ = minifyCSS(css_output_);
        js_output_ = minifyJS(js_output_);
    }
    
    return getCombinedOutput();
}

std::string CHTLGeneratorComplete::generateLocalStyleBlock(std::shared_ptr<CHTLBaseNode> node) {
    auto style = std::dynamic_pointer_cast<StyleNode>(node);
    if (!style) return "";
    
    if (style->isInlineStyle()) {
        // 内联样式
        return generateInlineStyle(style->getStyleProperties());
    } else {
        // 局部样式块 - 生成CSS并添加到全局样式
        std::stringstream ss;
        
        // 处理自动类名/ID
        if (style->hasClassSelectors()) {
            for (const auto& className : style->getClassSelectors()) {
                std::string css = generateAutoClassStyle(className, style->getStyleProperties());
                css_output_ += css + "\n";
            }
        }
        
        if (style->hasIdSelectors()) {
            for (const auto& idName : style->getIdSelectors()) {
                std::string css = generateAutoIdStyle(idName, style->getStyleProperties());
                css_output_ += css + "\n";
            }
        }
        
        // 处理上下文选择器
        if (style->hasPseudoSelectors()) {
            for (const auto& pseudo : style->getPseudoSelectors()) {
                std::string css = generateContextualStyle(pseudo, style->getStyleProperties());
                css_output_ += css + "\n";
            }
        }
        
        return ""; // 局部样式块不生成HTML内容
    }
}

std::string CHTLGeneratorComplete::generateInlineStyle(const std::map<std::string, std::string>& properties) const {
    std::stringstream ss;
    ss << "style=\"";
    
    bool first = true;
    for (const auto& prop : properties) {
        if (!first) ss << "; ";
        
        std::string value = prop.second;
        
        // 处理属性运算
        if (value.find('+') != std::string::npos || 
            value.find('-') != std::string::npos ||
            value.find('*') != std::string::npos ||
            value.find('/') != std::string::npos) {
            value = generatePropertyCalculation(value);
        }
        
        // 处理引用属性
        if (value.find('.') != std::string::npos) {
            value = processPropertyReferences(value);
        }
        
        // 处理响应式值
        if (value.find('$') != std::string::npos) {
            value = processResponsiveValues(value);
        }
        
        ss << prop.first << ": " << value;
        first = false;
    }
    
    ss << "\"";
    return ss.str();
}

std::string CHTLGeneratorComplete::generateAutoClassStyle(const std::string& className, 
                                                         const std::map<std::string, std::string>& properties) const {
    std::stringstream ss;
    ss << "." << className << " {\n";
    
    for (const auto& prop : properties) {
        std::string value = prop.second;
        
        // 处理属性运算
        if (value.find('+') != std::string::npos || 
            value.find('-') != std::string::npos ||
            value.find('*') != std::string::npos ||
            value.find('/') != std::string::npos) {
            value = generatePropertyCalculation(value);
        }
        
        // 处理引用属性
        if (value.find('.') != std::string::npos) {
            value = processPropertyReferences(value);
        }
        
        // 处理响应式值
        if (value.find('$') != std::string::npos) {
            value = processResponsiveValues(value);
        }
        
        ss << "    " << prop.first << ": " << value << ";\n";
    }
    
    ss << "}";
    return ss.str();
}

std::string CHTLGeneratorComplete::generateAutoIdStyle(const std::string& idName, 
                                                      const std::map<std::string, std::string>& properties) const {
    std::stringstream ss;
    ss << "#" << idName << " {\n";
    
    for (const auto& prop : properties) {
        std::string value = prop.second;
        
        // 处理属性运算
        if (value.find('+') != std::string::npos || 
            value.find('-') != std::string::npos ||
            value.find('*') != std::string::npos ||
            value.find('/') != std::string::npos) {
            value = generatePropertyCalculation(value);
        }
        
        // 处理引用属性
        if (value.find('.') != std::string::npos) {
            value = processPropertyReferences(value);
        }
        
        // 处理响应式值
        if (value.find('$') != std::string::npos) {
            value = processResponsiveValues(value);
        }
        
        ss << "    " << prop.first << ": " << value << ";\n";
    }
    
    ss << "}";
    return ss.str();
}

std::string CHTLGeneratorComplete::generateContextualStyle(const std::string& context, 
                                                          const std::map<std::string, std::string>& properties) const {
    std::stringstream ss;
    ss << context << " {\n";
    
    for (const auto& prop : properties) {
        std::string value = prop.second;
        
        // 处理属性运算
        if (value.find('+') != std::string::npos || 
            value.find('-') != std::string::npos ||
            value.find('*') != std::string::npos ||
            value.find('/') != std::string::npos) {
            value = generatePropertyCalculation(value);
        }
        
        // 处理引用属性
        if (value.find('.') != std::string::npos) {
            value = processPropertyReferences(value);
        }
        
        // 处理响应式值
        if (value.find('$') != std::string::npos) {
            value = processResponsiveValues(value);
        }
        
        ss << "    " << prop.first << ": " << value << ";\n";
    }
    
    ss << "}";
    return ss.str();
}

std::string CHTLGeneratorComplete::generatePropertyCalculation(const std::string& expression) const {
    // 简单的属性运算实现
    // 实际实现中应该使用更复杂的表达式解析器
    
    std::string result = expression;
    
    // 处理基本算术运算
    std::regex addRegex(R"((\d+(?:\.\d+)?(?:px|em|rem|%|vh|vw)?)\s*\+\s*(\d+(?:\.\d+)?(?:px|em|rem|%|vh|vw)?))");
    std::smatch match;
    
    while (std::regex_search(result, match, addRegex)) {
        double left = std::stod(match[1].str());
        double right = std::stod(match[2].str());
        double sum = left + right;
        
        std::string unit = "";
        if (match[1].str().find("px") != std::string::npos) unit = "px";
        else if (match[1].str().find("em") != std::string::npos) unit = "em";
        else if (match[1].str().find("rem") != std::string::npos) unit = "rem";
        else if (match[1].str().find("%") != std::string::npos) unit = "%";
        else if (match[1].str().find("vh") != std::string::npos) unit = "vh";
        else if (match[1].str().find("vw") != std::string::npos) unit = "vw";
        
        std::string replacement = std::to_string(sum) + unit;
        result = std::regex_replace(result, addRegex, replacement);
    }
    
    // 处理减法
    std::regex subRegex(R"((\d+(?:\.\d+)?(?:px|em|rem|%|vh|vw)?)\s*-\s*(\d+(?:\.\d+)?(?:px|em|rem|%|vh|vw)?))");
    while (std::regex_search(result, match, subRegex)) {
        double left = std::stod(match[1].str());
        double right = std::stod(match[2].str());
        double diff = left - right;
        
        std::string unit = "";
        if (match[1].str().find("px") != std::string::npos) unit = "px";
        else if (match[1].str().find("em") != std::string::npos) unit = "em";
        else if (match[1].str().find("rem") != std::string::npos) unit = "rem";
        else if (match[1].str().find("%") != std::string::npos) unit = "%";
        else if (match[1].str().find("vh") != std::string::npos) unit = "vh";
        else if (match[1].str().find("vw") != std::string::npos) unit = "vw";
        
        std::string replacement = std::to_string(diff) + unit;
        result = std::regex_replace(result, subRegex, replacement);
    }
    
    // 处理乘法
    std::regex mulRegex(R"((\d+(?:\.\d+)?(?:px|em|rem|%|vh|vw)?)\s*\*\s*(\d+(?:\.\d+)?))");
    while (std::regex_search(result, match, mulRegex)) {
        double left = std::stod(match[1].str());
        double right = std::stod(match[2].str());
        double product = left * right;
        
        std::string unit = "";
        if (match[1].str().find("px") != std::string::npos) unit = "px";
        else if (match[1].str().find("em") != std::string::npos) unit = "em";
        else if (match[1].str().find("rem") != std::string::npos) unit = "rem";
        else if (match[1].str().find("%") != std::string::npos) unit = "%";
        else if (match[1].str().find("vh") != std::string::npos) unit = "vh";
        else if (match[1].str().find("vw") != std::string::npos) unit = "vw";
        
        std::string replacement = std::to_string(product) + unit;
        result = std::regex_replace(result, mulRegex, replacement);
    }
    
    // 处理除法
    std::regex divRegex(R"((\d+(?:\.\d+)?(?:px|em|rem|%|vh|vw)?)\s*/\s*(\d+(?:\.\d+)?))");
    while (std::regex_search(result, match, divRegex)) {
        double left = std::stod(match[1].str());
        double right = std::stod(match[2].str());
        if (right != 0) {
            double quotient = left / right;
            
            std::string unit = "";
            if (match[1].str().find("px") != std::string::npos) unit = "px";
            else if (match[1].str().find("em") != std::string::npos) unit = "em";
            else if (match[1].str().find("rem") != std::string::npos) unit = "rem";
            else if (match[1].str().find("%") != std::string::npos) unit = "%";
            else if (match[1].str().find("vh") != std::string::npos) unit = "vh";
            else if (match[1].str().find("vw") != std::string::npos) unit = "vw";
            
            std::string replacement = std::to_string(quotient) + unit;
            result = std::regex_replace(result, divRegex, replacement);
        }
    }
    
    return result;
}

std::string CHTLGeneratorComplete::generateArithmeticExpression(const std::string& expression) const {
    return generatePropertyCalculation(expression);
}

std::string CHTLGeneratorComplete::generateUnitConversion(const std::string& value, 
                                                         const std::string& fromUnit, 
                                                         const std::string& toUnit) const {
    // 简单的单位转换实现
    double numValue = std::stod(value);
    
    if (fromUnit == "px" && toUnit == "em") {
        return std::to_string(numValue / 16.0) + "em";
    } else if (fromUnit == "em" && toUnit == "px") {
        return std::to_string(numValue * 16.0) + "px";
    } else if (fromUnit == "px" && toUnit == "rem") {
        return std::to_string(numValue / 16.0) + "rem";
    } else if (fromUnit == "rem" && toUnit == "px") {
        return std::to_string(numValue * 16.0) + "px";
    }
    
    return value + toUnit;
}

std::string CHTLGeneratorComplete::generatePropertyReference(const std::string& selector, 
                                                           const std::string& property) const {
    if (context_) {
        return context_->getPropertyReference(selector, property);
    }
    return "";
}

std::string CHTLGeneratorComplete::generateDynamicPropertyReference(const std::string& expression) const {
    // 解析动态属性引用表达式
    // 例如: {{box}}->width > 2 ? 100px : 50px
    std::regex conditionRegex(R"(\{\{([^}]+)\}\}->(\w+)\s*([><=!]+)\s*(\d+(?:\.\d+)?)\s*\?\s*([^:]+)\s*:\s*([^}]+))");
    std::smatch match;
    
    if (std::regex_search(expression, match, conditionRegex)) {
        std::string selector = match[1].str();
        std::string property = match[2].str();
        std::string operator_ = match[3].str();
        std::string threshold = match[4].str();
        std::string trueValue = match[5].str();
        std::string falseValue = match[6].str();
        
        // 获取属性值
        std::string propValue = generatePropertyReference(selector, property);
        if (!propValue.empty()) {
            double propNum = std::stod(propValue);
            double thresholdNum = std::stod(threshold);
            
            bool condition = false;
            if (operator_ == ">") condition = propNum > thresholdNum;
            else if (operator_ == "<") condition = propNum < thresholdNum;
            else if (operator_ == ">=") condition = propNum >= thresholdNum;
            else if (operator_ == "<=") condition = propNum <= thresholdNum;
            else if (operator_ == "==") condition = propNum == thresholdNum;
            else if (operator_ == "!=") condition = propNum != thresholdNum;
            
            return condition ? trueValue : falseValue;
        }
    }
    
    return expression;
}

std::string CHTLGeneratorComplete::generateConditionalProperty(const std::string& condition, 
                                                             const std::string& trueValue, 
                                                             const std::string& falseValue) const {
    // 简单的条件属性实现
    // 实际实现中应该使用更复杂的条件解析器
    return condition == "true" ? trueValue : falseValue;
}

std::string CHTLGeneratorComplete::generateTemplateVariable(const std::string& varName, const std::string& value) const {
    if (context_) {
        context_->setVariable(varName, value, CHTLContext::VariableType::TEMPLATE_VAR);
    }
    return "{{" + varName + "}}";
}

std::string CHTLGeneratorComplete::generateCustomVariable(const std::string& varName, const std::string& value) const {
    if (context_) {
        context_->setVariable(varName, value, CHTLContext::VariableType::CUSTOM_VAR);
    }
    return "{{" + varName + "}}";
}

std::string CHTLGeneratorComplete::generateTemplateStyleGroup(const std::string& groupName, 
                                                             const std::map<std::string, std::string>& properties) const {
    if (context_) {
        context_->setTemplate(groupName, CHTLContext::TemplateType::TEMPLATE_STYLE, properties);
    }
    return "{{" + groupName + "}}";
}

std::string CHTLGeneratorComplete::generateCustomStyleGroup(const std::string& groupName, 
                                                           const std::map<std::string, std::string>& properties) const {
    if (context_) {
        context_->setTemplate(groupName, CHTLContext::TemplateType::CUSTOM_STYLE, properties);
    }
    return "{{" + groupName + "}}";
}

std::string CHTLGeneratorComplete::generateStyleGroupSpecialization(const std::string& parentGroup, 
                                                                   const std::string& childGroup, 
                                                                   const std::map<std::string, std::string>& properties) const {
    if (context_) {
        context_->specializeTemplate(parentGroup, childGroup, properties);
    }
    return "{{" + childGroup + "}}";
}

std::string CHTLGeneratorComplete::generateResponsiveValue(const std::string& jsVarName) const {
    if (context_) {
        return context_->getResponsiveValue(jsVarName);
    }
    return "$" + jsVarName + "$";
}

std::string CHTLGeneratorComplete::generateResponsiveCalculation(const std::string& expression) const {
    // 处理响应式值计算
    std::string result = expression;
    
    // 查找响应式值模式 $变量名$
    std::regex responsiveRegex(R"(\$(\w+)\$)");
    std::smatch match;
    
    while (std::regex_search(result, match, responsiveRegex)) {
        std::string varName = match[1].str();
        std::string value = generateResponsiveValue(varName);
        result = std::regex_replace(result, std::regex("\\$" + varName + "\\$"), value);
    }
    
    return result;
}

std::string CHTLGeneratorComplete::generateNamespaceImport(const std::string& namespaceName, 
                                                          const std::string& itemName) const {
    if (context_) {
        context_->addNamespaceImport(namespaceName, itemName, "");
    }
    return namespaceName + "::" + itemName;
}

std::string CHTLGeneratorComplete::generateFullQualifiedName(const std::string& namespaceName, 
                                                           const std::string& itemName) const {
    return namespaceName + "::" + itemName;
}

std::string CHTLGeneratorComplete::generateConfigurationValue(const std::string& configName, 
                                                            const std::string& key) const {
    if (context_) {
        return context_->getConfigurationValue(configName, key);
    }
    return "";
}

std::string CHTLGeneratorComplete::generateRawHTML(const std::string& content) const {
    return content;
}

std::string CHTLGeneratorComplete::generateRawCSS(const std::string& content) const {
    return content;
}

std::string CHTLGeneratorComplete::generateRawJavaScript(const std::string& content) const {
    return content;
}

std::string CHTLGeneratorComplete::generateImportHTML(const std::string& path, const std::string& alias) const {
    std::stringstream ss;
    ss << "<!-- Import HTML: " << path;
    if (!alias.empty()) {
        ss << " as " << alias;
    }
    ss << " -->";
    return ss.str();
}

std::string CHTLGeneratorComplete::generateImportCSS(const std::string& path, const std::string& alias) const {
    std::stringstream ss;
    ss << "/* Import CSS: " << path;
    if (!alias.empty()) {
        ss << " as " << alias;
    }
    ss << " */";
    return ss.str();
}

std::string CHTLGeneratorComplete::generateImportJavaScript(const std::string& path, const std::string& alias) const {
    std::stringstream ss;
    ss << "// Import JS: " << path;
    if (!alias.empty()) {
        ss << " as " << alias;
    }
    return ss.str();
}

std::string CHTLGeneratorComplete::generateImportCHTL(const std::string& path, const std::string& alias) const {
    std::stringstream ss;
    ss << "<!-- Import CHTL: " << path;
    if (!alias.empty()) {
        ss << " as " << alias;
    }
    ss << " -->";
    return ss.str();
}

std::string CHTLGeneratorComplete::generateImportCJMOD(const std::string& path, const std::string& alias) const {
    std::stringstream ss;
    ss << "// Import CJMOD: " << path;
    if (!alias.empty()) {
        ss << " as " << alias;
    }
    return ss.str();
}

std::string CHTLGeneratorComplete::generateDeleteAttribute(const std::string& attributeName) const {
    return "delete " + attributeName + ";";
}

std::string CHTLGeneratorComplete::generateDeleteElement(const std::string& elementSelector) const {
    return "delete element " + elementSelector + ";";
}

std::string CHTLGeneratorComplete::generateDeleteStyleProperty(const std::string& propertyName) const {
    return "delete property " + propertyName + ";";
}

std::string CHTLGeneratorComplete::generateDeleteStyleGroup(const std::string& groupName) const {
    return "delete group " + groupName + ";";
}

std::string CHTLGeneratorComplete::generateDeleteInheritance(const std::string& inheritedGroup) const {
    return "delete inheritance " + inheritedGroup + ";";
}

std::string CHTLGeneratorComplete::generateInsertElement(const std::string& position, 
                                                        const std::string& target, 
                                                        const std::string& content) const {
    return "insert " + position + " " + target + " " + content + ";";
}

std::string CHTLGeneratorComplete::generateInsertAttribute(const std::string& attributeName, 
                                                          const std::string& value) const {
    return "insert attribute " + attributeName + " = " + value + ";";
}

std::string CHTLGeneratorComplete::generateInsertStyleProperty(const std::string& propertyName, 
                                                              const std::string& value) const {
    return "insert property " + propertyName + " = " + value + ";";
}

std::string CHTLGeneratorComplete::generateVariableSpecialization(const std::string& parentVar, 
                                                                 const std::string& childVar, 
                                                                 const std::string& value) const {
    if (context_) {
        context_->specializeVariable(parentVar, childVar, value);
    }
    return "{{" + childVar + "}}";
}

std::string CHTLGeneratorComplete::generateTemplateSpecialization(const std::string& parentTemplate, 
                                                                 const std::string& childTemplate,
                                                                 const std::map<std::string, std::string>& properties) const {
    if (context_) {
        context_->specializeTemplate(parentTemplate, childTemplate, properties);
    }
    return "{{" + childTemplate + "}}";
}

std::string CHTLGeneratorComplete::generateStyleGroupInheritance(const std::string& parentGroup, 
                                                               const std::string& childGroup) const {
    return childGroup + " inherit(" + parentGroup + ")";
}

std::string CHTLGeneratorComplete::generateStyleGroupInheritanceDeletion(const std::string& parentGroup, 
                                                                        const std::string& childGroup) const {
    return childGroup + " delete " + parentGroup;
}

void CHTLGeneratorComplete::setContext(std::shared_ptr<CHTLContext> context) {
    context_ = context;
}

std::shared_ptr<CHTLContext> CHTLGeneratorComplete::getContext() const {
    return context_;
}

std::string CHTLGeneratorComplete::optimizeCSS(const std::string& css) const {
    return minifyCSS(css);
}

std::string CHTLGeneratorComplete::optimizeHTML(const std::string& html) const {
    return minifyHTML(html);
}

std::string CHTLGeneratorComplete::optimizeJavaScript(const std::string& js) const {
    return minifyJS(js);
}

void CHTLGeneratorComplete::setErrorHandler(std::function<void(const std::string&, int, int)> handler) {
    error_handler_ = handler;
}

void CHTLGeneratorComplete::reportError(const std::string& message, int line, int column) {
    logError(message, line, column);
    
    if (error_handler_) {
        error_handler_(message, line, column);
    }
}

std::string CHTLGeneratorComplete::processTemplateVariables(const std::string& content) const {
    std::string result = content;
    
    if (context_) {
        auto templateVars = context_->getVariablesByType(CHTLContext::VariableType::TEMPLATE_VAR);
        for (const auto& var : templateVars) {
            std::string placeholder = "{{" + var.name + "}}";
            result = std::regex_replace(result, std::regex(placeholder), var.value);
        }
    }
    
    return result;
}

std::string CHTLGeneratorComplete::processCustomVariables(const std::string& content) const {
    std::string result = content;
    
    if (context_) {
        auto customVars = context_->getVariablesByType(CHTLContext::VariableType::CUSTOM_VAR);
        for (const auto& var : customVars) {
            std::string placeholder = "{{" + var.name + "}}";
            result = std::regex_replace(result, std::regex(placeholder), var.value);
        }
    }
    
    return result;
}

std::string CHTLGeneratorComplete::processResponsiveValues(const std::string& content) const {
    std::string result = content;
    
    if (context_) {
        // 查找响应式值模式 $变量名$
        std::regex responsiveRegex(R"(\$(\w+)\$)");
        std::smatch match;
        
        while (std::regex_search(result, match, responsiveRegex)) {
            std::string varName = match[1].str();
            std::string value = context_->getResponsiveValue(varName);
            if (!value.empty()) {
                result = std::regex_replace(result, std::regex("\\$" + varName + "\\$"), value);
            }
        }
    }
    
    return result;
}

std::string CHTLGeneratorComplete::processPropertyReferences(const std::string& content) const {
    std::string result = content;
    
    if (context_) {
        // 查找属性引用模式 选择器.属性
        std::regex propRefRegex(R"((\w+)\.(\w+))");
        std::smatch match;
        
        while (std::regex_search(result, match, propRefRegex)) {
            std::string selector = match[1].str();
            std::string property = match[2].str();
            std::string value = context_->getPropertyReference(selector, property);
            if (!value.empty()) {
                result = std::regex_replace(result, std::regex(selector + "\\." + property), value);
            }
        }
    }
    
    return result;
}

std::string CHTLGeneratorComplete::processStyleProperties(const std::map<std::string, std::string>& properties) const {
    std::stringstream ss;
    
    for (const auto& prop : properties) {
        std::string value = prop.second;
        
        // 处理各种特殊语法
        value = processTemplateVariables(value);
        value = processCustomVariables(value);
        value = processResponsiveValues(value);
        value = processPropertyReferences(value);
        
        // 处理属性运算
        if (value.find('+') != std::string::npos || 
            value.find('-') != std::string::npos ||
            value.find('*') != std::string::npos ||
            value.find('/') != std::string::npos) {
            value = generatePropertyCalculation(value);
        }
        
        ss << prop.first << ": " << value << "; ";
    }
    
    return ss.str();
}

std::string CHTLGeneratorComplete::processStyleInheritance(const std::vector<std::string>& inheritedGroups) const {
    std::stringstream ss;
    
    for (const auto& group : inheritedGroups) {
        ss << "inherit(" << group << ") ";
    }
    
    return ss.str();
}

std::string CHTLGeneratorComplete::processStyleDeletion(const std::vector<std::string>& deletedProperties) const {
    std::stringstream ss;
    
    for (const auto& prop : deletedProperties) {
        ss << "delete " << prop << "; ";
    }
    
    return ss.str();
}

std::string CHTLGeneratorComplete::processArithmeticExpression(const std::string& expression) const {
    return generatePropertyCalculation(expression);
}

std::string CHTLGeneratorComplete::processConditionalExpression(const std::string& condition, 
                                                               const std::string& trueValue, 
                                                               const std::string& falseValue) const {
    return generateConditionalProperty(condition, trueValue, falseValue);
}

std::string CHTLGeneratorComplete::processSelector(const std::string& selector) const {
    // 处理选择器，包括上下文推导
    if (selector == "&") {
        return "&"; // 上下文推导，由调用者处理
    }
    return selector;
}

std::string CHTLGeneratorComplete::processContextualSelector(const std::string& context, 
                                                           const std::string& selector) const {
    if (selector == "&") {
        return context;
    }
    return context + " " + selector;
}

std::string CHTLGeneratorComplete::processUnits(const std::string& value) const {
    // 处理单位
    return value;
}

std::string CHTLGeneratorComplete::convertUnits(const std::string& value, 
                                               const std::string& fromUnit, 
                                               const std::string& toUnit) const {
    return generateUnitConversion(value, fromUnit, toUnit);
}

bool CHTLGeneratorComplete::validatePropertyValue(const std::string& property, const std::string& value) const {
    // 简单的属性值验证
    if (property == "color") {
        return value.find("#") == 0 || value.find("rgb") == 0 || value.find("hsl") == 0;
    } else if (property == "width" || property == "height") {
        return value.find("px") != std::string::npos || 
               value.find("em") != std::string::npos || 
               value.find("rem") != std::string::npos ||
               value.find("%") != std::string::npos;
    }
    return true;
}

bool CHTLGeneratorComplete::validateSelector(const std::string& selector) const {
    // 简单的选择器验证
    return !selector.empty() && selector != "&";
}

bool CHTLGeneratorComplete::validateExpression(const std::string& expression) const {
    // 简单的表达式验证
    return !expression.empty();
}

void CHTLGeneratorComplete::logError(const std::string& message, int line, int column) {
    std::stringstream ss;
    ss << "CHTLGeneratorComplete Error";
    if (line > 0) {
        ss << " at line " << line;
    }
    if (column > 0) {
        ss << ", column " << column;
    }
    ss << ": " << message;
    
    // 这里可以添加错误日志记录
}

} // namespace CHTL