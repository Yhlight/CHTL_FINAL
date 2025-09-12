#include "CHTLImportProcessor.h"
#include <sstream>
#include <algorithm>
#include <regex>
#include <filesystem>

namespace CHTL {

CHTLImportProcessor::CHTLImportProcessor() {}

void CHTLImportProcessor::addError(const std::string& error) {
    errors.push_back(error);
}

void CHTLImportProcessor::addWarning(const std::string& warning) {
    warnings.push_back(warning);
}

std::shared_ptr<ImportNode> CHTLImportProcessor::getImportNode(const std::string& path) const {
    auto it = importNodes.find(path);
    if (it != importNodes.end()) {
        return it->second;
    }
    return nullptr;
}

void CHTLImportProcessor::mergeImportProperties(std::shared_ptr<ImportNode> target, std::shared_ptr<ImportNode> source) {
    for (const auto& option : source->getImportOptions()) {
        if (!target->hasImportOption(option.first)) {
            target->setImportOption(option.first, option.second);
        }
    }
    for (const auto& meta : source->getMetadata()) {
        if (!target->hasMetadata(meta.first)) {
            target->setMetadata(meta.first, meta.second);
        }
    }
}

void CHTLImportProcessor::registerImportNode(std::shared_ptr<ImportNode> importNode) {
    if (importNode) {
        importNodes[importNode->getImportPath()] = importNode;
    }
}

void CHTLImportProcessor::unregisterImportNode(const std::string& path) {
    importNodes.erase(path);
}

bool CHTLImportProcessor::hasImportNode(const std::string& path) const {
    return importNodes.count(path) > 0;
}

std::shared_ptr<BaseNode> CHTLImportProcessor::processImportNode(const std::string& path) {
    auto importNode = getImportNode(path);
    if (!importNode) {
        addError("导入节点 '" + path + "' 未找到");
        return nullptr;
    }

    // 处理导入
    if (importNode->processImport()) {
        return importNode;
    } else {
        addError("导入节点 '" + path + "' 处理失败");
        return nullptr;
    }
}

std::shared_ptr<BaseNode> CHTLImportProcessor::instantiateImportNode(const std::string& path) {
    auto importNode = getImportNode(path);
    if (!importNode) {
        addError("导入节点 '" + path + "' 未找到");
        return nullptr;
    }
    return importNode->clone();
}

void CHTLImportProcessor::processAllImports() {
    for (auto& pair : importNodes) {
        pair.second->processImport();
    }
}

void CHTLImportProcessor::processImportByType(const std::string& type) {
    for (auto& pair : importNodes) {
        if (pair.second->getImportTypeName() == type) {
            pair.second->processImport();
        }
    }
}

void CHTLImportProcessor::processRequiredImports() {
    for (auto& pair : importNodes) {
        if (pair.second->isRequiredImport()) {
            pair.second->processImport();
        }
    }
}

void CHTLImportProcessor::processOptionalImports() {
    for (auto& pair : importNodes) {
        if (pair.second->isOptionalImport()) {
            pair.second->processImport();
        }
    }
}

void CHTLImportProcessor::addSearchPath(const std::string& path) {
    if (std::find(searchPaths.begin(), searchPaths.end(), path) == searchPaths.end()) {
        searchPaths.push_back(path);
    }
}

void CHTLImportProcessor::removeSearchPath(const std::string& path) {
    auto it = std::find(searchPaths.begin(), searchPaths.end(), path);
    if (it != searchPaths.end()) {
        searchPaths.erase(it);
    }
}

void CHTLImportProcessor::clearSearchPaths() {
    searchPaths.clear();
}

std::string CHTLImportProcessor::findImportPath(const std::string& path) const {
    // 首先检查绝对路径
    if (std::filesystem::exists(path)) {
        return path;
    }
    
    // 在搜索路径中查找
    for (const auto& searchPath : searchPaths) {
        std::string fullPath = searchPath + "/" + path;
        if (std::filesystem::exists(fullPath)) {
            return fullPath;
        }
    }
    
    return "";
}

std::vector<std::string> CHTLImportProcessor::findImportPaths(const std::string& pattern) const {
    std::vector<std::string> results;
    
    for (const auto& searchPath : searchPaths) {
        try {
            for (const auto& entry : std::filesystem::directory_iterator(searchPath)) {
                if (entry.is_regular_file()) {
                    std::string filename = entry.path().filename().string();
                    if (std::regex_match(filename, std::regex(pattern))) {
                        results.push_back(entry.path().string());
                    }
                }
            }
        } catch (const std::filesystem::filesystem_error& e) {
            // 在const方法中无法添加警告，暂时忽略
        }
    }
    
    return results;
}

std::vector<std::string> CHTLImportProcessor::findWildcardImports(const std::string& pattern) const {
    return findImportPaths(pattern);
}

void CHTLImportProcessor::addToCache(const std::string& path, const std::string& content) {
    importCache[path] = content;
}

void CHTLImportProcessor::addToNodeCache(const std::string& path, const std::vector<std::shared_ptr<BaseNode>>& nodes) {
    nodeCache[path] = nodes;
}

std::string CHTLImportProcessor::getFromCache(const std::string& path) const {
    auto it = importCache.find(path);
    return (it != importCache.end()) ? it->second : "";
}

std::vector<std::shared_ptr<BaseNode>> CHTLImportProcessor::getFromNodeCache(const std::string& path) const {
    auto it = nodeCache.find(path);
    return (it != nodeCache.end()) ? it->second : std::vector<std::shared_ptr<BaseNode>>();
}

bool CHTLImportProcessor::isInCache(const std::string& path) const {
    return importCache.find(path) != importCache.end();
}

void CHTLImportProcessor::clearCache() {
    importCache.clear();
}

void CHTLImportProcessor::clearNodeCache() {
    nodeCache.clear();
}

std::string CHTLImportProcessor::processHtmlContent(const std::string& content) const {
    // 简化的HTML内容处理
    return content;
}

std::string CHTLImportProcessor::processCssContent(const std::string& content) const {
    // 简化的CSS内容处理
    return content;
}

std::string CHTLImportProcessor::processJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容处理
    return content;
}

