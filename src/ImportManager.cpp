#include "CHTL/ImportManager.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <filesystem>

namespace CHTL {

ImportManager::ImportManager(std::shared_ptr<CHTLContext> context)
    : m_context(context)
    , m_debugMode(false)
    , m_totalImports(0)
    , m_successfulImports(0)
    , m_failedImports(0)
{
    // 添加默认导入路径
    m_importPaths.push_back(".");
    m_importPaths.push_back("./imports");
    m_importPaths.push_back("./modules");
}

ImportManager::~ImportManager() {
}

void ImportManager::addImportPath(const std::string& path) {
    m_importPaths.push_back(path);
}

const std::vector<std::string>& ImportManager::getImportPaths() const {
    return m_importPaths;
}

bool ImportManager::resolveImport(std::shared_ptr<ImportNode> importNode) {
    if (!importNode) return false;
    
    m_totalImports++;
    
    if (m_debugMode) {
        std::cout << "[ImportManager] Resolving import: " << importNode->getFilePath() << std::endl;
    }
    
    // 检查是否已经处理过
    if (m_processedFiles.find(importNode->getFilePath()) != m_processedFiles.end()) {
        if (m_debugMode) {
            std::cout << "[ImportManager] File already processed: " << importNode->getFilePath() << std::endl;
        }
        return true;
    }
    
    // 根据导入类型解析
    bool success = false;
    switch (importNode->getImportType()) {
        case ImportType::CHTL:
            success = resolveCHTLImport(importNode);
            break;
        case ImportType::HTML:
            success = resolveHTMLImport(importNode);
            break;
        case ImportType::CSS:
            success = resolveCSSImport(importNode);
            break;
        case ImportType::JS:
            success = resolveJSImport(importNode);
            break;
        case ImportType::CMOD:
            success = resolveCMODImport(importNode);
            break;
        case ImportType::CJMOD:
            success = resolveCJMODImport(importNode);
            break;
        default:
            addError("Unknown import type: " + std::to_string(static_cast<int>(importNode->getImportType())));
            break;
    }
    
    if (success) {
        m_successfulImports++;
        m_processedFiles.insert(importNode->getFilePath());
    } else {
        m_failedImports++;
    }
    
    return success;
}

bool ImportManager::resolveAllImports(const std::vector<std::shared_ptr<ImportNode>>& importNodes) {
    bool allSuccess = true;
    
    for (const auto& importNode : importNodes) {
        if (!resolveImport(importNode)) {
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

std::string ImportManager::getImportContent(std::shared_ptr<ImportNode> importNode) {
    if (!importNode) return "";
    
    return importNode->getImportContent();
}

bool ImportManager::fileExists(const std::string& filePath) {
    return std::filesystem::exists(filePath);
}

std::string ImportManager::findFile(const std::string& fileName) {
    // 首先检查当前目录
    if (fileExists(fileName)) {
        return fileName;
    }
    
    // 在导入路径中查找
    for (const auto& path : m_importPaths) {
        std::string fullPath = path + "/" + fileName;
        if (fileExists(fullPath)) {
            return fullPath;
        }
    }
    
    return "";
}

std::string ImportManager::readFile(const std::string& filePath) {
    // 检查缓存
    auto it = m_fileCache.find(filePath);
    if (it != m_fileCache.end()) {
        return it->second;
    }
    
    // 读取文件
    std::ifstream file(filePath);
    if (!file.is_open()) {
        addError("Cannot open file: " + filePath);
        return "";
    }
    
    std::string content((std::istreambuf_iterator<char>(file)),
                       std::istreambuf_iterator<char>());
    file.close();
    
    // 缓存内容
    m_fileCache[filePath] = content;
    
    return content;
}

void ImportManager::setDebugMode(bool debug) {
    m_debugMode = debug;
}

bool ImportManager::isDebugMode() const {
    return m_debugMode;
}

void ImportManager::clearCache() {
    m_fileCache.clear();
    m_processedFiles.clear();
}

std::string ImportManager::getStatistics() const {
    std::ostringstream oss;
    oss << "Import Statistics:\n";
    oss << "  Total imports: " << m_totalImports << "\n";
    oss << "  Successful: " << m_successfulImports << "\n";
    oss << "  Failed: " << m_failedImports << "\n";
    oss << "  Success rate: " << (m_totalImports > 0 ? (m_successfulImports * 100.0 / m_totalImports) : 0) << "%\n";
    return oss.str();
}

bool ImportManager::resolveCHTLImport(std::shared_ptr<ImportNode> importNode) {
    std::string filePath = findFile(importNode->getFilePath());
    if (filePath.empty()) {
        addError("CHTL file not found: " + importNode->getFilePath());
        return false;
    }
    
    std::string content = readFile(filePath);
    if (content.empty()) {
        addError("Cannot read CHTL file: " + filePath);
        return false;
    }
    
    importNode->setImportContent(content);
    return true;
}

bool ImportManager::resolveHTMLImport(std::shared_ptr<ImportNode> importNode) {
    std::string filePath = findFile(importNode->getFilePath());
    if (filePath.empty()) {
        addError("HTML file not found: " + importNode->getFilePath());
        return false;
    }
    
    std::string content = readFile(filePath);
    if (content.empty()) {
        addError("Cannot read HTML file: " + filePath);
        return false;
    }
    
    importNode->setImportContent(content);
    return true;
}

bool ImportManager::resolveCSSImport(std::shared_ptr<ImportNode> importNode) {
    std::string filePath = findFile(importNode->getFilePath());
    if (filePath.empty()) {
        addError("CSS file not found: " + importNode->getFilePath());
        return false;
    }
    
    std::string content = readFile(filePath);
    if (content.empty()) {
        addError("Cannot read CSS file: " + filePath);
        return false;
    }
    
    importNode->setImportContent(content);
    return true;
}

bool ImportManager::resolveJSImport(std::shared_ptr<ImportNode> importNode) {
    std::string filePath = findFile(importNode->getFilePath());
    if (filePath.empty()) {
        addError("JavaScript file not found: " + importNode->getFilePath());
        return false;
    }
    
    std::string content = readFile(filePath);
    if (content.empty()) {
        addError("Cannot read JavaScript file: " + filePath);
        return false;
    }
    
    importNode->setImportContent(content);
    return true;
}

bool ImportManager::resolveCMODImport(std::shared_ptr<ImportNode> importNode) {
    std::string filePath = findFile(importNode->getFilePath());
    if (filePath.empty()) {
        addError("CMOD file not found: " + importNode->getFilePath());
        return false;
    }
    
    std::string content = readFile(filePath);
    if (content.empty()) {
        addError("Cannot read CMOD file: " + filePath);
        return false;
    }
    
    importNode->setImportContent(content);
    return true;
}

bool ImportManager::resolveCJMODImport(std::shared_ptr<ImportNode> importNode) {
    std::string filePath = findFile(importNode->getFilePath());
    if (filePath.empty()) {
        addError("CJMOD file not found: " + importNode->getFilePath());
        return false;
    }
    
    std::string content = readFile(filePath);
    if (content.empty()) {
        addError("Cannot read CJMOD file: " + filePath);
        return false;
    }
    
    importNode->setImportContent(content);
    return true;
}

ImportType ImportManager::determineImportType(const std::string& filePath) {
    std::string extension = std::filesystem::path(filePath).extension().string();
    std::transform(extension.begin(), extension.end(), extension.begin(), ::tolower);
    
    if (extension == ".chtl") {
        return ImportType::CHTL;
    } else if (extension == ".html" || extension == ".htm") {
        return ImportType::HTML;
    } else if (extension == ".css") {
        return ImportType::CSS;
    } else if (extension == ".js") {
        return ImportType::JS;
    } else if (extension == ".cmod") {
        return ImportType::CMOD;
    } else if (extension == ".cjmod") {
        return ImportType::CJMOD;
    }
    
    return ImportType::UNKNOWN;
}

void ImportManager::addError(const std::string& message) {
    if (m_context) {
        m_context->addError("ImportManager Error: " + message);
    }
}

void ImportManager::addWarning(const std::string& message) {
    if (m_context) {
        m_context->addWarning("ImportManager Warning: " + message);
    }
}

} // namespace CHTL