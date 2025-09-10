#include "CSSCompiler.h"
#include <sstream>
#include <algorithm>
#include <cctype>
#include <regex>
#include <fstream>

namespace CHTL {

// CSSCompiler 实现
CSSCompiler::CSSCompiler() : debugMode(false), strictMode(false), minifyOutput(false), beautifyOutput(false) {
    initialize();
}

CSSCompiler::~CSSCompiler() {
    cleanup();
}

// 基本编译功能实现
CSSCompileResult CSSCompiler::compile(const std::string& cssCode) {
    CSSCompileResult result;
    result.success = false;
    
    if (!validateInput(cssCode)) {
        result.errors.push_back("Invalid CSS input");
        return result;
    }
    
    try {
        // 解析CSS规则
        auto rules = parseRules(cssCode);
        
        // 生成CSS
        result.css = generateCSS(rules);
        
        // 应用优化
        if (minifyOutput) {
            result.css = minifyCSS(result.css);
        } else if (beautifyOutput) {
            result.css = beautifyCSS(result.css);
        }
        
        result.success = true;
        result.errors = errors;
        result.warnings = warnings;
        
    } catch (const std::exception& e) {
        result.errors.push_back("CSS compilation error: " + std::string(e.what()));
        result.success = false;
    }
    
    return result;
}

CSSCompileResult CSSCompiler::compileFile(const std::string& filePath) {
    CSSCompileResult result;
    result.success = false;
    
    try {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            result.errors.push_back("Cannot open file: " + filePath);
            return result;
        }
        
        std::stringstream buffer;
        buffer << file.rdbuf();
        std::string cssCode = buffer.str();
        file.close();
        
        return compile(cssCode);
        
    } catch (const std::exception& e) {
        result.errors.push_back("File reading error: " + std::string(e.what()));
        return result;
    }
}

// 规则解析实现
std::vector<CSSRule> CSSCompiler::parseRules(const std::string& cssCode) {
    return parseRulesInternal(cssCode);
}

CSSRule CSSCompiler::parseRule(const std::string& ruleCode) {
    return parseRuleInternal(ruleCode);
}

// 选择器解析实现
std::string CSSCompiler::parseSelector(const std::string& selector) {
    return parseSelectorInternal(selector);
}

std::vector<std::string> CSSCompiler::parseComplexSelector(const std::string& selector) {
    return parseComplexSelectorInternal(selector);
}

// 属性解析实现
std::map<std::string, std::string> CSSCompiler::parseProperties(const std::string& propertiesCode) {
    return parsePropertiesInternal(propertiesCode);
}

std::pair<std::string, std::string> CSSCompiler::parseProperty(const std::string& propertyCode) {
    return parsePropertyInternal(propertyCode);
}

// CSS生成实现
std::string CSSCompiler::generateCSS(const std::vector<CSSRule>& rules) {
    return generateCSSInternal(rules);
}

std::string CSSCompiler::generateRule(const CSSRule& rule) {
    return generateRuleInternal(rule);
}

std::string CSSCompiler::generateSelector(const std::string& selector) {
    return generateSelectorInternal(selector);
}

std::string CSSCompiler::generateProperties(const std::map<std::string, std::string>& properties) {
    return generatePropertiesInternal(properties);
}

// CSS优化实现
std::string CSSCompiler::optimizeCSS(const std::string& css) {
    return optimizeCSSInternal(css);
}

std::string CSSCompiler::minifyCSS(const std::string& css) {
    return minifyCSSInternal(css);
}

std::string CSSCompiler::beautifyCSS(const std::string& css) {
    return beautifyCSSInternal(css);
}

// CSS验证实现
bool CSSCompiler::validateCSS(const std::string& css) {
    return validateCSSInternal(css);
}

bool CSSCompiler::validateRule(const CSSRule& rule) {
    return validateRuleInternal(rule);
}

bool CSSCompiler::validateSelector(const std::string& selector) {
    return validateSelectorInternal(selector);
}

bool CSSCompiler::validateProperty(const std::string& property, const std::string& value) {
    return validatePropertyInternal(property, value);
}

// CSS转换实现
std::string CSSCompiler::convertToSCSS(const std::string& css) {
    return convertToSCSSInternal(css);
}

