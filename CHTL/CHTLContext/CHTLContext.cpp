#include "CHTLContext.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {

CHTLContext::CHTLContext() : current_scope_("global") {
    scope_stack_.push_back("global");
    
    // 初始化默认编译选项
    setCompileOption("minify", false);
    setCompileOption("source_maps", true);
    setCompileOption("strict_mode", true);
    setCompileOption("warnings", true);
    setCompileOption("optimize", false);
}

CHTLContext::~CHTLContext() {
}

void CHTLContext::setVariable(const std::string& name, const std::string& value, 
                             VariableType type, int line, int column, const std::string& scope) {
    std::string actualScope = resolveScope(scope);
    std::string fullName = getFullVariableName(name, actualScope);
    
    VariableInfo varInfo(name, value, type, line, column, actualScope);
    
    // 检查是否已存在
    auto& vars = variables_[actualScope];
    for (auto& var : vars) {
        if (var.name == name) {
            var = varInfo; // 更新现有变量
            return;
        }
    }
    
    // 添加新变量
    vars.push_back(varInfo);
}

CHTLContext::VariableInfo* CHTLContext::getVariable(const std::string& name, const std::string& scope) {
    std::string actualScope = resolveScope(scope);
    
    // 在当前作用域查找
    auto it = variables_.find(actualScope);
    if (it != variables_.end()) {
        for (auto& var : it->second) {
            if (var.name == name) {
                return &var;
            }
        }
    }
    
    // 在全局作用域查找
    if (actualScope != "global") {
        it = variables_.find("global");
        if (it != variables_.end()) {
            for (auto& var : it->second) {
                if (var.name == name) {
                    return &var;
                }
            }
        }
    }
    
    return nullptr;
}

