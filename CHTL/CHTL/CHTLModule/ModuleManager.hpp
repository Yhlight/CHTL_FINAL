#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <set>
#include <filesystem>
#include "CHTLNode/BaseNode.hpp"

namespace CHTL {

/**
 * @brief 模块管理器
 * 
 * 管理CMOD、CJMOD、混合模块结构
 */
class ModuleManager {
public:
    enum class ModuleType {
        CMOD,           // CMOD模块
        CJMOD,          // CJMOD模块
        MIXED,          // 混合模块
        CUSTOM          // 自定义模块
    };
    
    enum class ModuleStatus {
        UNLOADED,       // 未加载
        LOADING,        // 加载中
        LOADED,         // 已加载
        ERROR           // 错误状态
    };
    
    struct ModuleInfo {
        std::string name;
        std::string version;
        std::string description;
        std::string author;
        std::string license;
        std::vector<std::string> dependencies;
        std::string category;
        std::string minCHTLVersion;
        std::string maxCHTLVersion;
        ModuleType type;
        ModuleStatus status;
        std::string path;
        std::map<std::string, std::string> exports;
        std::map<std::string, std::string> imports;
        std::map<std::string, std::string> metadata;
    };
    
    struct ModuleStructure {
        std::string srcPath;
        std::string infoPath;
        std::vector<std::string> sourceFiles;
        std::vector<std::string> infoFiles;
        std::map<std::string, std::string> exports;
        std::map<std::string, std::string> imports;
    };
    
    // 单例模式
    static ModuleManager& getInstance();
    
    // 加载模块
    bool loadModule(const std::string& name, const std::string& path = "");
    
    // 卸载模块
    void unloadModule(const std::string& name);
    
    // 重新加载模块
    bool reloadModule(const std::string& name);
    
    // 获取模块信息
    std::shared_ptr<ModuleInfo> getModuleInfo(const std::string& name) const;
    
    // 检查模块是否已加载
    bool isModuleLoaded(const std::string& name) const;
    
    // 获取模块状态
    ModuleStatus getModuleStatus(const std::string& name) const;
    
    // 获取所有已加载模块
    std::vector<std::string> getLoadedModules() const;
    
    // 获取模块依赖
    std::vector<std::string> getModuleDependencies(const std::string& name) const;
    
    // 检查模块依赖
    bool checkModuleDependencies(const std::string& name) const;
    
    // 解析模块依赖
    std::vector<std::string> resolveModuleDependencies(const std::string& name) const;
    
    // 获取模块导出
    std::map<std::string, std::string> getModuleExports(const std::string& name) const;
    
    // 获取模块导入
    std::map<std::string, std::string> getModuleImports(const std::string& name) const;
    
    // 搜索模块
    std::vector<std::string> searchModules(const std::string& query) const;
    
    // 获取模块列表
    std::vector<std::string> getModuleList() const;
    
    // 获取指定类型的模块
    std::vector<std::string> getModulesByType(ModuleType type) const;
    
    // 获取指定类别的模块
    std::vector<std::string> getModulesByCategory(const std::string& category) const;
    
    // 验证模块
    bool validateModule(const std::string& name) const;
    
    // 获取模块验证错误
    std::vector<std::string> getModuleValidationErrors(const std::string& name) const;
    
    // 打包模块
    bool packageModule(const std::string& name, const std::string& outputPath) const;
    
    // 解包模块
    bool unpackModule(const std::string& packagePath, const std::string& outputPath) const;
    
    // 安装模块
    bool installModule(const std::string& packagePath) const;
    
    // 卸载模块
    bool uninstallModule(const std::string& name) const;
    
    // 更新模块
    bool updateModule(const std::string& name) const;
    
    // 设置模块搜索路径
    void addModuleSearchPath(const std::string& path);
    
    // 获取模块搜索路径
    std::vector<std::string> getModuleSearchPaths() const;
    
    // 清除模块搜索路径
    void clearModuleSearchPaths();
    
    // 设置模块缓存路径
    void setModuleCachePath(const std::string& path);
    
    // 获取模块缓存路径
    std::string getModuleCachePath() const;
    
    // 清除模块缓存
    void clearModuleCache();
    
    // 获取模块统计信息
    std::map<std::string, int> getModuleStatistics() const;
    
    // 导出模块配置
    std::map<std::string, std::string> exportModuleConfig() const;
    
    // 导入模块配置
    bool importModuleConfig(const std::map<std::string, std::string>& config);
    
    // 清除所有模块
    void clear();
    
    // 移除模块
    void removeModule(const std::string& name);
    
private:
    ModuleManager() = default;
    ~ModuleManager() = default;
    ModuleManager(const ModuleManager&) = delete;
    ModuleManager& operator=(const ModuleManager&) = delete;
    
    std::map<std::string, std::shared_ptr<ModuleInfo>> modules_;
    std::vector<std::string> moduleSearchPaths_;
    std::string moduleCachePath_;
    
    // 加载CMOD模块
    bool loadCMODModule(const std::string& name, const std::string& path);
    
    // 加载CJMOD模块
    bool loadCJMODModule(const std::string& name, const std::string& path);
    
    // 加载混合模块
    bool loadMixedModule(const std::string& name, const std::string& path);
    
    // 加载自定义模块
    bool loadCustomModule(const std::string& name, const std::string& path);
    
    // 解析模块信息
    std::shared_ptr<ModuleInfo> parseModuleInfo(const std::string& infoPath) const;
    
    // 解析模块结构
    ModuleStructure parseModuleStructure(const std::string& modulePath) const;
    
    // 验证模块结构
    bool validateModuleStructure(const ModuleStructure& structure) const;
    
    // 验证模块信息
    bool validateModuleInfo(const ModuleInfo& info) const;
    
    // 搜索模块文件
    std::string searchModuleFile(const std::string& name, ModuleType type) const;
    
    // 获取模块类型
    ModuleType getModuleType(const std::string& path) const;
    
    // 创建模块缓存
    bool createModuleCache(const std::string& name, const ModuleInfo& info) const;
    
    // 加载模块缓存
    std::shared_ptr<ModuleInfo> loadModuleCache(const std::string& name) const;
    
    // 更新模块缓存
    bool updateModuleCache(const std::string& name, const ModuleInfo& info) const;
    
    // 删除模块缓存
    bool deleteModuleCache(const std::string& name) const;
    
    // 检查模块版本兼容性
    bool checkVersionCompatibility(const std::string& moduleVersion, 
                                  const std::string& minVersion, 
                                  const std::string& maxVersion) const;
    
    // 解析版本号
    std::vector<int> parseVersion(const std::string& version) const;
    
    // 比较版本号
    int compareVersions(const std::vector<int>& version1, const std::vector<int>& version2) const;
    
    // 处理模块错误
    void handleModuleError(const std::string& name, const std::string& error);
    
    // 处理模块警告
    void handleModuleWarning(const std::string& name, const std::string& warning);
    
    // 清理模块资源
    void cleanupModuleResources(const std::string& name);
    
    // 初始化模块系统
    void initializeModuleSystem();
    
    // 扫描模块目录
    void scanModuleDirectories();
    
    // 更新模块索引
    void updateModuleIndex();
    
    // 保存模块索引
    bool saveModuleIndex() const;
    
    // 加载模块索引
    bool loadModuleIndex();
};

} // namespace CHTL