#include "ImportManager.h"
#include <iostream>
#include <chrono>
#include <algorithm>
#include <fstream>
#include <sstream>

namespace CHTL {

ImportManager::ImportManager() 
    : cacheEnabled(true), debugMode(false) {
    resetStats();
}

void ImportManager::addImport(std::shared_ptr<ImportNode> importNode) {
    if (importNode) {
        imports.push_back(importNode);
        stats.totalImports++;
        
        switch (importNode->getImportType()) {
            case ImportType::FILE:
                stats.fileImports++;
                break;
            case ImportType::MODULE:
                stats.moduleImports++;
                break;
            case ImportType::PRECISE:
                stats.preciseImports++;
                break;
            case ImportType::TYPE:
                stats.typeImports++;
                break;
            case ImportType::WILDCARD:
                stats.wildcardImports++;
                break;
            default:
                break;
        }
    }
}

void ImportManager::removeImport(std::shared_ptr<ImportNode> importNode) {
    auto it = std::find(imports.begin(), imports.end(), importNode);
    if (it != imports.end()) {
        imports.erase(it);
        stats.totalImports--;
    }
}

std::string ImportManager::resolveImport(std::shared_ptr<ImportNode> importNode) {
    if (!importNode) {
        return "";
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    std::string result;
    
    // 检查缓存
    if (cacheEnabled) {
        std::string cacheKey = getCacheKey(importNode->getTargetPath());
        auto cacheIt = importCache.find(cacheKey);
        if (cacheIt != importCache.end()) {
            stats.cachedImports++;
            return cacheIt->second;
        }
    }
    
    // 解析导入
    result = processImport(importNode);
    
    // 更新缓存
    if (cacheEnabled && !result.empty()) {
        std::string cacheKey = getCacheKey(importNode->getTargetPath());
        importCache[cacheKey] = result;
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double resolveTime = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    stats.totalResolveTime += resolveTime;
    
    updateStats(importNode->getImportType(), resolveTime);
    
    return result;
}

std::vector<std::string> ImportManager::resolveAllImports() {
    std::vector<std::string> results;
    
    for (const auto& import : imports) {
        std::string resolved = resolveImport(import);
        if (!resolved.empty()) {
            results.push_back(resolved);
        }
    }
    
    return results;
}

void ImportManager::addSearchPath(const std::string& path) {
    searchPaths.push_back(path);
}

void ImportManager::removeSearchPath(const std::string& path) {
    auto it = std::find(searchPaths.begin(), searchPaths.end(), path);
    if (searchPaths.end() != it) {
        searchPaths.erase(it);
    }
}

void ImportManager::clearCache() {
    importCache.clear();
    stats.cachedImports = 0;
}

bool ImportManager::validateImport(std::shared_ptr<ImportNode> importNode) {
    if (!importNode) {
        return false;
    }
    
    // 检查路径是否有效
    if (!importNode->isValidPath()) {
        debugLog("Invalid path: " + importNode->getTargetPath());
        return false;
    }
    
    // 检查文件是否存在
    if (!importNode->isFileExists()) {
        debugLog("File not found: " + importNode->getTargetPath());
        return false;
    }
    
    // 检查循环依赖
    if (isCircularDependency(importNode->getTargetPath(), importNode->getTargetPath())) {
        debugLog("Circular dependency detected: " + importNode->getTargetPath());
        return false;
    }
    
    return true;
}

std::vector<std::string> ImportManager::validateAllImports() {
    std::vector<std::string> errors;
    
    for (const auto& import : imports) {
        if (!validateImport(import)) {
            errors.push_back("Invalid import: " + import->getTargetPath());
        }
    }
    
    return errors;
}

bool ImportManager::hasCircularDependency() const {
    for (const auto& import : imports) {
        if (isCircularDependency(import->getTargetPath(), import->getTargetPath())) {
            return true;
        }
    }
    return false;
}

std::vector<std::string> ImportManager::getCircularDependencies() const {
    std::vector<std::string> circular;
    
    for (const auto& import : imports) {
        if (isCircularDependency(import->getTargetPath(), import->getTargetPath())) {
            circular.push_back(import->getTargetPath());
        }
    }
    
    return circular;
}

std::vector<std::shared_ptr<ImportNode>> ImportManager::getImportOrder() const {
    // 简单的导入顺序，按添加顺序返回
    // 在实际实现中，这里应该进行拓扑排序
    return imports;
}

void ImportManager::sortImports() {
    // 按导入类型和路径排序
    std::sort(imports.begin(), imports.end(), 
        [](const std::shared_ptr<ImportNode>& a, const std::shared_ptr<ImportNode>& b) {
            if (a->getImportType() != b->getImportType()) {
                return static_cast<int>(a->getImportType()) < static_cast<int>(b->getImportType());
            }
            return a->getTargetPath() < b->getTargetPath();
        });
}

void ImportManager::setImportHandler(ImportTargetType type, ImportHandler handler) {
    importHandlers[type] = handler;
}

ImportManager::ImportHandler ImportManager::getImportHandler(ImportTargetType type) const {
    auto it = importHandlers.find(type);
    if (it != importHandlers.end()) {
        return it->second;
    }
    return nullptr;
}

std::string ImportManager::getDebugInfo() const {
    std::ostringstream oss;
    oss << "ImportManager Debug Info:\n";
    oss << "  Total Imports: " << stats.totalImports << "\n";
    oss << "  File Imports: " << stats.fileImports << "\n";
    oss << "  Module Imports: " << stats.moduleImports << "\n";
    oss << "  Precise Imports: " << stats.preciseImports << "\n";
    oss << "  Type Imports: " << stats.typeImports << "\n";
    oss << "  Wildcard Imports: " << stats.wildcardImports << "\n";
    oss << "  Cached Imports: " << stats.cachedImports << "\n";
    oss << "  Total Resolve Time: " << stats.totalResolveTime << "ms\n";
    oss << "  Search Paths: " << searchPaths.size() << "\n";
    oss << "  Cache Size: " << importCache.size() << "\n";
    return oss.str();
}

std::string ImportManager::loadFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        debugLog("Failed to open file: " + path);
        return "";
    }
    
    std::ostringstream buffer;
    buffer << file.rdbuf();
    file.close();
    
    return buffer.str();
}

std::string ImportManager::processImport(std::shared_ptr<ImportNode> importNode) {
    std::string path = importNode->resolvePath();
    if (path.empty()) {
        debugLog("Failed to resolve path: " + importNode->getTargetPath());
        return "";
    }
    
    // 检查是否有自定义处理器
    ImportHandler handler = getImportHandler(importNode->getTargetType());
    if (handler) {
        return handler(path, importNode->getTargetType());
    }
    
    // 默认处理：直接加载文件内容
    return loadFile(path);
}

bool ImportManager::isCircularDependency(const std::string& path, const std::string& target) const {
    // 简单的循环依赖检测
    // 在实际实现中，这里应该构建依赖图并进行深度优先搜索
    return path == target;
}

void ImportManager::updateStats(ImportType type, double resolveTime) {
    // 更新统计信息
    // 这里可以添加更详细的统计逻辑
}

std::string ImportManager::getCacheKey(const std::string& path) const {
    return "import_" + path;
}

void ImportManager::debugLog(const std::string& message) const {
    if (debugMode) {
        std::cout << "[ImportManager] " << message << std::endl;
    }
}

void ImportManager::resetStats() {
    stats = ImportStats{};
}

} // namespace CHTL