bool CHTLContext::hasVariable(const std::string& name, const std::string& scope) const {
    std::string actualScope = resolveScope(scope);
    
    // 在当前作用域查找
    auto it = variables_.find(actualScope);
    if (it != variables_.end()) {
        for (const auto& var : it->second) {
            if (var.name == name) {
                return true;
            }
        }
    }
    
    // 在全局作用域查找
    if (actualScope != "global") {
        it = variables_.find("global");
        if (it != variables_.end()) {
            for (const auto& var : it->second) {
                if (var.name == name) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

void CHTLContext::removeVariable(const std::string& name, const std::string& scope) {
    std::string actualScope = resolveScope(scope);
    
    auto it = variables_.find(actualScope);
    if (it != variables_.end()) {
        auto& vars = it->second;
        vars.erase(std::remove_if(vars.begin(), vars.end(),
            [&name](const VariableInfo& var) { return var.name == name; }), vars.end());
    }
}

std::vector<CHTLContext::VariableInfo> CHTLContext::getVariablesByType(VariableType type) const {
    std::vector<VariableInfo> result;
    
    for (const auto& scopePair : variables_) {
        for (const auto& var : scopePair.second) {
            if (var.type == type) {
                result.push_back(var);
            }
        }
    }
    
    return result;
}

std::vector<CHTLContext::VariableInfo> CHTLContext::getVariablesByScope(const std::string& scope) const {
    std::vector<VariableInfo> result;
    
    auto it = variables_.find(scope);
    if (it != variables_.end()) {
        result = it->second;
    }
    
    return result;
}

void CHTLContext::specializeVariable(const std::string& parentName, const std::string& childName, 
                                    const std::string& value, int line, int column) {
    // 查找父变量
    VariableInfo* parent = getVariable(parentName);
    if (!parent) {
        reportError("Parent variable not found: " + parentName, line, column);
        return;
    }
    
    // 创建特例化变量
    VariableInfo childVar(childName, value, parent->type, line, column, parent->scope);
    childVar.isSpecialized = true;
    childVar.parentName = parentName;
    
    variables_[parent->scope].push_back(childVar);
}

std::vector<CHTLContext::VariableInfo> CHTLContext::getSpecializedVariables(const std::string& parentName) const {
    std::vector<VariableInfo> result;
    
    for (const auto& scopePair : variables_) {
        for (const auto& var : scopePair.second) {
            if (var.isSpecialized && var.parentName == parentName) {
                result.push_back(var);
            }
        }
    }
    
    return result;
}

void CHTLContext::setTemplate(const std::string& name, TemplateType type, 
                             const std::map<std::string, std::string>& properties,
                             const std::vector<std::shared_ptr<CHTLBaseNode>>& children,
                             int line, int column, const std::string& scope) {
    std::string actualScope = resolveScope(scope);
    
    TemplateInfo templateInfo(name, type, line, column, actualScope);
    templateInfo.properties = properties;
    templateInfo.children = children;
    
    // 检查是否已存在
    auto& templates = templates_[actualScope];
    for (auto& tmpl : templates) {
        if (tmpl.name == name) {
            tmpl = templateInfo; // 更新现有模板
            return;
        }
    }
    
    // 添加新模板
    templates.push_back(templateInfo);
}

CHTLContext::TemplateInfo* CHTLContext::getTemplate(const std::string& name, const std::string& scope) {
    std::string actualScope = resolveScope(scope);
    
    // 在当前作用域查找
    auto it = templates_.find(actualScope);
    if (it != templates_.end()) {
        for (auto& tmpl : it->second) {
            if (tmpl.name == name) {
                return &tmpl;
            }
        }
    }
    
    // 在全局作用域查找
    if (actualScope != "global") {
        it = templates_.find("global");
        if (it != templates_.end()) {
            for (auto& tmpl : it->second) {
                if (tmpl.name == name) {
                    return &tmpl;
                }
            }
        }
    }
    
    return nullptr;
}

bool CHTLContext::hasTemplate(const std::string& name, const std::string& scope) const {
    std::string actualScope = resolveScope(scope);
    
    // 在当前作用域查找
    auto it = templates_.find(actualScope);
    if (it != templates_.end()) {
        for (const auto& tmpl : it->second) {
            if (tmpl.name == name) {
                return true;
            }
        }
    }
    
    // 在全局作用域查找
    if (actualScope != "global") {
        it = templates_.find("global");
        if (it != templates_.end()) {
            for (const auto& tmpl : it->second) {
                if (tmpl.name == name) {
                    return true;
                }
            }
        }
    }
    
    return false;
}

void CHTLContext::removeTemplate(const std::string& name, const std::string& scope) {
    std::string actualScope = resolveScope(scope);
    
    auto it = templates_.find(actualScope);
    if (it != templates_.end()) {
        auto& templates = it->second;
        templates.erase(std::remove_if(templates.begin(), templates.end(),
            [&name](const TemplateInfo& tmpl) { return tmpl.name == name; }), templates.end());
    }
}

std::vector<CHTLContext::TemplateInfo> CHTLContext::getTemplatesByType(TemplateType type) const {
    std::vector<TemplateInfo> result;
    
    for (const auto& scopePair : templates_) {
        for (const auto& tmpl : scopePair.second) {
            if (tmpl.type == type) {
                result.push_back(tmpl);
            }
        }
    }
    
    return result;
}

std::vector<CHTLContext::TemplateInfo> CHTLContext::getTemplatesByScope(const std::string& scope) const {
    std::vector<TemplateInfo> result;
    
    auto it = templates_.find(scope);
    if (it != templates_.end()) {
        result = it->second;
    }
    
    return result;
}

void CHTLContext::specializeTemplate(const std::string& parentName, const std::string& childName,
                                    const std::map<std::string, std::string>& properties,
                                    const std::vector<std::shared_ptr<CHTLBaseNode>>& children,
                                    int line, int column) {
    // 查找父模板
    TemplateInfo* parent = getTemplate(parentName);
    if (!parent) {
        reportError("Parent template not found: " + parentName, line, column);
        return;
    }
    
    // 创建特例化模板
    TemplateInfo childTemplate(childName, parent->type, line, column, parent->scope);
    childTemplate.properties = properties;
    childTemplate.children = children;
    childTemplate.isSpecialized = true;
    childTemplate.parentName = parentName;
    
    templates_[parent->scope].push_back(childTemplate);
}

std::vector<CHTLContext::TemplateInfo> CHTLContext::getSpecializedTemplates(const std::string& parentName) const {
    std::vector<TemplateInfo> result;
    
    for (const auto& scopePair : templates_) {
        for (const auto& tmpl : scopePair.second) {
            if (tmpl.isSpecialized && tmpl.parentName == parentName) {
                result.push_back(tmpl);
            }
        }
    }
    
    return result;
}

void CHTLContext::setNamespace(const std::string& name, const std::string& scope) {
    std::string actualScope = resolveScope(scope);
    namespaces_[name] = NamespaceInfo(name, actualScope);
}

CHTLContext::NamespaceInfo* CHTLContext::getNamespace(const std::string& name, const std::string& scope) {
    auto it = namespaces_.find(name);
    if (it != namespaces_.end()) {
        return &it->second;
    }
    return nullptr;
}

bool CHTLContext::hasNamespace(const std::string& name, const std::string& scope) const {
    return namespaces_.find(name) != namespaces_.end();
}

void CHTLContext::removeNamespace(const std::string& name, const std::string& scope) {
    namespaces_.erase(name);
}

void CHTLContext::addNamespaceImport(const std::string& namespaceName, const std::string& importName, 
                                    const std::string& value, const std::string& scope) {
    auto it = namespaces_.find(namespaceName);
    if (it != namespaces_.end()) {
        it->second.imports[importName] = value;
    }
}

std::vector<CHTLContext::NamespaceInfo> CHTLContext::getAllNamespaces() const {
    std::vector<NamespaceInfo> result;
    for (const auto& pair : namespaces_) {
        result.push_back(pair.second);
    }
    return result;
}

void CHTLContext::setConfiguration(const std::string& name, const std::map<std::string, std::string>& settings,
                                  int line, int column, const std::string& scope) {
    std::string actualScope = resolveScope(scope);
    configurations_[name] = ConfigurationInfo(name, actualScope);
    configurations_[name].settings = settings;
    configurations_[name].line = line;
    configurations_[name].column = column;
}

CHTLContext::ConfigurationInfo* CHTLContext::getConfiguration(const std::string& name, const std::string& scope) {
    auto it = configurations_.find(name);
    if (it != configurations_.end()) {
        return &it->second;
    }
    return nullptr;
}

bool CHTLContext::hasConfiguration(const std::string& name, const std::string& scope) const {
    return configurations_.find(name) != configurations_.end();
}

void CHTLContext::removeConfiguration(const std::string& name, const std::string& scope) {
    configurations_.erase(name);
}

std::string CHTLContext::getConfigurationValue(const std::string& configName, const std::string& key, 
                                              const std::string& scope) const {
    auto it = configurations_.find(configName);
    if (it != configurations_.end()) {
        auto settingIt = it->second.settings.find(key);
        if (settingIt != it->second.settings.end()) {
            return settingIt->second;
        }
    }
    return "";
}

void CHTLContext::enterScope(const std::string& scopeName) {
    scope_stack_.push_back(scopeName);
    current_scope_ = scopeName;
}

void CHTLContext::exitScope() {
    if (scope_stack_.size() > 1) {
        scope_stack_.pop_back();
        current_scope_ = scope_stack_.back();
    }
}

std::string CHTLContext::getCurrentScope() const {
    return current_scope_;
}

std::vector<std::string> CHTLContext::getScopeHierarchy() const {
    return scope_stack_;
}

void CHTLContext::addPropertyReference(const std::string& selector, const std::string& property, 
                                      const std::string& value) {
    property_references_[selector][property] = value;
}

std::string CHTLContext::getPropertyReference(const std::string& selector, const std::string& property) const {
    auto it = property_references_.find(selector);
    if (it != property_references_.end()) {
        auto propIt = it->second.find(property);
        if (propIt != it->second.end()) {
            return propIt->second;
        }
    }
    return "";
}

bool CHTLContext::hasPropertyReference(const std::string& selector, const std::string& property) const {
    auto it = property_references_.find(selector);
    if (it != property_references_.end()) {
        return it->second.find(property) != it->second.end();
    }
    return false;
}

void CHTLContext::setResponsiveValue(const std::string& jsVarName, const std::string& value) {
    responsive_values_[jsVarName] = value;
}

std::string CHTLContext::getResponsiveValue(const std::string& jsVarName) const {
    auto it = responsive_values_.find(jsVarName);
    if (it != responsive_values_.end()) {
        return it->second;
    }
    return "";
}

bool CHTLContext::hasResponsiveValue(const std::string& jsVarName) const {
    return responsive_values_.find(jsVarName) != responsive_values_.end();
}

void CHTLContext::removeResponsiveValue(const std::string& jsVarName) {
    responsive_values_.erase(jsVarName);
}

void CHTLContext::addLocalStyle(const std::string& elementPath, const std::string& selector, 
                               const std::map<std::string, std::string>& properties) {
    local_styles_[elementPath][selector] = properties;
}

std::map<std::string, std::string> CHTLContext::getLocalStyle(const std::string& elementPath, 
                                                             const std::string& selector) const {
    auto pathIt = local_styles_.find(elementPath);
    if (pathIt != local_styles_.end()) {
        auto selectorIt = pathIt->second.find(selector);
        if (selectorIt != pathIt->second.end()) {
            return selectorIt->second;
        }
    }
    return {};
}

bool CHTLContext::hasLocalStyle(const std::string& elementPath, const std::string& selector) const {
    auto pathIt = local_styles_.find(elementPath);
    if (pathIt != local_styles_.end()) {
        return pathIt->second.find(selector) != pathIt->second.end();
    }
    return false;
}

void CHTLContext::setCompileOption(const std::string& option, bool value) {
    bool_options_[option] = value;
}

bool CHTLContext::getCompileOption(const std::string& option) const {
    auto it = bool_options_.find(option);
    if (it != bool_options_.end()) {
        return it->second;
    }
    return false;
}

void CHTLContext::setCompileOption(const std::string& option, const std::string& value) {
    string_options_[option] = value;
}

std::string CHTLContext::getCompileOptionString(const std::string& option) const {
    auto it = string_options_.find(option);
    if (it != string_options_.end()) {
        return it->second;
    }
    return "";
}

void CHTLContext::setErrorHandler(std::function<void(const std::string&, int, int)> handler) {
    error_handler_ = handler;
}

void CHTLContext::reportError(const std::string& message, int line, int column) {
    logError(message, line, column);
    
    if (error_handler_) {
        error_handler_(message, line, column);
    }
}

bool CHTLContext::hasErrors() const {
    return !errors_.empty();
}

const std::vector<std::string>& CHTLContext::getErrors() const {
    return errors_;
}

void CHTLContext::clearErrors() {
    errors_.clear();
}

void CHTLContext::printContext() const {
    std::cout << "=== CHTL Context ===" << std::endl;
    std::cout << "Current Scope: " << current_scope_ << std::endl;
    std::cout << "Variables: " << getVariableCount() << std::endl;
    std::cout << "Templates: " << getTemplateCount() << std::endl;
    std::cout << "Namespaces: " << getNamespaceCount() << std::endl;
    std::cout << "Configurations: " << getConfigurationCount() << std::endl;
    std::cout << "===================" << std::endl;
}

size_t CHTLContext::getVariableCount() const {
    size_t count = 0;
    for (const auto& pair : variables_) {
        count += pair.second.size();
    }
    return count;
}

size_t CHTLContext::getTemplateCount() const {
    size_t count = 0;
    for (const auto& pair : templates_) {
        count += pair.second.size();
    }
    return count;
}

size_t CHTLContext::getNamespaceCount() const {
    return namespaces_.size();
}

size_t CHTLContext::getConfigurationCount() const {
    return configurations_.size();
}

void CHTLContext::clear() {
    variables_.clear();
    templates_.clear();
    namespaces_.clear();
    configurations_.clear();
    property_references_.clear();
    responsive_values_.clear();
    local_styles_.clear();
    errors_.clear();
    
    scope_stack_.clear();
    scope_stack_.push_back("global");
    current_scope_ = "global";
}

void CHTLContext::clearScope(const std::string& scope) {
    variables_.erase(scope);
    templates_.erase(scope);
    
    // 清理引用属性
    for (auto it = property_references_.begin(); it != property_references_.end();) {
        if (it->first.find(scope) == 0) {
            it = property_references_.erase(it);
        } else {
            ++it;
        }
    }
    
    // 清理局部样式
    for (auto it = local_styles_.begin(); it != local_styles_.end();) {
        if (it->first.find(scope) == 0) {
            it = local_styles_.erase(it);
        } else {
            ++it;
        }
    }
}

std::string CHTLContext::getFullVariableName(const std::string& name, const std::string& scope) const {
    return scope + "::" + name;
}

std::string CHTLContext::getFullTemplateName(const std::string& name, const std::string& scope) const {
    return scope + "::" + name;
}

void CHTLContext::logError(const std::string& message, int line, int column) {
    std::stringstream ss;
    ss << "Error";
    if (line > 0) {
        ss << " at line " << line;
    }
    if (column > 0) {
        ss << ", column " << column;
    }
    ss << ": " << message;
    
    errors_.push_back(ss.str());
}

bool CHTLContext::isValidScope(const std::string& scope) const {
    return !scope.empty() && scope != "::";
}

std::string CHTLContext::resolveScope(const std::string& scope) const {
    if (scope.empty()) {
        return current_scope_;
    }
    return scope;
}

} // namespace CHTL