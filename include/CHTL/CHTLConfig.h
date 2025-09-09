#pragma once

#include "CHTLNode.h"
#include "CHTLContext.h"
#include <string>
#include <memory>
#include <unordered_map>
#include <vector>
#include <set>

namespace CHTL {

/**
 * @brief 配置类型枚举
 */
enum class ConfigType {
    BASIC,          // 基础配置
    NAMED,          // 命名配置
    IMPORTED        // 导入配置
};

/**
 * @brief 配置项
 */
class ConfigItem {
public:
    ConfigItem(const std::string& key, const std::string& value, 
               const std::string& type = "", int line = 0, int column = 0);
    ~ConfigItem() = default;

    std::string getKey() const { return m_key; }
    std::string getValue() const { return m_value; }
    std::string getType() const { return m_type; }
    int getLine() const { return m_line; }
    int getColumn() const { return m_column; }
    
    void setValue(const std::string& value) { m_value = value; }
    void setType(const std::string& type) { m_type = type; }
    
    // 值类型检查
    bool isBoolean() const;
    bool isNumber() const;
    bool isString() const;
    bool isArray() const;
    
    // 值转换
    bool getBooleanValue() const;
    int getNumberValue() const;
    std::string getStringValue() const;
    std::vector<std::string> getArrayValue() const;

private:
    std::string m_key;
    std::string m_value;
    std::string m_type;
    int m_line;
    int m_column;
};

/**
 * @brief 配置组
 */
class ConfigGroup {
public:
    ConfigGroup(const std::string& name = "", ConfigType type = ConfigType::BASIC);
    ~ConfigGroup() = default;

    std::string getName() const { return m_name; }
    ConfigType getType() const { return m_type; }
    bool isNamed() const { return !m_name.empty(); }
    
    // 配置项管理
    void addConfigItem(std::shared_ptr<ConfigItem> item);
    void addConfigItem(const std::string& key, const std::string& value, 
                      const std::string& type = "");
    
    std::shared_ptr<ConfigItem> getConfigItem(const std::string& key) const;
    std::string getConfigValue(const std::string& key) const;
    bool hasConfigItem(const std::string& key) const;
    
    const std::unordered_map<std::string, std::shared_ptr<ConfigItem>>& getConfigItems() const;
    
    // 配置验证
    bool validateConfig() const;
    void validateConfigItem(std::shared_ptr<ConfigItem> item) const;

private:
    std::string m_name;
    ConfigType m_type;
    std::unordered_map<std::string, std::shared_ptr<ConfigItem>> m_configItems;
};

/**
 * @brief 配置管理器
 */
class ConfigManager {
public:
    explicit ConfigManager(std::shared_ptr<CHTLContext> context);
    ~ConfigManager() = default;

    // 配置组管理
    void addConfigGroup(std::shared_ptr<ConfigGroup> configGroup);
    void addConfigGroup(const std::string& name, std::shared_ptr<ConfigGroup> configGroup);
    
    std::shared_ptr<ConfigGroup> getConfigGroup(const std::string& name) const;
    std::shared_ptr<ConfigGroup> getDefaultConfigGroup() const;
    
    // 配置项管理
    void setConfigValue(const std::string& key, const std::string& value, 
                       const std::string& groupName = "");
    std::string getConfigValue(const std::string& key, const std::string& groupName = "") const;
    bool hasConfigValue(const std::string& key, const std::string& groupName = "") const;
    
    // 配置应用
    void applyConfig(std::shared_ptr<BaseNode> root);
    void applyConfigGroup(std::shared_ptr<ConfigGroup> configGroup);
    
    // 配置验证
    bool validateAllConfigs() const;
    bool validateConfigGroup(std::shared_ptr<ConfigGroup> configGroup) const;
    
    // 清理
    void clear();
    void removeConfigGroup(const std::string& name);

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unordered_map<std::string, std::shared_ptr<ConfigGroup>> m_configGroups;
    std::shared_ptr<ConfigGroup> m_defaultConfigGroup;
    
    // 辅助方法
    void createDefaultConfigGroup();
    void applyBasicConfig(std::shared_ptr<ConfigGroup> configGroup);
    void applyNameConfig(std::shared_ptr<ConfigGroup> configGroup);
    void applyOriginTypeConfig(std::shared_ptr<ConfigGroup> configGroup);
};

/**
 * @brief 配置解析器
 */
class ConfigParser {
public:
    explicit ConfigParser(std::shared_ptr<CHTLContext> context);
    ~ConfigParser() = default;

    // 解析配置
    std::shared_ptr<ConfigGroup> parseConfigGroup(std::shared_ptr<ConfigNode> node);
    std::vector<std::shared_ptr<ConfigGroup>> parseConfigGroups(std::shared_ptr<BaseNode> root);
    
    // 解析配置项
    std::shared_ptr<ConfigItem> parseConfigItem(const std::string& key, 
                                               const std::string& value, 
                                               int line = 0, int column = 0);

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    std::string parseConfigValue(const std::string& value) const;
    std::string parseConfigType(const std::string& value) const;
    std::vector<std::string> parseArrayValue(const std::string& value) const;
};

/**
 * @brief 配置生成器
 */
class ConfigGenerator {
public:
    explicit ConfigGenerator(std::shared_ptr<CHTLContext> context);
    ~ConfigGenerator() = default;

    // 生成配置
    std::string generateConfig(std::shared_ptr<ConfigGroup> configGroup);
    std::string generateAllConfigs(const std::vector<std::shared_ptr<ConfigGroup>>& configGroups);
    
    // 生成配置文档
    std::string generateConfigDocumentation(std::shared_ptr<ConfigGroup> configGroup);

private:
    std::shared_ptr<CHTLContext> m_context;
    
    // 辅助方法
    std::string generateConfigItem(std::shared_ptr<ConfigItem> item) const;
    std::string generateConfigGroupHeader(std::shared_ptr<ConfigGroup> configGroup) const;
};

/**
 * @brief 配置处理器
 */
class ConfigProcessor {
public:
    explicit ConfigProcessor(std::shared_ptr<CHTLContext> context);
    ~ConfigProcessor() = default;

    // 处理配置
    void processConfigs(std::shared_ptr<BaseNode> root);
    
    // 应用配置
    void applyConfigs(std::shared_ptr<BaseNode> root);
    
    // 验证配置
    bool validateConfigs(std::shared_ptr<BaseNode> root);

private:
    std::shared_ptr<CHTLContext> m_context;
    std::unique_ptr<ConfigManager> m_configManager;
    std::unique_ptr<ConfigParser> m_parser;
    std::unique_ptr<ConfigGenerator> m_generator;
    
    // 辅助方法
    void processConfigGroup(std::shared_ptr<ConfigGroup> configGroup);
    void processNameConfig(std::shared_ptr<ConfigGroup> configGroup);
    void processOriginTypeConfig(std::shared_ptr<ConfigGroup> configGroup);
};

} // namespace CHTL