#include "CHTL/CHTLContext.h"
#include <algorithm>

namespace CHTL {

CHTLContext::CHTLContext()
    : m_debugMode(false)
    , m_indexInitialCount(0)
    , m_disableStyleAutoAddClass(false)
    , m_disableStyleAutoAddId(false)
    , m_disableDefaultNamespace(false)
    , m_outputDirectory(".")
    , m_officialModulePath(".")
    , m_currentNamespace("")
    , m_compilationState("idle")
{
}

void CHTLContext::setDebugMode(bool debug) {
    m_debugMode = debug;
}

bool CHTLContext::isDebugMode() const {
    return m_debugMode;
}

void CHTLContext::setIndexInitialCount(int count) {
    m_indexInitialCount = count;
}

int CHTLContext::getIndexInitialCount() const {
    return m_indexInitialCount;
}

void CHTLContext::setDisableStyleAutoAddClass(bool disable) {
    m_disableStyleAutoAddClass = disable;
}

bool CHTLContext::isDisableStyleAutoAddClass() const {
    return m_disableStyleAutoAddClass;
}

void CHTLContext::setDisableStyleAutoAddId(bool disable) {
    m_disableStyleAutoAddId = disable;
}

bool CHTLContext::isDisableStyleAutoAddId() const {
    return m_disableStyleAutoAddId;
}

void CHTLContext::setDisableDefaultNamespace(bool disable) {
    m_disableDefaultNamespace = disable;
}

bool CHTLContext::isDisableDefaultNamespace() const {
    return m_disableDefaultNamespace;
}

void CHTLContext::addSourceFile(const std::string& filePath) {
    if (std::find(m_sourceFiles.begin(), m_sourceFiles.end(), filePath) == m_sourceFiles.end()) {
        m_sourceFiles.push_back(filePath);
    }
}

const std::vector<std::string>& CHTLContext::getSourceFiles() const {
    return m_sourceFiles;
}

void CHTLContext::setOutputDirectory(const std::string& dir) {
    m_outputDirectory = dir;
}

const std::string& CHTLContext::getOutputDirectory() const {
    return m_outputDirectory;
}

void CHTLContext::addModulePath(const std::string& path) {
    if (std::find(m_modulePaths.begin(), m_modulePaths.end(), path) == m_modulePaths.end()) {
        m_modulePaths.push_back(path);
    }
}

const std::vector<std::string>& CHTLContext::getModulePaths() const {
    return m_modulePaths;
}

void CHTLContext::setOfficialModulePath(const std::string& path) {
    m_officialModulePath = path;
}

const std::string& CHTLContext::getOfficialModulePath() const {
    return m_officialModulePath;
}

void CHTLContext::setCurrentNamespace(const std::string& ns) {
    m_currentNamespace = ns;
}

const std::string& CHTLContext::getCurrentNamespace() const {
    return m_currentNamespace;
}

void CHTLContext::pushNamespace(const std::string& ns) {
    m_namespaceStack.push_back(ns);
    m_currentNamespace = ns;
}

void CHTLContext::popNamespace() {
    if (!m_namespaceStack.empty()) {
        m_namespaceStack.pop_back();
        m_currentNamespace = m_namespaceStack.empty() ? "" : m_namespaceStack.back();
    }
}

const std::vector<std::string>& CHTLContext::getNamespaceStack() const {
    return m_namespaceStack;
}

void CHTLContext::addError(const std::string& error) {
    m_errors.push_back(error);
}

void CHTLContext::addWarning(const std::string& warning) {
    m_warnings.push_back(warning);
}

const std::vector<std::string>& CHTLContext::getErrors() const {
    return m_errors;
}

const std::vector<std::string>& CHTLContext::getWarnings() const {
    return m_warnings;
}

bool CHTLContext::hasErrors() const {
    return !m_errors.empty();
}

void CHTLContext::clearErrors() {
    m_errors.clear();
    m_warnings.clear();
}

void CHTLContext::setCompilationState(const std::string& state) {
    m_compilationState = state;
}

const std::string& CHTLContext::getCompilationState() const {
    return m_compilationState;
}

} // namespace CHTL