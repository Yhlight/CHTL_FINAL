#include "CHTLValidator.h"
#include <chrono>
#include <algorithm>
#include <sstream>

namespace CHTL {

CHTLValidator::CHTLValidator() 
    : strictMode(false), enableWarnings(true), enableHints(true), 
      maxLevel(ValidationLevel::PERFORMANCE), debugMode(false) {
    resetStats();
}

CHTLValidator::CHTLValidator(bool strict, bool warnings, bool hints, ValidationLevel maxLevel)
    : strictMode(strict), enableWarnings(warnings), enableHints(hints), 
      maxLevel(maxLevel), debugMode(false) {
    resetStats();
}

std::vector<ValidationIssue> CHTLValidator::validate(std::shared_ptr<BaseNode> ast) {
    auto startTime = std::chrono::high_resolution_clock::now();
    
    clearIssues();
    contextStack.clear();
    templates.clear();
    customs.clear();
    variables.clear();
    usedIdentifiers.clear();
    definedIdentifiers.clear();
    
    if (!ast) {
        addIssue(ValidationSeverity::ERROR, "AST is null", 0, 0);
        return issues;
    }
    
    try {
        validateNode(ast);
        
        // 检查未使用的定义
        for (const auto& identifier : definedIdentifiers) {
            if (usedIdentifiers.find(identifier) == usedIdentifiers.end()) {
                addIssue(ValidationSeverity::WARNING, 
                        "Unused identifier: " + identifier, 0, 0);
            }
        }
        
    } catch (const std::exception& e) {
        addIssue(ValidationSeverity::ERROR, "Validation error: " + std::string(e.what()), 0, 0);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
    stats.validationTime = duration.count();
    
    return issues;
}

void CHTLValidator::validateNode(std::shared_ptr<BaseNode> node) {
    if (!node) return;
    
    pushContext(node);
    
    switch (node->getNodeType()) {
        case NodeType::ELEMENT:
            validateElement(node);
            break;
        case NodeType::TEMPLATE_STYLE:
        case NodeType::TEMPLATE_ELEMENT:
        case NodeType::TEMPLATE_VAR:
            validateTemplate(node);
            break;
        case NodeType::CUSTOM_STYLE:
        case NodeType::CUSTOM_ELEMENT:
        case NodeType::CUSTOM_VAR:
            validateCustom(node);
            break;
        case NodeType::STYLE:
            validateStyle(node);
            break;
        case NodeType::SCRIPT:
            validateScript(node);
            break;
        case NodeType::IMPORT:
            validateImport(node);
            break;
        case NodeType::NAMESPACE:
            validateNamespace(node);
            break;
        case NodeType::CONFIGURATION:
            validateConfiguration(node);
            break;
        default:
            break;
    }
    
    // 递归验证子节点
    for (const auto& child : node->getChildren()) {
        validateNode(child);
    }
    
    popContext();
}

void CHTLValidator::validateElement(std::shared_ptr<BaseNode> node) {
    // 验证元素名
    std::string elementName = node->getName();
    if (!isValidElementName(elementName)) {
        addIssue(ValidationSeverity::ERROR, 
                "Invalid element name: " + elementName, node);
    }
    
    // 验证属性
    for (const auto& attr : node->getAttributes()) {
        if (!isValidAttributeName(attr.first)) {
            addIssue(ValidationSeverity::ERROR, 
                    "Invalid attribute name: " + attr.first, node);
        }
        
        // 验证特殊属性
        if (attr.first == "class") {
            // 验证class属性值
            std::string classes = attr.second;
            if (classes.empty()) {
                addIssue(ValidationSeverity::WARNING, 
                        "Empty class attribute", node);
            }
        } else if (attr.first == "id") {
            // 验证id属性值
            std::string id = attr.second;
            if (id.empty()) {
                addIssue(ValidationSeverity::WARNING, 
                        "Empty id attribute", node);
            } else if (!std::isalpha(id[0])) {
                addIssue(ValidationSeverity::ERROR, 
                        "ID must start with a letter: " + id, node);
            }
        }
    }
    
    // 检查自闭合标签
    if (node->getChildCount() == 0) {
        // 检查是否为void元素
        static const std::set<std::string> voidElements = {
            "area", "base", "br", "col", "embed", "hr", "img", "input",
            "link", "meta", "param", "source", "track", "wbr"
        };
        
        if (voidElements.find(elementName) == voidElements.end()) {
            addIssue(ValidationSeverity::WARNING, 
                    "Non-void element without content: " + elementName, node);
        }
    }
}

void CHTLValidator::validateTemplate(std::shared_ptr<BaseNode> node) {
    std::string templateName = node->getAttribute("name");
    if (templateName.empty()) {
        addIssue(ValidationSeverity::ERROR, "Template name is required", node);
        return;
    }
    
    if (!isValidTemplateName(templateName)) {
        addIssue(ValidationSeverity::ERROR, 
                "Invalid template name: " + templateName, node);
        return;
    }
    
    // 检查重复定义
    if (templates.find(templateName) != templates.end()) {
        addIssue(ValidationSeverity::ERROR, 
                "Duplicate template definition: " + templateName, node);
    } else {
        templates[templateName] = node;
        definedIdentifiers.insert(templateName);
    }
    
    // 验证模板内容
    if (node->getChildCount() == 0) {
        addIssue(ValidationSeverity::WARNING, 
                "Empty template: " + templateName, node);
    }
}

void CHTLValidator::validateCustom(std::shared_ptr<BaseNode> node) {
    std::string customName = node->getAttribute("name");
    if (customName.empty()) {
        addIssue(ValidationSeverity::ERROR, "Custom name is required", node);
        return;
    }
    
    if (!isValidTemplateName(customName)) {
        addIssue(ValidationSeverity::ERROR, 
                "Invalid custom name: " + customName, node);
        return;
    }
    
    // 检查重复定义
    if (customs.find(customName) != customs.end()) {
        addIssue(ValidationSeverity::ERROR, 
                "Duplicate custom definition: " + customName, node);
    } else {
        customs[customName] = node;
        definedIdentifiers.insert(customName);
    }
}

void CHTLValidator::validateStyle(std::shared_ptr<BaseNode> node) {
    std::string content = node->getAttribute("content");
    if (content.empty()) {
        addIssue(ValidationSeverity::WARNING, "Empty style block", node);
        return;
    }
    
    // 验证CSS语法
    validateStyleRules(node);
}

void CHTLValidator::validateScript(std::shared_ptr<BaseNode> node) {
    std::string content = node->getAttribute("content");
    if (content.empty()) {
        addIssue(ValidationSeverity::WARNING, "Empty script block", node);
        return;
    }
    
    // 验证JavaScript语法（基础检查）
    if (content.find("{{") != std::string::npos) {
        // 检查CHTL JS语法
        if (content.find("{{") != content.find("}}")) {
            addIssue(ValidationSeverity::ERROR, 
                    "Unclosed CHTL JS selector", node);
        }
    }
}

void CHTLValidator::validateImport(std::shared_ptr<BaseNode> node) {
    std::string path = node->getAttribute("path");
    if (path.empty()) {
        addIssue(ValidationSeverity::ERROR, "Import path is required", node);
        return;
    }
    
    // 验证路径格式
    if (path.find("..") != std::string::npos) {
        addIssue(ValidationSeverity::WARNING, 
                "Relative path with '..' may be unsafe: " + path, node);
    }
}

void CHTLValidator::validateNamespace(std::shared_ptr<BaseNode> node) {
    std::string name = node->getAttribute("name");
    if (name.empty()) {
        addIssue(ValidationSeverity::ERROR, "Namespace name is required", node);
        return;
    }
    
    if (!isValidTemplateName(name)) {
        addIssue(ValidationSeverity::ERROR, 
                "Invalid namespace name: " + name, node);
    }
}

void CHTLValidator::validateConfiguration(std::shared_ptr<BaseNode> node) {
    // 验证配置项
    for (const auto& child : node->getChildren()) {
        if (child->getNodeType() == NodeType::ELEMENT) {
            std::string configName = child->getName();
            std::string configValue = child->getAttribute("value");
            
            if (configValue.empty()) {
                addIssue(ValidationSeverity::WARNING, 
                        "Configuration value is empty: " + configName, child);
            }
        }
    }
}

void CHTLValidator::validateStyleRules(std::shared_ptr<BaseNode> node) {
    std::string content = node->getAttribute("content");
    
    // 简单的CSS语法检查
    size_t braceCount = 0;
    bool inString = false;
    char stringDelimiter = '\0';
    
    for (size_t i = 0; i < content.length(); ++i) {
        char c = content[i];
        
        if (inString) {
            if (c == stringDelimiter && content[i-1] != '\\') {
                inString = false;
                stringDelimiter = '\0';
            }
        } else {
            if (c == '"' || c == '\'') {
                inString = true;
                stringDelimiter = c;
            } else if (c == '{') {
                braceCount++;
            } else if (c == '}') {
                braceCount--;
                if (braceCount < 0) {
                    addIssue(ValidationSeverity::ERROR, 
                            "Unmatched closing brace", node);
                    break;
                }
            }
        }
    }
    
    if (braceCount != 0) {
        addIssue(ValidationSeverity::ERROR, 
                "Unmatched braces in CSS", node);
    }
}

bool CHTLValidator::isValidElementName(const std::string& name) const {
    if (name.empty()) return false;
    
    // HTML5元素名规则
    if (!std::isalpha(name[0]) && name[0] != '_') {
        return false;
    }
    
    for (char c : name) {
        if (!std::isalnum(c) && c != '-' && c != '_') {
            return false;
        }
    }
    
    return true;
}

bool CHTLValidator::isValidAttributeName(const std::string& name) const {
    if (name.empty()) return false;
    
    for (char c : name) {
        if (!std::isalnum(c) && c != '-' && c != '_' && c != ':') {
            return false;
        }
    }
    
    return true;
}

bool CHTLValidator::isValidTemplateName(const std::string& name) const {
    if (name.empty()) return false;
    
    for (char c : name) {
        if (!std::isalnum(c) && c != '_') {
            return false;
        }
    }
    
    return true;
}

bool CHTLValidator::isValidVariableName(const std::string& name) const {
    if (name.empty()) return false;
    
    for (char c : name) {
        if (!std::isalnum(c) && c != '_') {
            return false;
        }
    }
    
    return true;
}

bool CHTLValidator::isValidCSSProperty(const std::string& property) const {
    // 简化的CSS属性验证
    static const std::set<std::string> validProperties = {
        "color", "background-color", "width", "height", "margin", "padding",
        "border", "font-size", "font-family", "display", "position", "top",
        "left", "right", "bottom", "z-index", "opacity", "transform"
    };
    
    return validProperties.find(property) != validProperties.end();
}

bool CHTLValidator::isValidCSSValue(const std::string& value) const {
    // 简化的CSS值验证
    if (value.empty()) return false;
    
    // 检查基本值类型
    if (value == "auto" || value == "inherit" || value == "initial" || value == "unset") {
        return true;
    }
    
    // 检查数值
    if (std::isdigit(value[0]) || value[0] == '-' || value[0] == '+') {
        return true;
    }
    
    // 检查颜色值
    if (value[0] == '#' || value.substr(0, 3) == "rgb" || value.substr(0, 4) == "rgba") {
        return true;
    }
    
    return true; // 简化实现
}

void CHTLValidator::addIssue(ValidationSeverity severity, const std::string& message, 
                           size_t line, size_t column, const std::string& context) {
    if (severity == ValidationSeverity::WARNING && !enableWarnings) return;
    if (severity == ValidationSeverity::HINT && !enableHints) return;
    
    issues.emplace_back(severity, message, line, column, context);
    
    switch (severity) {
        case ValidationSeverity::ERROR: stats.errorCount++; break;
        case ValidationSeverity::WARNING: stats.warningCount++; break;
        case ValidationSeverity::INFO: stats.infoCount++; break;
        case ValidationSeverity::HINT: stats.hintCount++; break;
    }
    
    stats.totalIssues++;
}

void CHTLValidator::addIssue(ValidationSeverity severity, const std::string& message, 
                           std::shared_ptr<BaseNode> node, const std::string& context) {
    addIssue(severity, message, node->getLine(), node->getColumn(), context);
}

void CHTLValidator::pushContext(std::shared_ptr<BaseNode> node) {
    contextStack.push_back(node);
}

void CHTLValidator::popContext() {
    if (!contextStack.empty()) {
        contextStack.pop_back();
    }
}

std::shared_ptr<BaseNode> CHTLValidator::getCurrentContext() const {
    if (contextStack.empty()) {
        return nullptr;
    }
    return contextStack.back();
}

void CHTLValidator::clearIssues() {
    issues.clear();
    resetStats();
}

size_t CHTLValidator::getIssueCount() const {
    return issues.size();
}

size_t CHTLValidator::getErrorCount() const {
    return stats.errorCount;
}

size_t CHTLValidator::getWarningCount() const {
    return stats.warningCount;
}

std::vector<ValidationIssue> CHTLValidator::getIssues() const {
    return issues;
}

std::vector<ValidationIssue> CHTLValidator::getIssues(ValidationSeverity severity) const {
    std::vector<ValidationIssue> filtered;
    for (const auto& issue : issues) {
        if (issue.severity == severity) {
            filtered.push_back(issue);
        }
    }
    return filtered;
}

CHTLValidator::ValidationStats CHTLValidator::getStats() const {
    return stats;
}

void CHTLValidator::resetStats() {
    stats = ValidationStats{};
}

void CHTLValidator::enableDebugMode(bool enable) {
    debugMode = enable;
}

std::string CHTLValidator::getDebugInfo() const {
    std::ostringstream oss;
    oss << "CHTLValidator Debug Info:\n";
    oss << "  Total Issues: " << stats.totalIssues << "\n";
    oss << "  Errors: " << stats.errorCount << "\n";
    oss << "  Warnings: " << stats.warningCount << "\n";
    oss << "  Info: " << stats.infoCount << "\n";
    oss << "  Hints: " << stats.hintCount << "\n";
    oss << "  Validation Time: " << stats.validationTime << "ms\n";
    return oss.str();
}

} // namespace CHTL