std::string CSSCompiler::convertToLESS(const std::string& css) {
    return convertToLESSInternal(css);
}

std::string CSSCompiler::convertToSASS(const std::string& css) {
    return convertToSASSInternal(css);
}

std::string CSSCompiler::convertFromSCSS(const std::string& scss) {
    return convertFromSCSSInternal(scss);
}

std::string CSSCompiler::convertFromLESS(const std::string& less) {
    return convertFromLESSInternal(less);
}

std::string CSSCompiler::convertFromSASS(const std::string& sass) {
    return convertFromSASSInternal(sass);
}

// CSS合并实现
std::string CSSCompiler::mergeCSS(const std::vector<std::string>& cssFiles) {
    return mergeCSSInternal(cssFiles);
}

std::string CSSCompiler::mergeRules(const std::vector<CSSRule>& rules) {
    return mergeRulesInternal(rules);
}

// CSS分割实现
std::vector<std::string> CSSCompiler::splitCSS(const std::string& css) {
    return splitCSSInternal(css);
}

std::vector<CSSRule> CSSCompiler::splitRules(const std::string& css) {
    return splitRulesInternal(css);
}

// CSS过滤实现
std::string CSSCompiler::filterCSS(const std::string& css, const std::string& selector) {
    return filterCSSInternal(css, selector);
}

std::vector<CSSRule> CSSCompiler::filterRules(const std::vector<CSSRule>& rules, const std::string& selector) {
    return filterRulesInternal(rules, selector);
}

// CSS排序实现
std::string CSSCompiler::sortCSS(const std::string& css) {
    return sortCSSInternal(css);
}

std::vector<CSSRule> CSSCompiler::sortRules(const std::vector<CSSRule>& rules) {
    return sortRulesInternal(rules);
}

// CSS去重实现
std::string CSSCompiler::uniqueCSS(const std::string& css) {
    return uniqueCSSInternal(css);
}

std::vector<CSSRule> CSSCompiler::uniqueRules(const std::vector<CSSRule>& rules) {
    return uniqueRulesInternal(rules);
}

// CSS统计实现
std::map<std::string, int> CSSCompiler::getStatistics(const std::string& css) {
    return getStatisticsInternal(css);
}

std::map<std::string, int> CSSCompiler::getRuleStatistics(const std::vector<CSSRule>& rules) {
    return getRuleStatisticsInternal(rules);
}

// CSS调试实现
std::string CSSCompiler::debugCSS(const std::string& css) {
    return debugCSSInternal(css);
}

std::string CSSCompiler::debugRule(const CSSRule& rule) {
    return debugRuleInternal(rule);
}

// 错误处理实现
void CSSCompiler::clearErrors() {
    errors.clear();
}

void CSSCompiler::clearWarnings() {
    warnings.clear();
}

void CSSCompiler::clearMessages() {
    clearErrors();
    clearWarnings();
}

// 工具函数实现
std::string CSSCompiler::normalizeSelector(const std::string& selector) {
    return normalizeSelectorInternal(selector);
}

std::string CSSCompiler::normalizeProperty(const std::string& property) {
    return normalizePropertyInternal(property);
}

std::string CSSCompiler::normalizeValue(const std::string& value) {
    return normalizeValueInternal(value);
}

bool CSSCompiler::isValidSelector(const std::string& selector) {
    return isValidSelectorInternal(selector);
}

bool CSSCompiler::isValidProperty(const std::string& property) {
    return isValidPropertyInternal(property);
}

bool CSSCompiler::isValidValue(const std::string& value) {
    return isValidValueInternal(value);
}

// 序列化实现
std::string CSSCompiler::toJSON() const {
    return toJSONInternal();
}

std::string CSSCompiler::toXML() const {
    return toXMLInternal();
}

std::string CSSCompiler::toYAML() const {
    return toYAMLInternal();
}

std::string CSSCompiler::toString() const {
    return toStringInternal();
}

std::string CSSCompiler::toDebugString() const {
    return toDebugStringInternal();
}

// 反序列化实现
bool CSSCompiler::fromJSON(const std::string& json) {
    return fromJSONInternal(json);
}