std::string CHTLImportProcessor::processChtlContent(const std::string& content) const {
    // 简化的CHTL内容处理
    return content;
}

std::string CHTLImportProcessor::processMixedContent(const std::string& content) const {
    // 简化的混合内容处理
    return content;
}

bool CHTLImportProcessor::validateHtmlContent(const std::string& content) const {
    // 简化的HTML内容验证
    return !content.empty();
}

bool CHTLImportProcessor::validateCssContent(const std::string& content) const {
    // 简化的CSS内容验证
    return !content.empty();
}

bool CHTLImportProcessor::validateJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容验证
    return !content.empty();
}

bool CHTLImportProcessor::validateChtlContent(const std::string& content) const {
    // 简化的CHTL内容验证
    return !content.empty();
}

bool CHTLImportProcessor::validateMixedContent(const std::string& content) const {
    // 简化的混合内容验证
    return !content.empty();
}

std::string CHTLImportProcessor::sanitizeHtmlContent(const std::string& content) const {
    // 简化的HTML内容清理
    return content;
}

std::string CHTLImportProcessor::sanitizeCssContent(const std::string& content) const {
    // 简化的CSS内容清理
    return content;
}

std::string CHTLImportProcessor::sanitizeJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容清理
    return content;
}

std::string CHTLImportProcessor::sanitizeChtlContent(const std::string& content) const {
    // 简化的CHTL内容清理
    return content;
}

std::string CHTLImportProcessor::sanitizeMixedContent(const std::string& content) const {
    // 简化的混合内容清理
    return content;
}

std::string CHTLImportProcessor::formatHtmlContent(const std::string& content) const {
    // 简化的HTML内容格式化
    return content;
}

std::string CHTLImportProcessor::formatCssContent(const std::string& content) const {
    // 简化的CSS内容格式化
    return content;
}

std::string CHTLImportProcessor::formatJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容格式化
    return content;
}

std::string CHTLImportProcessor::formatChtlContent(const std::string& content) const {
    // 简化的CHTL内容格式化
    return content;
}

std::string CHTLImportProcessor::formatMixedContent(const std::string& content) const {
    // 简化的混合内容格式化
    return content;
}

std::string CHTLImportProcessor::compressHtmlContent(const std::string& content) const {
    // 简化的HTML内容压缩
    return content;
}

std::string CHTLImportProcessor::compressCssContent(const std::string& content) const {
    // 简化的CSS内容压缩
    return content;
}

std::string CHTLImportProcessor::compressJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容压缩
    return content;
}

std::string CHTLImportProcessor::compressChtlContent(const std::string& content) const {
    // 简化的CHTL内容压缩
    return content;
}

std::string CHTLImportProcessor::compressMixedContent(const std::string& content) const {
    // 简化的混合内容压缩
    return content;
}

std::string CHTLImportProcessor::encodeHtmlContent(const std::string& content) const {
    // 简化的HTML内容编码
    return content;
}

std::string CHTLImportProcessor::encodeCssContent(const std::string& content) const {
    // 简化的CSS内容编码
    return content;
}

std::string CHTLImportProcessor::encodeJavaScriptContent(const std::string& content) const {
    // 简化的JavaScript内容编码
    return content;
}

std::string CHTLImportProcessor::encodeChtlContent(const std::string& content) const {
    // 简化的CHTL内容编码
    return content;
}

std::string CHTLImportProcessor::encodeMixedContent(const std::string& content) const {
    // 简化的混合内容编码
    return content;
}

bool CHTLImportProcessor::validateImportNode(const std::string& path) const {
    auto importNode = getImportNode(path);
    if (!importNode) {
        return false;
    }
    return importNode->validateImport();
}

bool CHTLImportProcessor::validateAllImports() const {
    for (const auto& pair : importNodes) {
        if (!pair.second->validateImport()) {
            return false;
        }
    }
    return true;
}

const std::vector<std::string>& CHTLImportProcessor::getErrors() const {
    return errors;
}

const std::vector<std::string>& CHTLImportProcessor::getWarnings() const {
    return warnings;
}

void CHTLImportProcessor::clearMessages() {
    errors.clear();
    warnings.clear();
}

void CHTLImportProcessor::reset() {
    importNodes.clear();
    clearCache();
    clearNodeCache();
    clearSearchPaths();
    clearMessages();
}

std::string CHTLImportProcessor::getDebugInfo() const {
    std::ostringstream oss;
    oss << "CHTL导入处理器调试信息:\n";
    oss << "注册导入节点数: " << importNodes.size() << "\n";
    oss << "搜索路径数: " << searchPaths.size() << "\n";
    oss << "缓存条目数: " << importCache.size() << "\n";
    oss << "节点缓存条目数: " << nodeCache.size() << "\n";
    oss << "错误数: " << errors.size() << "\n";
    oss << "警告数: " << warnings.size() << "\n";
    return oss.str();
}

} // namespace CHTL