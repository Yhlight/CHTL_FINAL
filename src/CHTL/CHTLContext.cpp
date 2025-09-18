#include "CHTLContext.h"

namespace CHTL {

CHTLContext::CHTLContext() {
    // 初始化默认配置
    configurations_["DEBUG_MODE"] = "false";
    configurations_["INDEX_INITIAL_COUNT"] = "0";
    configurations_["DISABLE_STYLE_AUTO_ADD_CLASS"] = "false";
    configurations_["DISABLE_STYLE_AUTO_ADD_ID"] = "false";
    configurations_["DISABLE_DEFAULT_NAMESPACE"] = "false";
    configurations_["DISABLE_CUSTOM_ORIGIN_TYPE"] = "false";
    configurations_["DISABLE_SCRIPT_AUTO_ADD_CLASS"] = "true";
    configurations_["DISABLE_SCRIPT_AUTO_ADD_ID"] = "true";
}

void CHTLContext::setConfiguration(const std::string& key, const std::string& value) {
    configurations_[key] = value;
}

std::string CHTLContext::getConfiguration(const std::string& key) const {
    auto it = configurations_.find(key);
    return (it != configurations_.end()) ? it->second : "";
}

bool CHTLContext::hasConfiguration(const std::string& key) const {
    return configurations_.find(key) != configurations_.end();
}

void CHTLContext::registerTemplate(const std::string& name, const std::string& content) {
    templates_[name] = content;
}

std::string CHTLContext::getTemplate(const std::string& name) const {
    auto it = templates_.find(name);
    return (it != templates_.end()) ? it->second : "";
}

bool CHTLContext::hasTemplate(const std::string& name) const {
    return templates_.find(name) != templates_.end();
}

void CHTLContext::registerCustom(const std::string& name, const std::string& content) {
    customs_[name] = content;
}

std::string CHTLContext::getCustom(const std::string& name) const {
    auto it = customs_.find(name);
    return (it != customs_.end()) ? it->second : "";
}

bool CHTLContext::hasCustom(const std::string& name) const {
    return customs_.find(name) != customs_.end();
}

void CHTLContext::enterNamespace(const std::string& name) {
    namespaceStack_.push_back(name);
}

void CHTLContext::exitNamespace() {
    if (!namespaceStack_.empty()) {
        namespaceStack_.pop_back();
    }
}

std::string CHTLContext::getCurrentNamespace() const {
    if (namespaceStack_.empty()) {
        return "";
    }
    return namespaceStack_.back();
}

void CHTLContext::addError(const std::string& error) {
    errors_.push_back(error);
}

void CHTLContext::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
}

void CHTLContext::clear() {
    configurations_.clear();
    templates_.clear();
    customs_.clear();
    namespaceStack_.clear();
    errors_.clear();
    warnings_.clear();
}

} // namespace CHTL