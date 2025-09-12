#include "ChthollyModule.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {
namespace Module {

// 静态常量定义
const std::string ChthollyModule::MODULE_NAME = "Chtholly";
const std::string ChthollyModule::MODULE_VERSION = "1.0.0";
const std::string ChthollyModule::MODULE_DESCRIPTION = "CHTL官方核心模块，提供CHTL语言的核心功能";
const std::string ChthollyModule::MODULE_AUTHOR = "CHTL Team";
const std::string ChthollyModule::MODULE_LICENSE = "MIT";

ChthollyModule::ChthollyModule() : initialized(false) {
    // 初始化模块
}

ChthollyModule::~ChthollyModule() {
    cleanup();
}

// 模块信息实现
std::string ChthollyModule::getName() const {
    return MODULE_NAME;
}

std::string ChthollyModule::getVersion() const {
    return MODULE_VERSION;
}

std::string ChthollyModule::getDescription() const {
    return MODULE_DESCRIPTION;
}

std::string ChthollyModule::getAuthor() const {
    return MODULE_AUTHOR;
}

std::string ChthollyModule::getLicense() const {
    return MODULE_LICENSE;
}

// 模块管理实现
bool ChthollyModule::initialize() {
    if (initialized) {
        return true;
    }
    
    try {
        // 初始化默认配置
        configurations["debug"] = "false";
        configurations["minify"] = "true";
        configurations["beautify"] = "false";
        configurations["validate"] = "true";
        
        // 初始化默认模板
        templates["default"] = "<div>{{content}}</div>";
        templates["container"] = "<div class=\"container\">{{content}}</div>";
        templates["button"] = "<button class=\"btn\">{{text}}</button>";
        
        // 初始化默认脚本
        scripts["utils"] = "function utils() { return {}; }";
        scripts["helpers"] = "function helpers() { return {}; }";
        
        // 初始化默认组件
        components["header"] = "<header>{{title}}</header>";
        components["footer"] = "<footer>{{copyright}}</footer>";
        
        initialized = true;
        return true;
    } catch (const std::exception& e) {
        addError("初始化失败: " + std::string(e.what()));
        return false;
    }
}

bool ChthollyModule::isInitialized() const {
    return initialized;
}

void ChthollyModule::cleanup() {
    if (initialized) {
        templates.clear();
        scripts.clear();
        components.clear();
        configurations.clear();
        errors.clear();
        warnings.clear();
        initialized = false;
    }
}

// 核心API实现
std::string ChthollyModule::processCHTL(const std::string& chtlCode) const {
    if (!initialized) {
        addError("模块未初始化");
        return "";
    }
    
    try {
        // 简单的CHTL处理实现
        // 实际实现应该使用CHTL解析器和生成器
        std::string result = chtlCode;
        
        // 处理注释
        result = std::regex_replace(result, std::regex(R"(//.*$)"), "");
        result = std::regex_replace(result, std::regex(R"(/\*.*?\*/)"), "");
        
        // 处理文本节点
        result = std::regex_replace(result, std::regex(R"(text\s*\{\s*([^}]+)\s*\})"), "$1");
        
        // 处理元素节点
        result = std::regex_replace(result, std::regex(R"((\w+)\s*\{\s*([^}]+)\s*\})"), "<$1>$2</$1>");
        
        return result;
    } catch (const std::exception& e) {
        addError("处理CHTL代码失败: " + std::string(e.what()));
        return "";
    }
}

std::string ChthollyModule::processCHTLJS(const std::string& chtljsCode) const {
    if (!initialized) {
        addError("模块未初始化");
        return "";
    }
    
    try {
        // 简单的CHTL JS处理实现
        // 实际实现应该使用CHTL JS解析器和生成器
        std::string result = chtljsCode;
        
        // 处理增强选择器
        result = std::regex_replace(result, std::regex(R"(\{\{([^}]+)\}\})"), "document.querySelector('$1')");
        
        // 处理链式调用
        result = std::regex_replace(result, std::regex(R"(\->)"), ".");
        
        return result;
    } catch (const std::exception& e) {
        addError("处理CHTL JS代码失败: " + std::string(e.what()));
        return "";
    }
}

std::string ChthollyModule::processCSS(const std::string& cssCode) const {
    if (!initialized) {
        addError("模块未初始化");
        return "";
    }
    
    try {
        // 简单的CSS处理实现
        std::string result = cssCode;
        
        // 处理注释
        result = std::regex_replace(result, std::regex(R"(/\*.*?\*/)"), "");
        
        // 处理空白
        result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
        result = std::regex_replace(result, std::regex(R"(\s*\{\s*)"), " { ");
        result = std::regex_replace(result, std::regex(R"(\s*\}\s*)"), " } ");
        
        return trimString(result);
    } catch (const std::exception& e) {
        addError("处理CSS代码失败: " + std::string(e.what()));
        return "";
    }
}

std::string ChthollyModule::processJavaScript(const std::string& jsCode) const {
    if (!initialized) {
        addError("模块未初始化");
        return "";
    }
    
    try {
        // 简单的JavaScript处理实现
        std::string result = jsCode;
        
        // 处理注释
        result = std::regex_replace(result, std::regex(R"(//.*$)"), "");
        result = std::regex_replace(result, std::regex(R"(/\*.*?\*/)"), "");
        
        // 处理空白
        result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
        
        return trimString(result);
    } catch (const std::exception& e) {
        addError("处理JavaScript代码失败: " + std::string(e.what()));
        return "";
    }
}

// 模板功能实现
std::string ChthollyModule::createTemplate(const std::string& name, const std::string& content) {
    if (!initialized) {
        addError("模块未初始化");
        return "";
    }
    
    if (!validateTemplate(content)) {
        addError("无效的模板内容: " + name);
        return "";
    }
    
    templates[name] = content;
    return "模板创建成功: " + name;
}

std::string ChthollyModule::applyTemplate(const std::string& templateName, const std::map<std::string, std::string>& variables) const {
    if (!initialized) {
        addError("模块未初始化");
        return "";
    }
    
    auto it = templates.find(templateName);
    if (it == templates.end()) {
        addError("模板不存在: " + templateName);
        return "";
    }
    
    return processTemplateVariables(it->second, variables);
}

bool ChthollyModule::hasTemplate(const std::string& name) const {
    return templates.find(name) != templates.end();
}

std::vector<std::string> ChthollyModule::getTemplateNames() const {
    std::vector<std::string> names;
    for (const auto& pair : templates) {
        names.push_back(pair.first);
    }
    return names;
}

// 样式功能实现
std::string ChthollyModule::createStyle(const std::string& selector, const std::map<std::string, std::string>& properties) const {
    if (!initialized) {
        addError("模块未初始化");
        return "";
    }
    
    if (!validateStyle(properties)) {
        addError("无效的样式属性: " + selector);
        return "";
    }
    
    std::string style = selector + " {\n";
    style += processStyleProperties(properties);
    style += "\n}";
    
    return style;
}

std::string ChthollyModule::applyStyle(const std::string& element, const std::string& style) const {
    if (!initialized) {
        addError("模块未初始化");
        return "";
    }
    
    return "<style>\n" + style + "\n</style>\n" + element;
}

std::string ChthollyModule::generateCSS(const std::map<std::string, std::map<std::string, std::string>>& styles) const {
    if (!initialized) {
        addError("模块未初始化");
        return "";
    }
    
    std::ostringstream oss;
    for (const auto& style : styles) {
        oss << style.first << " {\n";
        for (const auto& property : style.second) {
            oss << "  " << property.first << ": " << property.second << ";\n";
        }
        oss << "}\n\n";
    }
    
    return oss.str();
}

// 脚本功能实现
std::string ChthollyModule::createScript(const std::string& name, const std::string& code) {
    if (!initialized) {
        addError("模块未初始化");
        return "";
    }
    
    if (!validateScript(code)) {
        addError("无效的脚本代码: " + name);
        return "";
    }
    
    scripts[name] = code;
    return "脚本创建成功: " + name;
}

std::string ChthollyModule::executeScript(const std::string& scriptName, const std::map<std::string, std::string>& parameters) const {
    if (!initialized) {
        addError("模块未初始化");
        return "";
    }
    
    auto it = scripts.find(scriptName);
    if (it == scripts.end()) {
        addError("脚本不存在: " + scriptName);
        return "";
    }
    
    return processScriptParameters(it->second, parameters);
}

bool ChthollyModule::hasScript(const std::string& name) const {
    return scripts.find(name) != scripts.end();
}

std::vector<std::string> ChthollyModule::getScriptNames() const {
    std::vector<std::string> names;
    for (const auto& pair : scripts) {
        names.push_back(pair.first);
    }
    return names;
}

// 组件功能实现
std::string ChthollyModule::createComponent(const std::string& name, const std::string& templateContent, 
                                           const std::string& style, const std::string& script) {
    if (!initialized) {
        addError("模块未初始化");
        return "";
    }
    
    if (!validateComponent(templateContent)) {
        addError("无效的组件模板: " + name);
        return "";
    }
    
    std::string component = "<!-- Component: " + name + " -->\n";
    component += "<style>\n" + style + "\n</style>\n";
    component += templateContent + "\n";
    component += "<script>\n" + script + "\n</script>";
    
    components[name] = component;
    return "组件创建成功: " + name;
}

std::string ChthollyModule::renderComponent(const std::string& componentName, const std::map<std::string, std::string>& props) const {
    if (!initialized) {
        addError("模块未初始化");
        return "";
    }
    
    auto it = components.find(componentName);
    if (it == components.end()) {
        addError("组件不存在: " + componentName);
        return "";
    }
    
    return processComponentProps(it->second, props);
}

bool ChthollyModule::hasComponent(const std::string& name) const {
    return components.find(name) != components.end();
}

std::vector<std::string> ChthollyModule::getComponentNames() const {
    std::vector<std::string> names;
    for (const auto& pair : components) {
        names.push_back(pair.first);
    }
    return names;
}

// 工具功能实现
std::string ChthollyModule::minifyCode(const std::string& code) const {
    if (!initialized) {
        addError("模块未初始化");
        return "";
    }
    
    std::string result = code;
    
    // 移除注释
        result = std::regex_replace(result, std::regex(R"(//.*$)"), "");
    result = std::regex_replace(result, std::regex(R"(/\*.*?\*/)"), "");
    
    // 移除多余空白
    result = std::regex_replace(result, std::regex(R"(\s+)"), " ");
    result = std::regex_replace(result, std::regex(R"(\s*([{}();,=])\s*)"), "$1");
    
    return trimString(result);
}

std::string ChthollyModule::beautifyCode(const std::string& code) const {
    if (!initialized) {
        addError("模块未初始化");
        return "";
    }
    
    std::string result = code;
    
    // 格式化大括号
    result = std::regex_replace(result, std::regex(R"(\{)"), " {\n");
    result = std::regex_replace(result, std::regex(R"(\})"), "\n}\n");
    
    // 格式化分号
    result = std::regex_replace(result, std::regex(R"(;)"), ";\n");
    
    // 添加缩进
    std::vector<std::string> lines = splitString(result, '\n');
    std::ostringstream oss;
    int indent = 0;
    
    for (const auto& line : lines) {
        std::string trimmed = trimString(line);
        if (trimmed.empty()) continue;
        
        if (trimmed == "}") indent--;
        
        for (int i = 0; i < indent; i++) {
            oss << "  ";
        }
        oss << trimmed << "\n";
        
        if (trimmed.back() == '{') indent++;
    }
    
    return oss.str();
}

std::string ChthollyModule::validateCode(const std::string& code) const {
    if (!initialized) {
        addError("模块未初始化");
        return "";
    }
    
    // 简单的代码验证
    if (code.empty()) {
        return "错误: 代码为空";
    }
    
    // 检查括号匹配
    int braceCount = 0;
    int parenCount = 0;
    int bracketCount = 0;
    
    for (char c : code) {
        switch (c) {
            case '{': braceCount++; break;
            case '}': braceCount--; break;
            case '(': parenCount++; break;
            case ')': parenCount--; break;
            case '[': bracketCount++; break;
            case ']': bracketCount--; break;
        }
    }
    
    if (braceCount != 0) return "错误: 大括号不匹配";
    if (parenCount != 0) return "错误: 圆括号不匹配";
    if (bracketCount != 0) return "错误: 方括号不匹配";
    
    return "验证通过";
}

std::map<std::string, std::string> ChthollyModule::analyzeCode(const std::string& code) const {
    std::map<std::string, std::string> analysis;
    
    if (!initialized) {
        analysis["error"] = "模块未初始化";
        return analysis;
    }
    
    analysis["length"] = std::to_string(code.length());
    analysis["lines"] = std::to_string(std::count(code.begin(), code.end(), '\n') + 1);
    analysis["words"] = std::to_string(std::count(code.begin(), code.end(), ' ') + 1);
    // 简单的计数实现
    analysis["functions"] = "0";
    analysis["classes"] = "0";
    analysis["variables"] = "0";
    
    return analysis;
}

// 配置功能实现
void ChthollyModule::setConfiguration(const std::string& key, const std::string& value) {
    configurations[key] = value;
}

std::string ChthollyModule::getConfiguration(const std::string& key) const {
    auto it = configurations.find(key);
    return (it != configurations.end()) ? it->second : "";
}

bool ChthollyModule::hasConfiguration(const std::string& key) const {
    return configurations.find(key) != configurations.end();
}

std::map<std::string, std::string> ChthollyModule::getAllConfigurations() const {
    return configurations;
}

// 错误处理实现
std::vector<std::string> ChthollyModule::getErrors() const {
    return errors;
}

std::vector<std::string> ChthollyModule::getWarnings() const {
    return warnings;
}

void ChthollyModule::addError(const std::string& error) const {
    errors.push_back(error);
}

void ChthollyModule::addWarning(const std::string& warning) const {
    warnings.push_back(warning);
}

void ChthollyModule::clearErrors() {
    errors.clear();
}

void ChthollyModule::clearWarnings() {
    warnings.clear();
}

// 统计信息实现
size_t ChthollyModule::getTemplateCount() const {
    return templates.size();
}

size_t ChthollyModule::getStyleCount() const {
    return 0; // 样式不单独存储
}

size_t ChthollyModule::getScriptCount() const {
    return scripts.size();
}

size_t ChthollyModule::getComponentCount() const {
    return components.size();
}

std::map<std::string, size_t> ChthollyModule::getStatistics() const {
    std::map<std::string, size_t> stats;
    stats["templates"] = getTemplateCount();
    stats["scripts"] = getScriptCount();
    stats["components"] = getComponentCount();
    stats["configurations"] = configurations.size();
    stats["errors"] = errors.size();
    stats["warnings"] = warnings.size();
    return stats;
}

// 内部方法实现
std::string ChthollyModule::processTemplateVariables(const std::string& templateContent, const std::map<std::string, std::string>& variables) const {
    std::string result = templateContent;
    
    for (const auto& variable : variables) {
        std::string placeholder = "{{" + variable.first + "}}";
        result = std::regex_replace(result, std::regex(placeholder), variable.second);
    }
    
    return result;
}

std::string ChthollyModule::processStyleProperties(const std::map<std::string, std::string>& properties) const {
    std::ostringstream oss;
    
    for (const auto& property : properties) {
        oss << "  " << property.first << ": " << property.second << ";\n";
    }
    
    return oss.str();
}

std::string ChthollyModule::processScriptParameters(const std::string& script, const std::map<std::string, std::string>& parameters) const {
    std::string result = script;
    
    for (const auto& parameter : parameters) {
        std::string placeholder = "{{" + parameter.first + "}}";
        result = std::regex_replace(result, std::regex(placeholder), parameter.second);
    }
    
    return result;
}

std::string ChthollyModule::processComponentProps(const std::string& component, const std::map<std::string, std::string>& props) const {
    return processTemplateVariables(component, props);
}

// 验证方法实现
bool ChthollyModule::validateTemplate(const std::string& templateContent) const {
    return !templateContent.empty() && templateContent.length() > 0;
}

bool ChthollyModule::validateStyle(const std::map<std::string, std::string>& properties) const {
    return !properties.empty();
}

bool ChthollyModule::validateScript(const std::string& script) const {
    return !script.empty() && script.length() > 0;
}

bool ChthollyModule::validateComponent(const std::string& component) const {
    return !component.empty() && component.length() > 0;
}

// 工具方法实现
std::string ChthollyModule::escapeString(const std::string& str) const {
    std::string result = str;
    result = std::regex_replace(result, std::regex(R"(\\)"), "\\\\");
    result = std::regex_replace(result, std::regex(R"(")"), "\\\"");
    result = std::regex_replace(result, std::regex(R"(')"), "\\'");
    return result;
}

std::string ChthollyModule::unescapeString(const std::string& str) const {
    std::string result = str;
    result = std::regex_replace(result, std::regex(R"(\\\\)"), "\\");
    result = std::regex_replace(result, std::regex(R"(\\")"), "\"");
    result = std::regex_replace(result, std::regex(R"(\\')"), "'");
    return result;
}

std::vector<std::string> ChthollyModule::splitString(const std::string& str, char delimiter) const {
    std::vector<std::string> parts;
    std::istringstream iss(str);
    std::string part;
    
    while (std::getline(iss, part, delimiter)) {
        parts.push_back(part);
    }
    
    return parts;
}

std::string ChthollyModule::joinString(const std::vector<std::string>& parts, const std::string& delimiter) const {
    std::ostringstream oss;
    
    for (size_t i = 0; i < parts.size(); ++i) {
        if (i > 0) oss << delimiter;
        oss << parts[i];
    }
    
    return oss.str();
}

std::string ChthollyModule::trimString(const std::string& str) const {
    size_t start = str.find_first_not_of(" \t\n\r");
    if (start == std::string::npos) return "";
    
    size_t end = str.find_last_not_of(" \t\n\r");
    return str.substr(start, end - start + 1);
}

bool ChthollyModule::startsWith(const std::string& str, const std::string& prefix) const {
    return str.length() >= prefix.length() && str.substr(0, prefix.length()) == prefix;
}

bool ChthollyModule::endsWith(const std::string& str, const std::string& suffix) const {
    return str.length() >= suffix.length() && str.substr(str.length() - suffix.length()) == suffix;
}

} // namespace Module
} // namespace CHTL