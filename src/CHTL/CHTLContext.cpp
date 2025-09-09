#include "CHTL/CHTLContext.h"
#include <algorithm>
#include <sstream>

namespace CHTL {

CHTLContext::CHTLContext() : m_debugMode(false) {
    // 初始化默认配置
    m_configurations["INDEX_INITIAL_COUNT"] = "0";
    m_configurations["DEBUG_MODE"] = "false";
    m_configurations["DISABLE_STYLE_AUTO_ADD_CLASS"] = "false";
    m_configurations["DISABLE_STYLE_AUTO_ADD_ID"] = "false";
    m_configurations["DISABLE_SCRIPT_AUTO_ADD_CLASS"] = "true";
    m_configurations["DISABLE_SCRIPT_AUTO_ADD_ID"] = "true";
    m_configurations["DISABLE_DEFAULT_NAMESPACE"] = "false";
    m_configurations["DISABLE_CUSTOM_ORIGIN_TYPE"] = "false";
}

CHTLContext::~CHTLContext() = default;

void CHTLContext::setConfiguration(const std::string& key, const std::string& value) {
    m_configurations[key] = value;
}

std::string CHTLContext::getConfiguration(const std::string& key) const {
    auto it = m_configurations.find(key);
    return (it != m_configurations.end()) ? it->second : "";
}

void CHTLContext::addNamespace(const std::string& name, const std::string& content) {
    m_namespaces[name] = content;
}

std::string CHTLContext::getNamespace(const std::string& name) const {
    auto it = m_namespaces.find(name);
    return (it != m_namespaces.end()) ? it->second : "";
}

void CHTLContext::addTemplate(const std::string& name, const std::string& type, const std::string& content) {
    m_templates[type][name] = content;
}

std::string CHTLContext::getTemplate(const std::string& name, const std::string& type) const {
    auto typeIt = m_templates.find(type);
    if (typeIt != m_templates.end()) {
        auto nameIt = typeIt->second.find(name);
        if (nameIt != typeIt->second.end()) {
            return nameIt->second;
        }
    }
    return "";
}

void CHTLContext::addCustomElement(const std::string& name, const std::string& content) {
    m_customElements[name] = content;
}

std::string CHTLContext::getCustomElement(const std::string& name) const {
    auto it = m_customElements.find(name);
    return (it != m_customElements.end()) ? it->second : "";
}

void CHTLContext::addVariable(const std::string& name, const std::string& value) {
    m_variables[name] = value;
}

std::string CHTLContext::getVariable(const std::string& name) const {
    auto it = m_variables.find(name);
    return (it != m_variables.end()) ? it->second : "";
}

void CHTLContext::addModule(const std::string& name, const std::string& path) {
    m_modules[name] = path;
}

std::string CHTLContext::getModulePath(const std::string& name) const {
    auto it = m_modules.find(name);
    return (it != m_modules.end()) ? it->second : "";
}

void CHTLContext::addError(const std::string& error) {
    m_errors.push_back(error);
}

const std::vector<std::string>& CHTLContext::getErrors() const {
    return m_errors;
}

bool CHTLContext::hasErrors() const {
    return !m_errors.empty();
}

void CHTLContext::setDebugMode(bool enabled) {
    m_debugMode = enabled;
    m_configurations["DEBUG_MODE"] = enabled ? "true" : "false";
}

bool CHTLContext::isDebugMode() const {
    return m_debugMode;
}

} // namespace CHTL