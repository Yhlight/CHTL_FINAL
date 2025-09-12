#include "CHTL/CHTLContext.h"
#include <algorithm>

namespace CHTL {

CHTLContext::CHTLContext() : debug_mode_(false) {
    // 初始化默认配置
    configurations_["INDEX_INITIAL_COUNT"] = "0";
    configurations_["DEBUG_MODE"] = "false";
    configurations_["DISABLE_STYLE_AUTO_ADD_CLASS"] = "false";
    configurations_["DISABLE_STYLE_AUTO_ADD_ID"] = "false";
    configurations_["DISABLE_DEFAULT_NAMESPACE"] = "false";
    configurations_["DISABLE_CUSTOM_ORIGIN_TYPE"] = "false";
    configurations_["DISABLE_SCRIPT_AUTO_ADD_CLASS"] = "true";
    configurations_["DISABLE_SCRIPT_AUTO_ADD_ID"] = "true";
}

void CHTLContext::setConfiguration(const std::string& key, const std::string& value) {
    configurations_[key] = value;
    if (key == "DEBUG_MODE") {
        debug_mode_ = (value == "true");
    }
}

std::string CHTLContext::getConfiguration(const std::string& key) const {
    auto it = configurations_.find(key);
    return (it != configurations_.end()) ? it->second : "";
}

bool CHTLContext::hasConfiguration(const std::string& key) const {
    return configurations_.find(key) != configurations_.end();
}

void CHTLContext::addNamespace(const std::string& name, const std::string& content) {
    namespaces_[name] = content;
}

std::string CHTLContext::getNamespace(const std::string& name) const {
    auto it = namespaces_.find(name);
    return (it != namespaces_.end()) ? it->second : "";
}

bool CHTLContext::hasNamespace(const std::string& name) const {
    return namespaces_.find(name) != namespaces_.end();
}

void CHTLContext::addTemplate(const std::string& name, const std::string& type, const std::string& content) {
    templates_[type][name] = content;
}

std::string CHTLContext::getTemplate(const std::string& name, const std::string& type) const {
    auto type_it = templates_.find(type);
    if (type_it != templates_.end()) {
        auto name_it = type_it->second.find(name);
        if (name_it != type_it->second.end()) {
            return name_it->second;
        }
    }
    return "";
}

bool CHTLContext::hasTemplate(const std::string& name, const std::string& type) const {
    auto type_it = templates_.find(type);
    if (type_it != templates_.end()) {
        return type_it->second.find(name) != type_it->second.end();
    }
    return false;
}

void CHTLContext::addCustom(const std::string& name, const std::string& type, const std::string& content) {
    customs_[type][name] = content;
}

std::string CHTLContext::getCustom(const std::string& name, const std::string& type) const {
    auto type_it = customs_.find(type);
    if (type_it != customs_.end()) {
        auto name_it = type_it->second.find(name);
        if (name_it != type_it->second.end()) {
            return name_it->second;
        }
    }
    return "";
}

bool CHTLContext::hasCustom(const std::string& name, const std::string& type) const {
    auto type_it = customs_.find(type);
    if (type_it != customs_.end()) {
        return type_it->second.find(name) != type_it->second.end();
    }
    return false;
}

void CHTLContext::addVariable(const std::string& name, const std::string& value) {
    variables_[name] = value;
}

std::string CHTLContext::getVariable(const std::string& name) const {
    auto it = variables_.find(name);
    return (it != variables_.end()) ? it->second : "";
}

bool CHTLContext::hasVariable(const std::string& name) const {
    return variables_.find(name) != variables_.end();
}

void CHTLContext::setDebugMode(bool enabled) {
    debug_mode_ = enabled;
    configurations_["DEBUG_MODE"] = enabled ? "true" : "false";
}

bool CHTLContext::isDebugMode() const {
    return debug_mode_;
}

void CHTLContext::clear() {
    configurations_.clear();
    namespaces_.clear();
    templates_.clear();
    customs_.clear();
    variables_.clear();
    debug_mode_ = false;
}

} // namespace CHTL