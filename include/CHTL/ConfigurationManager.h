#pragma once

#include "CHTL/CHTLNode/ConfigurationNode.h"
#include "CHTL/CHTLContext.h"
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>
#include <unordered_set>

namespace CHTL {

/**
 * @brief 配置管理器类
 * 
 * 负责管理配置、关键字自定义和模块配置
 */
class ConfigurationManager {
public:
    /**
     * @brief 构造函数
     * @param context CHTL上下文
     */
    explicit ConfigurationManager(std::shared_ptr<CHTLContext> context);
    
    /**
     * @brief 析构函数
     */
    ~ConfigurationManager();
    
    /**
     * @brief 添加配置
     * @param configurationNode 配置节点
     * @return 是否添加成功
     */
    bool addConfiguration(std::shared_ptr<ConfigurationNode> configurationNode);
    
    /**
     * @brief 获取配置
     * @param name 配置名称
     * @return 配置节点
     */
    std::shared_ptr<ConfigurationNode> getConfiguration(const std::string& name);
    
    /**
     * @brief 检查配置是否存在
     * @param name 配置名称
     * @return 是否存在
     */
    bool hasConfiguration(const std::string& name);
    
    /**
     * @brief 获取配置项
     * @param name 配置名称
     * @param key 配置键
     * @return 配置值
     */
    std::string getConfigurationValue(const std::string& name, const std::string& key);
    
    /**
     * @brief 设置配置项
     * @param name 配置名称
     * @param key 配置键
     * @param value 配置值
     * @return 是否设置成功
     */
    bool setConfigurationValue(const std::string& name, const std::string& key, const std::string& value);
    
    /**
     * @brief 获取所有配置
     * @return 配置列表
     */
    const std::vector<std::shared_ptr<ConfigurationNode>>& getAllConfigurations() const;
    
    /**
     * @brief 获取配置组
     * @param group 配置组名称
     * @return 配置组列表
     */
    std::vector<std::shared_ptr<ConfigurationNode>> getConfigurationGroup(const std::string& group);
    
    /**
     * @brief 获取关键字配置
     * @return 关键字配置列表
     */
    std::vector<std::shared_ptr<ConfigurationNode>> getKeywordConfigurations();
    
    /**
     * @brief 获取模块配置
     * @return 模块配置列表
     */
    std::vector<std::shared_ptr<ConfigurationNode>> getModuleConfigurations();
    
    /**
     * @brief 获取编译器配置
     * @return 编译器配置列表
     */
    std::vector<std::shared_ptr<ConfigurationNode>> getCompilerConfigurations();
    
    /**
     * @brief 获取输出配置
     * @return 输出配置列表
     */
    std::vector<std::shared_ptr<ConfigurationNode>> getOutputConfigurations();
    
    /**
     * @brief 获取调试配置
     * @return 调试配置列表
     */
    std::vector<std::shared_ptr<ConfigurationNode>> getDebugConfigurations();
    
    /**
     * @brief 应用配置
     * @param configurationNode 配置节点
     * @return 是否应用成功
     */
    bool applyConfiguration(std::shared_ptr<ConfigurationNode> configurationNode);
    
    /**
     * @brief 应用所有配置
     * @return 是否应用成功
     */
    bool applyAllConfigurations();
    
    /**
     * @brief 清除所有配置
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
     * @brief 应用关键字配置
     * @param configurationNode 配置节点
     * @return 是否应用成功
     */
    bool applyKeywordConfiguration(std::shared_ptr<ConfigurationNode> configurationNode);
    
    /**
     * @brief 应用模块配置
     * @param configurationNode 配置节点
     * @return 是否应用成功
     */
    bool applyModuleConfiguration(std::shared_ptr<ConfigurationNode> configurationNode);
    
    /**
     * @brief 应用编译器配置
     * @param configurationNode 配置节点
     * @return 是否应用成功
     */
    bool applyCompilerConfiguration(std::shared_ptr<ConfigurationNode> configurationNode);
    
    /**
     * @brief 应用输出配置
     * @param configurationNode 配置节点
     * @return 是否应用成功
     */
    bool applyOutputConfiguration(std::shared_ptr<ConfigurationNode> configurationNode);
    
    /**
     * @brief 应用调试配置
     * @param configurationNode 配置节点
     * @return 是否应用成功
     */
    bool applyDebugConfiguration(std::shared_ptr<ConfigurationNode> configurationNode);
    
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
    std::vector<std::shared_ptr<ConfigurationNode>> m_configurations;
    std::unordered_map<std::string, std::shared_ptr<ConfigurationNode>> m_configurationMap;
    std::unordered_map<std::string, std::vector<std::shared_ptr<ConfigurationNode>>> m_configurationGroups;
    std::unordered_set<std::string> m_appliedConfigurations;
    bool m_debugMode;
    
    // 统计信息
    size_t m_totalConfigurations;
    size_t m_appliedConfigurationsCount;
    size_t m_failedConfigurationsCount;
};

} // namespace CHTL