bool CSSCompiler::fromXML(const std::string& xml) {
    return fromXMLInternal(xml);
}

bool CSSCompiler::fromYAML(const std::string& yaml) {
    return fromYAMLInternal(yaml);
}

bool CSSCompiler::fromString(const std::string& str) {
    return fromStringInternal(str);
}

// 克隆实现
std::unique_ptr<CSSCompiler> CSSCompiler::clone() const {
    return cloneInternal();
}

// 比较实现
bool CSSCompiler::equals(const CSSCompiler& other) const {
    return equalsInternal(other);
}

bool CSSCompiler::equals(const std::string& css) const {
    return equalsInternal(css);
}

// 格式化实现
std::string CSSCompiler::format() const {
    return formatInternal();
}

std::string CSSCompiler::minify() const {
    return minifyInternal();
}

std::string CSSCompiler::beautify() const {
    return beautifyInternal();
}

// 内部方法实现
void CSSCompiler::initialize() {
    clearMessages();
}

void CSSCompiler::cleanup() {
    clearMessages();
}

bool CSSCompiler::validateInput(const std::string& css) const {
    return !css.empty();
}

// 规则解析内部方法实现
std::vector<CSSRule> CSSCompiler::parseRulesInternal(const std::string& cssCode) {
    std::vector<CSSRule> rules;
    
    // 简化实现：基本的CSS规则解析
    std::regex ruleRegex(R"(([^{]+)\{([^}]+)\})");
    std::sregex_iterator iter(cssCode.begin(), cssCode.end(), ruleRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        CSSRule rule;
        rule.selector = iter->str(1);
        std::string propertiesStr = iter->str(2);
        rule.properties = parsePropertiesInternal(propertiesStr);
        rules.push_back(rule);
    }
    
    return rules;
}

CSSRule CSSCompiler::parseRuleInternal(const std::string& ruleCode) {
    CSSRule rule;
    
    size_t bracePos = ruleCode.find('{');
    if (bracePos != std::string::npos) {
        rule.selector = ruleCode.substr(0, bracePos);
        std::string propertiesStr = ruleCode.substr(bracePos + 1);
        size_t endBracePos = propertiesStr.find('}');
        if (endBracePos != std::string::npos) {
            propertiesStr = propertiesStr.substr(0, endBracePos);
            rule.properties = parsePropertiesInternal(propertiesStr);
        }
    }
    
    return rule;
}

// 选择器解析内部方法实现
std::string CSSCompiler::parseSelectorInternal(const std::string& selector) {
    // 简化实现：去除多余空格
    std::string result = selector;
    result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
    result = std::regex_replace(result, std::regex(R"(^\s+|\s+$)"), "");
    return result;
}

std::vector<std::string> CSSCompiler::parseComplexSelectorInternal(const std::string& selector) {
    std::vector<std::string> selectors;
    
    // 简化实现：按逗号分割
    std::stringstream ss(selector);
    std::string item;
    while (std::getline(ss, item, ',')) {
        selectors.push_back(parseSelectorInternal(item));
    }
    
    return selectors;
}

// 属性解析内部方法实现
std::map<std::string, std::string> CSSCompiler::parsePropertiesInternal(const std::string& propertiesCode) {
    std::map<std::string, std::string> properties;
    
    // 简化实现：基本的属性解析
    std::regex propertyRegex(R"(([^:]+):([^;]+);?)");
    std::sregex_iterator iter(propertiesCode.begin(), propertiesCode.end(), propertyRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string property = iter->str(1);
        std::string value = iter->str(2);
        
        // 去除空格
        property = std::regex_replace(property, std::regex(R"(^\s+|\s+$)"), "");
        value = std::regex_replace(value, std::regex(R"(^\s+|\s+$)"), "");
        
        properties[property] = value;
    }
    
    return properties;
}

std::pair<std::string, std::string> CSSCompiler::parsePropertyInternal(const std::string& propertyCode) {
    std::pair<std::string, std::string> result;
    
    size_t colonPos = propertyCode.find(':');
    if (colonPos != std::string::npos) {
        result.first = propertyCode.substr(0, colonPos);
        result.second = propertyCode.substr(colonPos + 1);
        
        // 去除空格
        result.first = std::regex_replace(result.first, std::regex(R"(^\s+|\s+$)"), "");
        result.second = std::regex_replace(result.second, std::regex(R"(^\s+|\s+$)"), "");
    }
    
    return result;
}

