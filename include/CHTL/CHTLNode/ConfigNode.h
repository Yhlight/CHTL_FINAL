#pragma once

#include "BaseNode.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace CHTL {

/**
 * @brief 配置类型枚举
 */
enum class ConfigType {
    BASIC,          // 基础配置
    NAMED,          // 命名配置
    NAME,           // Name配置块
    ORIGIN_TYPE     // OriginType配置块
};

/**
 * @brief 配置项类
 */
class ConfigItem {
public:
    ConfigItem(const std::string& key, const std::string& value);
    ~ConfigItem() = default;

    /**
     * @brief 获取配置键
     * @return 配置键
     */
    const std::string& getKey() const { return m_key; }

    /**
     * @brief 设置配置键
     * @param key 配置键
     */
    void setKey(const std::string& key) { m_key = key; }

    /**
     * @brief 获取配置值
     * @return 配置值
     */
    const std::string& getValue() const { return m_value; }

    /**
     * @brief 设置配置值
     * @param value 配置值
     */
    void setValue(const std::string& value) { m_value = value; }

    /**
     * @brief 检查是否为布尔值
     * @return 是否为布尔值
     */
    bool isBoolean() const;

    /**
     * @brief 检查是否为数字
     * @return 是否为数字
     */
    bool isNumber() const;

    /**
     * @brief 获取布尔值
     * @return 布尔值
     */
    bool getBooleanValue() const;

    /**
     * @brief 获取数字值
     * @return 数字值
     */
    int getNumberValue() const;

    /**
     * @brief 获取字符串值
     * @return 字符串值
     */
    const std::string& getStringValue() const { return m_value; }

private:
    std::string m_key;
    std::string m_value;
};

/**
 * @brief 配置节点类
 * 
 * 表示CHTL中的配置节点
 */
class ConfigNode : public BaseNode {
public:
    ConfigNode(ConfigType type, const std::string& name = "");
    ~ConfigNode() = default;

    /**
     * @brief 获取配置类型
     * @return 配置类型
     */
    ConfigType getConfigType() const { return m_configType; }

    /**
     * @brief 设置配置类型
     * @param type 配置类型
     */
    void setConfigType(ConfigType type) { m_configType = type; }

    /**
     * @brief 获取配置名称
     * @return 配置名称
     */
    const std::string& getConfigName() const { return m_configName; }

    /**
     * @brief 设置配置名称
     * @param name 配置名称
     */
    void setConfigName(const std::string& name) { m_configName = name; }

    /**
     * @brief 添加配置项
     * @param key 配置键
     * @param value 配置值
     */
    void addConfigItem(const std::string& key, const std::string& value);

    /**
     * @brief 获取配置项
     * @param key 配置键
     * @return 配置项
     */
    ConfigItem* getConfigItem(const std::string& key) const;

    /**
     * @brief 检查是否有配置项
     * @param key 配置键
     * @return 是否有该配置项
     */
    bool hasConfigItem(const std::string& key) const;

    /**
     * @brief 移除配置项
     * @param key 配置键
     * @return 是否成功移除
     */
    bool removeConfigItem(const std::string& key);

    /**
     * @brief 获取所有配置项
     * @return 配置项映射
     */
    const std::unordered_map<std::string, std::unique_ptr<ConfigItem>>& getConfigItems() const { return m_configItems; }

    /**
     * @brief 添加关键字配置
     * @param keyword 关键字
     * @param values 值列表
     */
    void addKeywordConfig(const std::string& keyword, const std::vector<std::string>& values);

    /**
     * @brief 获取关键字配置
     * @param keyword 关键字
     * @return 值列表
     */
    std::vector<std::string> getKeywordConfig(const std::string& keyword) const;

    /**
     * @brief 检查是否有关键字配置
     * @param keyword 关键字
     * @return 是否有该关键字配置
     */
    bool hasKeywordConfig(const std::string& keyword) const;

    /**
     * @brief 获取所有关键字配置
     * @return 关键字配置映射
     */
    const std::unordered_map<std::string, std::vector<std::string>>& getKeywordConfigs() const { return m_keywordConfigs; }

    /**
     * @brief 检查是否为命名配置
     * @return 是否为命名配置
     */
    bool isNamedConfig() const { return !m_configName.empty(); }

    /**
     * @brief 检查是否为Name配置
     * @return 是否为Name配置
     */
    bool isNameConfig() const { return m_configType == ConfigType::NAME; }

    /**
     * @brief 检查是否为OriginType配置
     * @return 是否为OriginType配置
     */
    bool isOriginTypeConfig() const { return m_configType == ConfigType::ORIGIN_TYPE; }

    /**
     * @brief 获取配置值
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 配置值
     */
    std::string getConfigValue(const std::string& key, const std::string& defaultValue = "") const;

    /**
     * @brief 获取布尔配置值
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 布尔值
     */
    bool getBooleanConfig(const std::string& key, bool defaultValue = false) const;

    /**
     * @brief 获取数字配置值
     * @param key 配置键
     * @param defaultValue 默认值
     * @return 数字值
     */
    int getNumberConfig(const std::string& key, int defaultValue = 0) const;

    /**
     * @brief 克隆节点
     * @return 克隆的节点
     */
    std::unique_ptr<BaseNode> clone() const override;

    /**
     * @brief 接受访问者
     * @param visitor 访问者
     */
    void accept(NodeVisitor* visitor) override;

    /**
     * @brief 获取节点字符串表示
     * @return 字符串表示
     */
    std::string toString() const override;

    /**
     * @brief 获取配置类型字符串
     * @return 类型字符串
     */
    static std::string configTypeToString(ConfigType type);

    /**
     * @brief 从字符串解析配置类型
     * @param str 字符串
     * @return 配置类型
     */
    static ConfigType parseConfigType(const std::string& str);

    /**
     * @brief 检查配置键是否有效
     * @param key 配置键
     * @return 是否有效
     */
    static bool isValidConfigKey(const std::string& key);

    /**
     * @brief 检查配置值是否有效
     * @param value 配置值
     * @return 是否有效
     */
    static bool isValidConfigValue(const std::string& value);

private:
    ConfigType m_configType;
    std::string m_configName;
    std::unordered_map<std::string, std::unique_ptr<ConfigItem>> m_configItems;
    std::unordered_map<std::string, std::vector<std::string>> m_keywordConfigs;
};

} // namespace CHTL