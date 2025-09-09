#include "CHTL/CHTLContext.h"
#include <algorithm>

namespace CHTL {

CHTLContext::CHTLContext() 
    : m_debugMode(false) {
}

void CHTLContext::setConfiguration(const std::string& key, const std::string& value) {
    m_configurations[key] = value;
}

std::string CHTLContext::getConfiguration(const std::string& key) const {
    auto it = m_configurations.find(key);
    return (it != m_configurations.end()) ? it->second : "";
}

bool CHTLContext::hasConfiguration(const std::string& key) const {
    return m_configurations.find(key) != m_configurations.end();
}

void CHTLContext::addError(const std::string& message, int line, int column) {
    std::string errorMsg = message;
    if (line >= 0) {
        errorMsg += " at line " + std::to_string(line);
    }
    if (column >= 0) {
        errorMsg += ", column " + std::to_string(column);
    }
    m_errors.push_back(errorMsg);
}

void CHTLContext::addWarning(const std::string& message, int line, int column) {
    std::string warningMsg = message;
    if (line >= 0) {
        warningMsg += " at line " + std::to_string(line);
    }
    if (column >= 0) {
        warningMsg += ", column " + std::to_string(column);
    }
    m_warnings.push_back(warningMsg);
}

bool CHTLContext::hasErrors() const {
    return !m_errors.empty();
}

const std::vector<std::string>& CHTLContext::getErrors() const {
    return m_errors;
}

const std::vector<std::string>& CHTLContext::getWarnings() const {
    return m_warnings;
}

void CHTLContext::clearErrors() {
    m_errors.clear();
    m_warnings.clear();
}

void CHTLContext::setDebugMode(bool enabled) {
    m_debugMode = enabled;
}

bool CHTLContext::isDebugMode() const {
    return m_debugMode;
}

void CHTLContext::setOutputPath(const std::string& path) {
    m_outputPath = path;
}

std::string CHTLContext::getOutputPath() const {
    return m_outputPath;
}

} // namespace CHTL