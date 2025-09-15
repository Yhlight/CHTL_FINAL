#include "CHTL/CHTLContext.h"
#include <iostream>
#include <fstream>
#include <sstream>

namespace CHTL {

CHTLContext::CHTLContext()
    : m_debugMode(false)
    , m_initialized(false)
{
    // 设置默认配置
    m_configs["INDEX_INITIAL_COUNT"] = "0";
    m_configs["DEBUG_MODE"] = "false";
    m_configs["DISABLE_STYLE_AUTO_ADD_CLASS"] = "false";
    m_configs["DISABLE_STYLE_AUTO_ADD_ID"] = "false";
    m_configs["DISABLE_DEFAULT_NAMESPACE"] = "false";
    m_configs["DISABLE_CUSTOM_ORIGIN_TYPE"] = "false";
    m_configs["DISABLE_SCRIPT_AUTO_ADD_CLASS"] = "true";
    m_configs["DISABLE_SCRIPT_AUTO_ADD_ID"] = "true";
}

CHTLContext::~CHTLContext() {
    cleanup();
}

bool CHTLContext::initialize(const std::string& configPath) {
    if (m_initialized) {
        return true;
    }
    
    // 如果提供了配置文件路径，尝试加载配置
    if (!configPath.empty()) {
        std::ifstream configFile(configPath);
        if (configFile.is_open()) {
            std::string line;
            while (std::getline(configFile, line)) {
                // 简单的键值对解析
                size_t pos = line.find('=');
                if (pos != std::string::npos) {
                    std::string key = line.substr(0, pos);
                    std::string value = line.substr(pos + 1);
                    
                    // 去除前后空格
                    key.erase(0, key.find_first_not_of(" \t"));
                    key.erase(key.find_last_not_of(" \t") + 1);
                    value.erase(0, value.find_first_not_of(" \t"));
                    value.erase(value.find_last_not_of(" \t") + 1);
                    
                    m_configs[key] = value;
                }
            }
            configFile.close();
        }
    }
    
    // 设置调试模式
    m_debugMode = (getConfig("DEBUG_MODE") == "true");
    
    m_initialized = true;
    return true;
}

void CHTLContext::cleanup() {
    m_sourceFiles.clear();
    m_modulePaths.clear();
    m_errors.clear();
    m_warnings.clear();
    m_initialized = false;
}

void CHTLContext::setConfig(const std::string& key, const std::string& value) {
    m_configs[key] = value;
    
    // 特殊配置项的处理
    if (key == "DEBUG_MODE") {
        m_debugMode = (value == "true");
    }
}

std::string CHTLContext::getConfig(const std::string& key, const std::string& defaultValue) const {
    auto it = m_configs.find(key);
    if (it != m_configs.end()) {
        return it->second;
    }
    return defaultValue;
}

bool CHTLContext::hasConfig(const std::string& key) const {
    return m_configs.find(key) != m_configs.end();
}

bool CHTLContext::addSourceFile(const std::string& filePath) {
    // 检查文件是否存在
    std::ifstream file(filePath);
    if (!file.good()) {
        addError("Source file not found: " + filePath);
        return false;
    }
    file.close();
    
    m_sourceFiles.push_back(filePath);
    return true;
}

const std::vector<std::string>& CHTLContext::getSourceFiles() const {
    return m_sourceFiles;
}

void CHTLContext::setOutputDirectory(const std::string& outputDir) {
    m_outputDirectory = outputDir;
}

const std::string& CHTLContext::getOutputDirectory() const {
    return m_outputDirectory;
}

void CHTLContext::addModulePath(const std::string& modulePath) {
    m_modulePaths.push_back(modulePath);
}

const std::vector<std::string>& CHTLContext::getModulePaths() const {
    return m_modulePaths;
}

void CHTLContext::addError(const std::string& error) {
    m_errors.push_back(error);
    if (m_debugMode) {
        std::cerr << "[ERROR] " << error << std::endl;
    }
}

void CHTLContext::addWarning(const std::string& warning) {
    m_warnings.push_back(warning);
    if (m_debugMode) {
        std::cout << "[WARNING] " << warning << std::endl;
    }
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

void CHTLContext::clearMessages() {
    m_errors.clear();
    m_warnings.clear();
}

void CHTLContext::setDebugMode(bool debug) {
    m_debugMode = debug;
    m_configs["DEBUG_MODE"] = debug ? "true" : "false";
}

bool CHTLContext::isDebugMode() const {
    return m_debugMode;
}

} // namespace CHTL