// CSS生成内部方法实现
std::string CSSCompiler::generateCSSInternal(const std::vector<CSSRule>& rules) {
    std::stringstream ss;
    
    for (const auto& rule : rules) {
        ss << generateRuleInternal(rule) << "\n";
    }
    
    return ss.str();
}

std::string CSSCompiler::generateRuleInternal(const CSSRule& rule) {
    std::stringstream ss;
    
    ss << rule.selector << " {\n";
    for (const auto& property : rule.properties) {
        ss << "  " << property.first << ": " << property.second << ";\n";
    }
    ss << "}";
    
    return ss.str();
}

std::string CSSCompiler::generateSelectorInternal(const std::string& selector) {
    return parseSelectorInternal(selector);
}

std::string CSSCompiler::generatePropertiesInternal(const std::map<std::string, std::string>& properties) {
    std::stringstream ss;
    
    for (const auto& property : properties) {
        ss << property.first << ": " << property.second << "; ";
    }
    
    return ss.str();
}

// CSS优化内部方法实现
std::string CSSCompiler::optimizeCSSInternal(const std::string& css) {
    std::string result = css;
    
    // 移除多余空格和换行
    result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
    
    // 移除注释
    result = std::regex_replace(result, std::regex(R"(/\*.*?\*/)"), "");
    
    // 移除不必要的分号
    result = std::regex_replace(result, std::regex(R"(\s*;\s*}"), "}");
    
    // 移除0值的单位
    result = std::regex_replace(result, std::regex(R"(\b0px\b)"), "0");
    result = std::regex_replace(result, std::regex(R"(\b0em\b)"), "0");
    result = std::regex_replace(result, std::regex(R"(\b0rem\b)"), "0");
    result = std::regex_replace(result, std::regex(R"(\b0%\b)"), "0");
    
    // 合并相同的属性
    result = mergeDuplicateProperties(result);
    
    // 移除重复的选择器
    result = removeDuplicateSelectors(result);
    
    return result;
}

// 合并重复属性的辅助方法
std::string CSSCompiler::mergeDuplicateProperties(const std::string& css) {
    // 简化实现：移除重复的属性声明
    std::string result = css;
    
    // 使用正则表达式匹配属性声明
    std::regex propertyRegex(R"(([^{]+)\{([^}]+)\})");
    std::sregex_iterator iter(result.begin(), result.end(), propertyRegex);
    std::sregex_iterator end;
    
    std::map<std::string, std::set<std::string>> selectorProperties;
    
    for (; iter != end; ++iter) {
        std::string selector = iter->str(1);
        std::string properties = iter->str(2);
        
        // 解析属性
        std::regex propRegex(R"(([^:]+):([^;]+);?)");
        std::sregex_iterator propIter(properties.begin(), properties.end(), propRegex);
        std::sregex_iterator propEnd;
        
        for (; propIter != propEnd; ++propIter) {
            std::string prop = propIter->str(1);
            std::string value = propIter->str(2);
            
            // 去除空格
            prop = std::regex_replace(prop, std::regex(R"(\s+)"), "");
            value = std::regex_replace(value, std::regex(R"(\s+)"), " ");
            value = std::regex_replace(value, std::regex(R"(^\s+|\s+$)"), "");
            
            selectorProperties[selector].insert(prop + ": " + value);
        }
    }
    
    // 重新构建CSS
    std::ostringstream oss;
    for (const auto& pair : selectorProperties) {
        oss << pair.first << " {\n";
        for (const auto& prop : pair.second) {
            oss << "  " << prop << ";\n";
        }
        oss << "}\n";
    }
    
    return oss.str();
}

