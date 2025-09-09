#include "CHTLState.h"

namespace CHTL {

CHTLState::CHTLState() : m_debugMode(false) {}

void CHTLState::setCurrentFile(const std::string& path) {
    m_currentFile = path;
}

void CHTLState::setOutputDirectory(const std::string& dir) {
    m_outputDirectory = dir;
}

void CHTLState::setDebugMode(bool debug) {
    m_debugMode = debug;
}

void CHTLState::addGlobalVariable(const std::string& name, const std::string& value) {
    m_globalVariables[name] = value;
}

std::string CHTLState::getGlobalVariable(const std::string& name) const {
    auto it = m_globalVariables.find(name);
    return it != m_globalVariables.end() ? it->second : "";
}

bool CHTLState::hasGlobalVariable(const std::string& name) const {
    return m_globalVariables.find(name) != m_globalVariables.end();
}

void CHTLState::addTemplate(const std::string& name, const std::string& templateData) {
    m_templates[name] = templateData;
}

std::string CHTLState::getTemplate(const std::string& name) const {
    auto it = m_templates.find(name);
    return it != m_templates.end() ? it->second : "";
}

bool CHTLState::hasTemplate(const std::string& name) const {
    return m_templates.find(name) != m_templates.end();
}

void CHTLState::addNamespace(const std::string& name, const std::string& namespaceData) {
    m_namespaces[name] = namespaceData;
}

std::string CHTLState::getNamespace(const std::string& name) const {
    auto it = m_namespaces.find(name);
    return it != m_namespaces.end() ? it->second : "";
}

bool CHTLState::hasNamespace(const std::string& name) const {
    return m_namespaces.find(name) != m_namespaces.end();
}

void CHTLState::clear() {
    m_currentFile.clear();
    m_outputDirectory.clear();
    m_globalVariables.clear();
    m_templates.clear();
    m_namespaces.clear();
}

} // namespace CHTL