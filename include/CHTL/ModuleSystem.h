#pragma once

#include "CHTL/Common.h"
#include "CHTL/BaseNode.h"
#include <string>
#include <vector>
#include <memory>
#include <map>

namespace CHTL {

/**
 * 模块系统 - 支持CMOD和CJMOD模块
 */
class ModuleSystem {
public:
    // 模块类型
    enum class ModuleType {
        CMOD,           // CHTL模块
        CJMOD,          // CHTL JS模块
        MIXED           // 混合模块
    };

    // 模块信息
    struct ModuleInfo {
        std::string name;
        std::string version;
        std::string description;
        std::string author;
        std::string license;
        std::string dependencies;
        std::string category;
        std::string minCHTLVersion;
        std::string maxCHTLVersion;
        ModuleType type;
        std::string filePath;
        std::map<std::string, std::string> metadata;
        
        ModuleInfo() : type(ModuleType::CMOD) {}
    };

    // 导出信息
    struct ExportInfo {
        std::vector<std::string> customStyles;
        std::vector<std::string> customElements;
        std::vector<std::string> customVars;
        std::vector<std::string> templateStyles;
        std::vector<std::string> templateElements;
        std::vector<std::string> templateVars;
        std::vector<std::string> originHtmls;
        std::vector<std::string> originStyles;
        std::vector<std::string> originJavascripts;
        std::vector<std::string> configurations;
        std::map<std::string, std::string> customOriginTypes;
    };

    // 模块结构
    struct ModuleStructure {
        std::string moduleName;
        std::string srcPath;
        std::string infoPath;
        std::vector<ModuleStructure> subModules;
        bool hasMainFile = false;
        std::string mainFilePath;
    };

    // 搜索路径
    struct SearchPath {
        std::string path;
        bool isOfficial = false;
        bool isUser = false;
        bool isOrdered = false; // 是否使用有序结构
        int priority = 0; // 优先级，数字越大优先级越高
    };

public:
    ModuleSystem();
    ~ModuleSystem();

    /**
     * 初始化模块系统
     * @param officialModulePath 官方模块路径
     * @param userModulePath 用户模块路径
     */
    void initialize(const std::string& officialModulePath, const std::string& userModulePath);

    /**
     * 加载模块
     * @param moduleName 模块名称
     * @param moduleType 模块类型
     * @return 是否成功
     */
    bool loadModule(const std::string& moduleName, ModuleType moduleType = ModuleType::CMOD);

    /**
     * 加载模块从文件
     * @param filePath 文件路径
     * @return 是否成功
     */
    bool loadModuleFromFile(const std::string& filePath);

    /**
     * 卸载模块
     * @param moduleName 模块名称
     * @return 是否成功
     */
    bool unloadModule(const std::string& moduleName);

    /**
     * 获取模块信息
     * @param moduleName 模块名称
     * @return 模块信息
     */
    ModuleInfo getModuleInfo(const std::string& moduleName) const;

    /**
     * 获取模块导出信息
     * @param moduleName 模块名称
     * @return 导出信息
     */
    ExportInfo getModuleExports(const std::string& moduleName) const;

    /**
     * 搜索模块
     * @param moduleName 模块名称
     * @param moduleType 模块类型
     * @return 找到的模块路径
     */
    std::string searchModule(const std::string& moduleName, ModuleType moduleType = ModuleType::CMOD);

    /**
     * 列出所有已加载的模块
     * @return 模块名称列表
     */
    std::vector<std::string> listLoadedModules() const;

    /**
     * 列出所有可用模块
     * @param moduleType 模块类型
     * @return 模块名称列表
     */
    std::vector<std::string> listAvailableModules(ModuleType moduleType = ModuleType::CMOD) const;

    /**
     * 解析模块依赖
     * @param moduleName 模块名称
     * @return 依赖列表
     */
    std::vector<std::string> resolveDependencies(const std::string& moduleName);

    /**
     * 验证模块
     * @param moduleName 模块名称
     * @return 验证结果
     */
    bool validateModule(const std::string& moduleName);

    /**
     * 打包模块
     * @param modulePath 模块路径
     * @param outputPath 输出路径
     * @return 是否成功
     */
    bool packageModule(const std::string& modulePath, const std::string& outputPath);

    /**
     * 解包模块
     * @param packagePath 包路径
     * @param outputPath 输出路径
     * @return 是否成功
     */
    bool unpackModule(const std::string& packagePath, const std::string& outputPath);

    /**
     * 添加搜索路径
     * @param path 路径
     * @param isOfficial 是否是官方路径
     * @param priority 优先级
     */
    void addSearchPath(const std::string& path, bool isOfficial = false, int priority = 0);

    /**
     * 移除搜索路径
     * @param path 路径
     * @return 是否成功
     */
    bool removeSearchPath(const std::string& path);

    /**
     * 获取搜索路径列表
     * @return 搜索路径列表
     */
    std::vector<SearchPath> getSearchPaths() const;

    /**
     * 设置模块缓存
     * @param enabled 是否启用
     */
    void setModuleCache(bool enabled);

    /**
     * 清除模块缓存
     */
    void clearModuleCache();

    /**
     * 获取模块统计信息
     * @return 统计信息
     */
    std::map<std::string, std::string> getModuleStatistics() const;

private:
    // 内部方法
    bool loadCMODModule(const std::string& modulePath);
    bool loadCJMODModule(const std::string& modulePath);
    bool loadMixedModule(const std::string& modulePath);
    
    ModuleInfo parseModuleInfo(const std::string& infoPath);
    ExportInfo parseExportInfo(const std::string& infoPath);
    ModuleStructure parseModuleStructure(const std::string& modulePath);
    
    std::string findModuleFile(const std::string& moduleName, ModuleType moduleType);
    std::string findModuleInPath(const std::string& moduleName, const std::string& path, ModuleType moduleType);
    
    bool validateModuleStructure(const ModuleStructure& structure);
    bool validateModuleInfo(const ModuleInfo& info);
    bool validateModuleDependencies(const std::string& moduleName);
    
    std::string createModulePackage(const std::string& modulePath);
    bool extractModulePackage(const std::string& packagePath, const std::string& outputPath);
    
    void updateModuleCache(const std::string& moduleName, const ModuleInfo& info);
    void removeFromModuleCache(const std::string& moduleName);
    
    // 错误处理
    void handleError(const std::string& error);
    void handleWarning(const std::string& warning);

private:
    std::vector<SearchPath> searchPaths_;
    std::map<std::string, ModuleInfo> loadedModules_;
    std::map<std::string, ExportInfo> moduleExports_;
    std::map<std::string, ModuleStructure> moduleStructures_;
    
    bool moduleCacheEnabled_;
    std::map<std::string, std::string> moduleCache_;
    
    // 统计信息
    size_t totalModulesLoaded_;
    size_t totalModulesSearched_;
    double totalLoadTime_;
    size_t totalMemoryUsage_;
    
    // 默认路径
    std::string officialModulePath_;
    std::string userModulePath_;
};

} // namespace CHTL