// 移除重复选择器的辅助方法
std::string CSSCompiler::removeDuplicateSelectors(const std::string& css) {
    // 简化实现：移除重复的选择器
    std::set<std::string> seenSelectors;
    std::string result;
    
    std::regex ruleRegex(R"(([^{]+)\{([^}]+)\})");
    std::sregex_iterator iter(css.begin(), css.end(), ruleRegex);
    std::sregex_iterator end;
    
    for (; iter != end; ++iter) {
        std::string selector = iter->str(1);
        std::string properties = iter->str(2);
        
        // 去除空格
        selector = std::regex_replace(selector, std::regex(R"(\s+)"), " ");
        selector = std::regex_replace(selector, std::regex(R"(^\s+|\s+$)"), "");
        
        if (seenSelectors.find(selector) == seenSelectors.end()) {
            seenSelectors.insert(selector);
            result += selector + " {" + properties + "}\n";
        }
    }
    
    return result;
}

std::string CSSCompiler::minifyCSSInternal(const std::string& css) {
    std::string result = css;
    
    // 移除所有空格和换行
    result = std::regex_replace(result, std::regex(R"(\s+)"), "");
    
    // 移除注释
    result = std::regex_replace(result, std::regex(R"(/\*.*?\*/)"), "");
    
    return result;
}

std::string CSSCompiler::beautifyCSSInternal(const std::string& css) {
    std::string result = css;
    
    // 基本美化：添加换行和缩进
    result = std::regex_replace(result, std::regex(R"(\{)"), " {\n  ");
    result = std::regex_replace(result, std::regex(R"(\})"), "\n}\n");
    result = std::regex_replace(result, std::regex(R"(;)"), ";\n  ");
    
    return result;
}

// CSS验证内部方法实现
bool CSSCompiler::validateCSSInternal(const std::string& css) {
    // 简化实现：检查基本语法
    return css.find('{') != std::string::npos && css.find('}') != std::string::npos;
}

bool CSSCompiler::validateRuleInternal(const CSSRule& rule) {
    return !rule.selector.empty() && !rule.properties.empty();
}

bool CSSCompiler::validateSelectorInternal(const std::string& selector) {
    return !selector.empty() && selector.find('{') == std::string::npos;
}

bool CSSCompiler::validatePropertyInternal(const std::string& property, const std::string& value) {
    return !property.empty() && !value.empty();
}

// 其他内部方法的简化实现
std::string CSSCompiler::convertToSCSSInternal(const std::string& css) {
    return css; // 简化实现
}

std::string CSSCompiler::convertToLESSInternal(const std::string& css) {
    return css; // 简化实现
}

std::string CSSCompiler::convertToSASSInternal(const std::string& css) {
    return css; // 简化实现
}

std::string CSSCompiler::convertFromSCSSInternal(const std::string& scss) {
    return scss; // 简化实现
}

std::string CSSCompiler::convertFromLESSInternal(const std::string& less) {
    return less; // 简化实现
}

std::string CSSCompiler::convertFromSASSInternal(const std::string& sass) {
    return sass; // 简化实现
}

std::string CSSCompiler::mergeCSSInternal(const std::vector<std::string>& cssFiles) {
    std::stringstream ss;
    for (const auto& css : cssFiles) {
        ss << css << "\n";
    }
    return ss.str();
}

std::string CSSCompiler::mergeRulesInternal(const std::vector<CSSRule>& rules) {
    return generateCSSInternal(rules);
}

std::vector<std::string> CSSCompiler::splitCSSInternal(const std::string& css) {
    std::vector<std::string> result;
    result.push_back(css); // 简化实现
    return result;
}

std::vector<CSSRule> CSSCompiler::splitRulesInternal(const std::string& css) {
    return parseRulesInternal(css);
}

std::string CSSCompiler::filterCSSInternal(const std::string& css, const std::string& selector) {
    return css; // 简化实现
}

std::vector<CSSRule> CSSCompiler::filterRulesInternal(const std::vector<CSSRule>& rules, const std::string& selector) {
    std::vector<CSSRule> result;
    for (const auto& rule : rules) {
        if (rule.selector.find(selector) != std::string::npos) {
            result.push_back(rule);
        }
    }
    return result;
}

std::string CSSCompiler::sortCSSInternal(const std::string& css) {
    return css; // 简化实现
}

std::vector<CSSRule> CSSCompiler::sortRulesInternal(const std::vector<CSSRule>& rules) {
    return rules; // 简化实现
}

