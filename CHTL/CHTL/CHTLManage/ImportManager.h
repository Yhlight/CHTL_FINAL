#pragma once

#include "CHTLNode/ImportNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <functional>

namespace CHTL {

class ImportManager {
public:
    ImportManager();
    virtual ~ImportManager() = default;
    
    // 导入管理
    void addImport(std::shared_ptr<ImportNode> importNode);
    void removeImport(std::shared_ptr<ImportNode> importNode);
    std::vector<std::shared_ptr<ImportNode>> getImports() const { return imports; }
    
    // 导入解析
    std::string resolveImport(std::shared_ptr<ImportNode> importNode);
    std::vector<std::string> resolveAllImports();
    
    // 路径管理
    void addSearchPath(const std::string& path);
    void removeSearchPath(const std::string& path);
    const std::vector<std::string>& getSearchPaths() const { return searchPaths; }
    
    void setBasePath(const std::string& path) { basePath = path; }
    const std::string& getBasePath() const { return basePath; }
    
    // 导入缓存
    void enableCache(bool enable) { cacheEnabled = enable; }
    bool isCacheEnabled() const { return cacheEnabled; }
    
    void clearCache();
    std::map<std::string, std::string> getCache() const { return importCache; }
    
    // 导入验证
    bool validateImport(std::shared_ptr<ImportNode> importNode);
    std::vector<std::string> validateAllImports();
    
    // 循环依赖检测
    bool hasCircularDependency() const;
    std::vector<std::string> getCircularDependencies() const;
    
    // 导入顺序
    std::vector<std::shared_ptr<ImportNode>> getImportOrder() const;
    void sortImports();
    
    // 导入统计
    struct ImportStats {
        size_t totalImports;
        size_t fileImports;
        size_t moduleImports;
        size_t preciseImports;
        size_t typeImports;
        size_t wildcardImports;
        size_t cachedImports;
        double totalResolveTime;
    };
    
    ImportStats getStats() const { return stats; }
    void resetStats();
    
    // 导入处理器
    using ImportHandler = std::function<std::string(const std::string&, ImportTargetType)>;
    void setImportHandler(ImportTargetType type, ImportHandler handler);
    ImportHandler getImportHandler(ImportTargetType type) const;
    
    // 调试
    void enableDebugMode(bool enable) { debugMode = enable; }
    bool isDebugMode() const { return debugMode; }
    std::string getDebugInfo() const;
    
private:
    std::vector<std::shared_ptr<ImportNode>> imports;
    std::vector<std::string> searchPaths;
    std::string basePath;
    std::map<std::string, std::string> importCache;
    std::map<ImportTargetType, ImportHandler> importHandlers;
    bool cacheEnabled;
    bool debugMode;
    ImportStats stats;
    
    // 内部方法
    std::string loadFile(const std::string& path);
    std::string processImport(std::shared_ptr<ImportNode> importNode);
    bool isCircularDependency(const std::string& path, const std::string& target) const;
    void updateStats(ImportType type, double resolveTime);
    std::string getCacheKey(const std::string& path) const;
    
    // 调试
    void debugLog(const std::string& message) const;
};

} // namespace CHTL