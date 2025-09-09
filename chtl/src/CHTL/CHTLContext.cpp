#include "CHTL/CHTLContext.h"
#include <algorithm>

namespace CHTL {

CHTLContext::CHTLContext() : debugMode_(false) {
    // 设置默认配置
    setConfiguration("INDEX_INITIAL_COUNT", "0");
    setConfiguration("DEBUG_MODE", "false");
    setConfiguration("DISABLE_STYLE_AUTO_ADD_CLASS", "false");
    setConfiguration("DISABLE_STYLE_AUTO_ADD_ID", "false");
    setConfiguration("DISABLE_DEFAULT_NAMESPACE", "false");
    setConfiguration("DISABLE_CUSTOM_ORIGIN_TYPE", "false");
}

void CHTLContext::setConfiguration(const std::string& key, const std::string& value) {
    configurations_[key] = value;
    
    // 特殊配置处理
    if (key == "DEBUG_MODE") {
        debugMode_ = (value == "true");
    }
}

std::string CHTLContext::getConfiguration(const std::string& key) const {
    auto it = configurations_.find(key);
    return (it != configurations_.end()) ? it->second : "";
}

void CHTLContext::addNamespace(const std::string& name, const std::string& content) {
    namespaces_[name] = content;
}

std::string CHTLContext::getNamespace(const std::string& name) const {
    auto it = namespaces_.find(name);
    return (it != namespaces_.end()) ? it->second : "";
}

void CHTLContext::addTemplate(const std::string& name, const std::string& type, const std::string& content) {
    templates_[type][name] = content;
}

std::string CHTLContext::getTemplate(const std::string& name, const std::string& type) const {
    auto typeIt = templates_.find(type);
    if (typeIt != templates_.end()) {
        auto nameIt = typeIt->second.find(name);
        if (nameIt != typeIt->second.end()) {
            return nameIt->second;
        }
    }
    return "";
}

void CHTLContext::addCustom(const std::string& name, const std::string& type, const std::string& content) {
    customs_[type][name] = content;
}

std::string CHTLContext::getCustom(const std::string& name, const std::string& type) const {
    auto typeIt = customs_.find(type);
    if (typeIt != customs_.end()) {
        auto nameIt = typeIt->second.find(name);
        if (nameIt != typeIt->second.end()) {
            return nameIt->second;
        }
    }
    return "";
}

void CHTLContext::setVariable(const std::string& name, const std::string& value) {
    variables_[name] = value;
}

std::string CHTLContext::getVariable(const std::string& name) const {
    auto it = variables_.find(name);
    return (it != variables_.end()) ? it->second : "";
}

void CHTLContext::addModule(const std::string& name, const std::string& path) {
    modules_[name] = path;
}

std::string CHTLContext::getModule(const std::string& name) const {
    auto it = modules_.find(name);
    return (it != modules_.end()) ? it->second : "";
}

void CHTLContext::setDebugMode(bool enabled) {
    debugMode_ = enabled;
    setConfiguration("DEBUG_MODE", enabled ? "true" : "false");
}

bool CHTLContext::isDebugMode() const {
    return debugMode_;
}

} // namespace CHTL