std::string CSSCompiler::uniqueCSSInternal(const std::string& css) {
    return css; // 简化实现
}

std::vector<CSSRule> CSSCompiler::uniqueRulesInternal(const std::vector<CSSRule>& rules) {
    return rules; // 简化实现
}

std::map<std::string, int> CSSCompiler::getStatisticsInternal(const std::string& css) {
    std::map<std::string, int> stats;
    stats["length"] = css.length();
    stats["lines"] = std::count(css.begin(), css.end(), '\n') + 1;
    stats["rules"] = std::count(css.begin(), css.end(), '{');
    return stats;
}

std::map<std::string, int> CSSCompiler::getRuleStatisticsInternal(const std::vector<CSSRule>& rules) {
    std::map<std::string, int> stats;
    stats["count"] = rules.size();
    return stats;
}

std::string CSSCompiler::debugCSSInternal(const std::string& css) {
    return "Debug CSS: " + css;
}

std::string CSSCompiler::debugRuleInternal(const CSSRule& rule) {
    return "Debug Rule: " + rule.selector;
}

// 工具函数内部方法实现
std::string CSSCompiler::normalizeSelectorInternal(const std::string& selector) {
    return parseSelectorInternal(selector);
}

std::string CSSCompiler::normalizePropertyInternal(const std::string& property) {
    std::string result = property;
    result = std::regex_replace(result, std::regex(R"(^\s+|\s+$)"), "");
    return result;
}

std::string CSSCompiler::normalizeValueInternal(const std::string& value) {
    std::string result = value;
    result = std::regex_replace(result, std::regex(R"(^\s+|\s+$)"), "");
    return result;
}

bool CSSCompiler::isValidSelectorInternal(const std::string& selector) {
    return !selector.empty() && selector.find('{') == std::string::npos;
}

bool CSSCompiler::isValidPropertyInternal(const std::string& property) {
    return !property.empty() && property.find(':') == std::string::npos;
}

bool CSSCompiler::isValidValueInternal(const std::string& value) {
    return !value.empty();
}

// 序列化内部方法实现
std::string CSSCompiler::toJSONInternal() const {
    return "{\"type\":\"CSSCompiler\",\"debugMode\":" + std::to_string(debugMode) + "}";
}

std::string CSSCompiler::toXMLInternal() const {
    return "<CSSCompiler><debugMode>" + std::to_string(debugMode) + "</debugMode></CSSCompiler>";
}

std::string CSSCompiler::toYAMLInternal() const {
    return "type: CSSCompiler\ndebugMode: " + std::to_string(debugMode);
}

std::string CSSCompiler::toStringInternal() const {
    return "CSSCompiler";
}

std::string CSSCompiler::toDebugStringInternal() const {
    return "CSSCompiler(debug=" + std::to_string(debugMode) + ")";
}

// 反序列化内部方法实现
bool CSSCompiler::fromJSONInternal(const std::string& json) {
    return true; // 简化实现
}

bool CSSCompiler::fromXMLInternal(const std::string& xml) {
    return true; // 简化实现
}

bool CSSCompiler::fromYAMLInternal(const std::string& yaml) {
    return true; // 简化实现
}

bool CSSCompiler::fromStringInternal(const std::string& str) {
    return true; // 简化实现
}

// 克隆内部方法实现
std::unique_ptr<CSSCompiler> CSSCompiler::cloneInternal() const {
    auto compiler = std::make_unique<CSSCompiler>();
    compiler->debugMode = debugMode;
    compiler->strictMode = strictMode;
    compiler->minifyOutput = minifyOutput;
    compiler->beautifyOutput = beautifyOutput;
    return compiler;
}

// 比较内部方法实现
bool CSSCompiler::equalsInternal(const CSSCompiler& other) const {
    return debugMode == other.debugMode && strictMode == other.strictMode;
}

bool CSSCompiler::equalsInternal(const std::string& css) const {
    return !css.empty();
}

// 格式化内部方法实现
std::string CSSCompiler::formatInternal() const {
    return toStringInternal();
}

std::string CSSCompiler::minifyInternal() const {
    return toStringInternal();
}

std::string CSSCompiler::beautifyInternal() const {
    return toStringInternal();
}

} // namespace CHTL