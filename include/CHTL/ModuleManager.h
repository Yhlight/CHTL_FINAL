#pragma once

#include "CHTL/CHTLNode/ModuleNode.h"
#include "CHTL/CHTLContext.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace CHTL {

/**
 * @brief 模块管理器类
 * 
 * 负责管理模块、依赖解析和模块加载
 */
class ModuleManager {
public:
    /**
     * @brief 构造函数
     * @param context CHTL上下文
     */
    explicit ModuleManager(std::shared_ptr<CHTLContext> context);
    
    /**
     * @brief 析构函数
     */
    ~ModuleManager();
    
    /**
     * @brief 添加模块
     * @param moduleNode 模块节点
     * @return 是否添加成功
     */
    bool addModule(std::shared_ptr<ModuleNode> moduleNode);
    
    /**
     * @brief 获取模块
     * @param name 模块名称
     * @return 模块节点
     */
    std::shared_ptr<ModuleNode> getModule(const std::string& name);
    
    /**
     * @brief 检查模块是否存在
     * @param name 模块名称
     * @return 是否存在
     */
    bool hasModule(const std::string& name);
    
    /**
     * @brief 获取所有模块
     * @return 模块列表
     */
    const std::vector<std::shared_ptr<ModuleNode>>& getAllModules() const;
    
    /**
     * @brief 获取模块类型
     * @param type 模块类型
     * @return 模块列表
     */
    std::vector<std::shared_ptr<ModuleNode>> getModulesByType(ModuleType type);
    
    /**
     * @brief 获取CMOD模块
     * @return CMOD模块列表
     */
    std::vector<std::shared_ptr<ModuleNode>> getCMODModules();
    
    /**
     * @brief 获取CJMOD模块
     * @return CJMOD模块列表
     */
    std::vector<std::shared_ptr<ModuleNode>> getCJMODModules();
    
    /**
     * @brief 解析模块依赖
     * @param moduleName 模块名称
     * @return 是否解析成功
     */
    bool resolveModuleDependencies(const std::string& moduleName);
    
    /**
     * @brief 解析所有模块依赖
     * @return 是否解析成功
     */
    bool resolveAllModuleDependencies();
    
    /**
     * @brief 加载模块
     * @param moduleName 模块名称
     * @return 是否加载成功
     */
    bool loadModule(const std::string& moduleName);
    
    /**
     * @brief 加载所有模块
     * @return 是否加载成功
     */
    bool loadAllModules();
    
    /**
     * @brief 卸载模块
     * @param moduleName 模块名称
     * @return 是否卸载成功
     */
    bool unloadModule(const std::string& moduleName);
    
    /**
     * @brief 卸载所有模块
     * @return 是否卸载成功
     */
    bool unloadAllModules();
    
    /**
     * @brief 检查模块依赖
     * @param moduleName 模块名称
     * @return 依赖检查结果
     */
    std::vector<std::string> checkModuleDependencies(const std::string& moduleName);
    
    /**
     * @brief 获取模块加载顺序
     * @return 模块加载顺序
     */
    std::vector<std::string> getModuleLoadOrder();
    
    /**
     * @brief 清除所有模块
     */
    void clear();
    
    /**
     * @brief 设置调试模式
     * @param debug 是否启用调试模式
     */
    void setDebugMode(bool debug);
    
    /**
     * @brief 是否处于调试模式
     * @return 是否调试模式
     */
    bool isDebugMode() const;
    
    /**
     * @brief 获取统计信息
     * @return 统计信息字符串
     */
    std::string getStatistics() const;

private:
    /**
     * @brief 解析模块依赖递归
     * @param moduleName 模块名称
     * @param visited 已访问的模块
     * @param resolving 正在解析的模块
     * @return 是否解析成功
     */
    bool resolveModuleDependenciesRecursive(const std::string& moduleName, 
                                           std::unordered_set<std::string>& visited,
                                           std::unordered_set<std::string>& resolving);
    
    /**
     * @brief 加载模块内容
     * @param moduleNode 模块节点
     * @return 是否加载成功
     */
    bool loadModuleContent(std::shared_ptr<ModuleNode> moduleNode);
    
    /**
     * @brief 添加错误信息
     * @param message 错误信息
     */
    void addError(const std::string& message);
    
    /**
     * @brief 添加警告信息
     * @param message 警告信息
     */
    void addWarning(const std::string& message);

private:
    std::shared_ptr<CHTLContext> m_context;
    std::vector<std::shared_ptr<ModuleNode>> m_modules;
    std::unordered_map<std::string, std::shared_ptr<ModuleNode>> m_moduleMap;
    std::unordered_set<std::string> m_loadedModules;
    std::unordered_set<std::string> m_resolvedModules;
    bool m_debugMode;
    
    // 统计信息
    size_t m_totalModules;
    size_t m_loadedModulesCount;
    size_t m_failedModulesCount;
    size_t m_dependencyConflicts;
};

} // namespace CHTL