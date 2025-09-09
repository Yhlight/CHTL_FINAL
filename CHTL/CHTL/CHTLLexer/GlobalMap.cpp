#include "GlobalMap.hpp"
#include <algorithm>

namespace CHTL {

GlobalMap& GlobalMap::getInstance() {
    static GlobalMap instance;
    return instance;
}

void GlobalMap::addTemplate(const std::string& name, const std::string& type, const std::string& content) {
    templates_[name] = {type, content};
}

bool GlobalMap::hasTemplate(const std::string& name) const {
    return templates_.find(name) != templates_.end();
}

std::string GlobalMap::getTemplate(const std::string& name) const {
    auto it = templates_.find(name);
    return (it != templates_.end()) ? it->second.second : "";
}

std::vector<std::string> GlobalMap::getAllTemplates() const {
    std::vector<std::string> result;
    for (const auto& pair : templates_) {
        result.push_back(pair.first);
    }
    return result;
}

void GlobalMap::addCustom(const std::string& name, const std::string& type, const std::string& content) {
    customs_[name] = {type, content};
}

bool GlobalMap::hasCustom(const std::string& name) const {
    return customs_.find(name) != customs_.end();
}

std::string GlobalMap::getCustom(const std::string& name) const {
    auto it = customs_.find(name);
    return (it != customs_.end()) ? it->second.second : "";
}

std::vector<std::string> GlobalMap::getAllCustoms() const {
    std::vector<std::string> result;
    for (const auto& pair : customs_) {
        result.push_back(pair.first);
    }
    return result;
}

void GlobalMap::addVariable(const std::string& name, const std::string& value) {
    variables_[name] = value;
}

bool GlobalMap::hasVariable(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

std::string GlobalMap::getVariable(const std::string& name) const {
    auto it = variables_.find(name);
    return (it != variables_.end()) ? it->second : "";
}

std::vector<std::string> GlobalMap::getAllVariables() const {
    std::vector<std::string> result;
    for (const auto& pair : variables_) {
        result.push_back(pair.first);
    }
    return result;
}

void GlobalMap::addNamespace(const std::string& name, const std::string& parent) {
    namespaces_[name] = parent;
}

bool GlobalMap::hasNamespace(const std::string& name) const {
    return namespaces_.find(name) != namespaces_.end();
}

std::string GlobalMap::getNamespaceParent(const std::string& name) const {
    auto it = namespaces_.find(name);
    return (it != namespaces_.end()) ? it->second : "";
}

std::vector<std::string> GlobalMap::getNamespaceChildren(const std::string& name) const {
    std::vector<std::string> result;
    for (const auto& pair : namespaces_) {
        if (pair.second == name) {
            result.push_back(pair.first);
        }
    }
    return result;
}

void GlobalMap::setConfig(const std::string& key, const std::string& value) {
    configs_[key] = value;
}

std::string GlobalMap::getConfig(const std::string& key, const std::string& defaultValue) const {
    auto it = configs_.find(key);
    return (it != configs_.end()) ? it->second : defaultValue;
}

bool GlobalMap::hasConfig(const std::string& key) const {
    return configs_.find(key) != configs_.end();
}

void GlobalMap::addImport(const std::string& name, const std::string& path, const std::string& type) {
    imports_[name] = {path, type};
}

bool GlobalMap::hasImport(const std::string& name) const {
    return imports_.find(name) != imports_.end();
}

std::string GlobalMap::getImportPath(const std::string& name) const {
    auto it = imports_.find(name);
    return (it != imports_.end()) ? it->second.first : "";
}

std::string GlobalMap::getImportType(const std::string& name) const {
    auto it = imports_.find(name);
    return (it != imports_.end()) ? it->second.second : "";
}

void GlobalMap::addConstraint(const std::string& scope, const std::string& constraint) {
    constraints_[scope].push_back(constraint);
}

std::vector<std::string> GlobalMap::getConstraints(const std::string& scope) const {
    auto it = constraints_.find(scope);
    return (it != constraints_.end()) ? it->second : std::vector<std::string>();
}

bool GlobalMap::hasConstraint(const std::string& scope, const std::string& constraint) const {
    auto it = constraints_.find(scope);
    if (it != constraints_.end()) {
        return std::find(it->second.begin(), it->second.end(), constraint) != it->second.end();
    }
    return false;
}

void GlobalMap::clear() {
    templates_.clear();
    customs_.clear();
    variables_.clear();
    namespaces_.clear();
    configs_.clear();
    imports_.clear();
    constraints_.clear();
}

void GlobalMap::reset() {
    clear();
    // 设置默认配置
    setConfig("INDEX_INITIAL_COUNT", "0");
    setConfig("DEBUG_MODE", "false");
    setConfig("DISABLE_NAME_GROUP", "false");
    setConfig("DISABLE_STYLE_AUTO_ADD_CLASS", "false");
    setConfig("DISABLE_STYLE_AUTO_ADD_ID", "false");
    setConfig("DISABLE_DEFAULT_NAMESPACE", "false");
    setConfig("DISABLE_CUSTOM_ORIGIN_TYPE", "false");
    setConfig("DISABLE_SCRIPT_AUTO_ADD_CLASS", "true");
    setConfig("DISABLE_SCRIPT_AUTO_ADD_ID", "true");
}

} // namespace CHTL