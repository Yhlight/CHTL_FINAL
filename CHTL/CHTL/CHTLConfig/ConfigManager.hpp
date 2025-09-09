#pragma once

#include <string>
#include <map>
#include <memory>
#include <vector>
#include <set>
#include <functional>
#include "CHTLNode/BaseNode.hpp"

namespace CHTL {

/**
 * @brief 配置管理器
 * 
 * 管理Name块、关键字自定义、模块配置
 */
class ConfigManager {
public:
    enum class ConfigType {
        NAME,           // Name块配置
        ORIGIN_TYPE,    // OriginType配置
        CONFIG,         // 配置组
        MODULE,         // 模块配置
        CUSTOM          // 自定义配置
    };
    
    struct ConfigDefinition {
        std::string name;
        ConfigType type;
        std::map<std::string, std::string> properties;
        std::map<std::string, std::string> keywords;
        std::vector<std::string> modules;
        std::map<std::string, std::string> customSettings;
        bool isActive;
    };
    
    // 单例模式
    static ConfigManager& getInstance();
    
    // 添加配置定义
    void addConfig(const std::string& name, ConfigType type, 
                  const std::map<std::string, std::string>& properties = {});
    
    // 添加Name块配置
    void addNameConfig(const std::string& name, 
                      const std::map<std::string, std::string>& properties = {});
    
    // 添加OriginType配置
    void addOriginTypeConfig(const std::string& name, 
                            const std::map<std::string, std::string>& properties = {});
    
    // 添加配置组
    void addConfigGroup(const std::string& name, 
                       const std::map<std::string, std::string>& properties = {});
    
    // 添加模块配置
    void addModuleConfig(const std::string& name, 
                        const std::vector<std::string>& modules = {},
                        const std::map<std::string, std::string>& properties = {});
    
    // 添加自定义配置
    void addCustomConfig(const std::string& name, 
                        const std::map<std::string, std::string>& customSettings = {});
    
    // 获取配置定义
    std::shared_ptr<ConfigDefinition> getConfig(const std::string& name) const;
    
    // 检查配置是否存在
    bool hasConfig(const std::string& name) const;
    
    // 设置配置属性
    void setConfigProperty(const std::string& name, const std::string& property, 
                          const std::string& value);
    
    // 获取配置属性
    std::string getConfigProperty(const std::string& name, const std::string& property) const;
    
    // 获取所有配置属性
    std::map<std::string, std::string> getConfigProperties(const std::string& name) const;
    
    // 设置关键字
    void setKeyword(const std::string& name, const std::string& keyword, 
                   const std::string& value);
    
    // 获取关键字
    std::string getKeyword(const std::string& name, const std::string& keyword) const;
    
    // 获取所有关键字
    std::map<std::string, std::string> getKeywords(const std::string& name) const;
    
    // 添加模块
    void addModule(const std::string& name, const std::string& module);
    
    // 获取模块列表
    std::vector<std::string> getModules(const std::string& name) const;
    
    // 设置自定义设置
    void setCustomSetting(const std::string& name, const std::string& setting, 
                         const std::string& value);
    
    // 获取自定义设置
    std::string getCustomSetting(const std::string& name, const std::string& setting) const;
    
    // 获取所有自定义设置
    std::map<std::string, std::string> getCustomSettings(const std::string& name) const;
    
    // 激活配置
    void activateConfig(const std::string& name);
    
    // 停用配置
    void deactivateConfig(const std::string& name);
    
    // 检查配置是否激活
    bool isConfigActive(const std::string& name) const;
    
    // 获取所有配置名称
    std::vector<std::string> getAllConfigNames() const;
    
    // 获取指定类型的配置名称
    std::vector<std::string> getConfigNamesByType(ConfigType type) const;
    
    // 清除配置
    void clearConfig(const std::string& name);
    
    // 清除所有配置
    void clear();
    
    // 移除配置
    void removeConfig(const std::string& name);
    
    // 导入配置
    void importConfig(const std::string& name, const std::string& sourceName);
    
    // 导出配置
    std::map<std::string, std::string> exportConfig(const std::string& name) const;
    
    // 合并配置
    void mergeConfig(const std::string& targetName, const std::string& sourceName);
    
    // 验证配置
    bool validateConfig(const std::string& name) const;
    
    // 获取配置验证错误
    std::vector<std::string> getConfigValidationErrors(const std::string& name) const;
    
    // 设置默认配置
    void setDefaultConfig(const std::string& name);
    
    // 获取默认配置
    std::string getDefaultConfig() const;
    
    // 重置为默认配置
    void resetToDefault();
    
    // 获取配置层次结构
    std::vector<std::string> getConfigHierarchy(const std::string& name) const;
    
    // 添加配置依赖
    void addConfigDependency(const std::string& name, const std::string& dependency);
    
    // 获取配置依赖
    std::vector<std::string> getConfigDependencies(const std::string& name) const;
    
    // 检查配置依赖
    bool checkConfigDependencies(const std::string& name) const;
    
private:
    ConfigManager() = default;
    ~ConfigManager() = default;
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;
    
    std::map<std::string, std::shared_ptr<ConfigDefinition>> configs_;
    std::string defaultConfig_;
    std::map<std::string, std::vector<std::string>> configDependencies_;
    
    // 验证配置名称
    bool validateConfigName(const std::string& name) const;
    
    // 验证配置属性
    bool validateConfigProperty(const std::string& property, const std::string& value) const;
    
    // 验证关键字
    bool validateKeyword(const std::string& keyword, const std::string& value) const;
    
    // 验证模块
    bool validateModule(const std::string& module) const;
    
    // 验证自定义设置
    bool validateCustomSetting(const std::string& setting, const std::string& value) const;
    
    // 创建默认配置
    void createDefaultConfigs();
    
    // 解析配置层次结构
    std::vector<std::string> parseConfigHierarchy(const std::string& name) const;
    
    // 检查配置循环依赖
    bool checkCircularDependency(const std::string& name, const std::string& dependency) const;
};

} // namespace CHTL