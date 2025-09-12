#include "CHTL/CHTLContext.h"
#include <iostream>

namespace CHTL {

CHTLContext::CHTLContext() : debug_mode_(false) {
    // 设置默认配置
    configurations_["INDEX_INITIAL_COUNT"] = "0";
    configurations_["DEBUG_MODE"] = "false";
    configurations_["DISABLE_STYLE_AUTO_ADD_CLASS"] = "false";
    configurations_["DISABLE_STYLE_AUTO_ADD_ID"] = "false";
    configurations_["DISABLE_DEFAULT_NAMESPACE"] = "false";
    configurations_["DISABLE_CUSTOM_ORIGIN_TYPE"] = "false";
}

CHTLContext::~CHTLContext() = default;

void CHTLContext::setConfiguration(const std::string& key, const std::string& value) {
    configurations_[key] = value;
    
    if (debug_mode_) {
        std::cout << "[CHTLContext] Set configuration: " << key << " = " << value << std::endl;
    }
}

std::string CHTLContext::getConfiguration(const std::string& key) const {
    auto it = configurations_.find(key);
    if (it != configurations_.end()) {
        return it->second;
    }
    return "";
}

void CHTLContext::addNamespace(const std::string& name, const std::string& parent) {
    if (parent.empty()) {
        namespaces_[name] = name;
    } else {
        namespaces_[name] = parent + "::" + name;
    }
    
    if (debug_mode_) {
        std::cout << "[CHTLContext] Added namespace: " << name 
                  << (parent.empty() ? "" : " (parent: " + parent + ")") << std::endl;
    }
}

bool CHTLContext::hasNamespace(const std::string& name) const {
    return namespaces_.find(name) != namespaces_.end();
}

std::string CHTLContext::getFullNamespace(const std::string& name) const {
    auto it = namespaces_.find(name);
    if (it != namespaces_.end()) {
        return it->second;
    }
    return name;
}

void CHTLContext::addTemplate(const std::string& type, const std::string& name, std::shared_ptr<CHTLNode> node) {
    templates_[type][name] = node;
    
    if (debug_mode_) {
        std::cout << "[CHTLContext] Added template: " << type << "::" << name << std::endl;
    }
}

std::shared_ptr<CHTLNode> CHTLContext::getTemplate(const std::string& type, const std::string& name) const {
    auto type_it = templates_.find(type);
    if (type_it != templates_.end()) {
        auto name_it = type_it->second.find(name);
        if (name_it != type_it->second.end()) {
            return name_it->second;
        }
    }
    return nullptr;
}

void CHTLContext::addCustom(const std::string& type, const std::string& name, std::shared_ptr<CHTLNode> node) {
    customs_[type][name] = node;
    
    if (debug_mode_) {
        std::cout << "[CHTLContext] Added custom: " << type << "::" << name << std::endl;
    }
}

std::shared_ptr<CHTLNode> CHTLContext::getCustom(const std::string& type, const std::string& name) const {
    auto type_it = customs_.find(type);
    if (type_it != customs_.end()) {
        auto name_it = type_it->second.find(name);
        if (name_it != type_it->second.end()) {
            return name_it->second;
        }
    }
    return nullptr;
}

void CHTLContext::addImport(const std::string& path, const std::string& alias) {
    if (alias.empty()) {
        // 使用路径作为别名
        imports_[path] = path;
    } else {
        imports_[alias] = path;
    }
    
    if (debug_mode_) {
        std::cout << "[CHTLContext] Added import: " << path 
                  << (alias.empty() ? "" : " (alias: " + alias + ")") << std::endl;
    }
}

std::string CHTLContext::getImportPath(const std::string& alias) const {
    auto it = imports_.find(alias);
    if (it != imports_.end()) {
        return it->second;
    }
    return "";
}

void CHTLContext::addConstraint(const std::string& element, const std::vector<std::string>& constraints) {
    constraints_[element] = constraints;
    
    if (debug_mode_) {
        std::cout << "[CHTLContext] Added constraints for " << element << ": ";
        for (const auto& constraint : constraints) {
            std::cout << constraint << " ";
        }
        std::cout << std::endl;
    }
}

bool CHTLContext::isConstraintViolated(const std::string& element, const std::string& constraint) const {
    auto it = constraints_.find(element);
    if (it != constraints_.end()) {
        const auto& element_constraints = it->second;
        return std::find(element_constraints.begin(), element_constraints.end(), constraint) 
               != element_constraints.end();
    }
    return false;
}

void CHTLContext::setDebugMode(bool enabled) {
    debug_mode_ = enabled;
    setConfiguration("DEBUG_MODE", enabled ? "true" : "false");
}

bool CHTLContext::isDebugMode() const {
    return debug_mode_;
}

void CHTLContext::clear() {
    configurations_.clear();
    namespaces_.clear();
    templates_.clear();
    customs_.clear();
    imports_.clear();
    constraints_.clear();
    
    // 重新设置默认配置
    setConfiguration("INDEX_INITIAL_COUNT", "0");
    setConfiguration("DEBUG_MODE", "false");
    setConfiguration("DISABLE_STYLE_AUTO_ADD_CLASS", "false");
    setConfiguration("DISABLE_STYLE_AUTO_ADD_ID", "false");
    setConfiguration("DISABLE_DEFAULT_NAMESPACE", "false");
    setConfiguration("DISABLE_CUSTOM_ORIGIN_TYPE", "false");
}

} // namespace CHTL