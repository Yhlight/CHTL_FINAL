#pragma once

#include "CHTL/CHTLNode/BaseNode.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace CHTL {

/**
 * @brief 配置类型枚举
 */
enum class ConfigurationType {
    KEYWORD,        // 关键字配置
    MODULE,         // 模块配置
    COMPILER,       // 编译器配置
    OUTPUT,         // 输出配置
    DEBUG,          // 调试配置
    UNKNOWN         // 未知类型
};

/**
 * @brief 配置节点类
 * 
 * 表示CHTL配置，支持关键字自定义和模块配置
 */
class ConfigurationNode : public BaseNode {
public:
    /**
     * @brief 构造函数
     * @param type 配置类型
     * @param name 配置名称
     */
    ConfigurationNode(ConfigurationType type, const std::string& name = "");
    
    /**
     * @brief 析构函数
     */
    ~ConfigurationNode() override;
    
    /**
     * @brief 设置配置类型
     * @param type 配置类型
     */
    void setConfigurationType(ConfigurationType type);
    
    /**
     * @brief 获取配置类型
     * @return 配置类型
     */
    ConfigurationType getConfigurationType() const;
    
    /**
     * @brief 设置配置名称
     * @param name 配置名称
     */
    void setConfigurationName(const std::string& name);
    
    /**
     * @brief 获取配置名称
     * @return 配置名称
     */
    const std::string& getConfigurationName() const;
    
    /**
     * @brief 添加配置项
     * @param key 配置键
     * @param value 配置值
     */
    void addConfigurationItem(const std::string& key, const std::string& value);
    
    /**
     * @brief 获取配置项
     * @param key 配置键
     * @return 配置值
     */
    std::string getConfigurationItem(const std::string& key) const;
    
    /**
     * @brief 检查是否有配置项
     * @param key 配置键
     * @return 是否有配置项
     */
    bool hasConfigurationItem(const std::string& key) const;
    
    /**
     * @brief 获取所有配置项
     * @return 配置项映射
     */
    const std::unordered_map<std::string, std::string>& getConfigurationItems() const;
    
    /**
     * @brief 设置配置组
     * @param group 配置组
     */
    void setConfigurationGroup(const std::string& group);
    
    /**
     * @brief 获取配置组
     * @return 配置组
     */
    const std::string& getConfigurationGroup() const;
    
    /**
     * @brief 检查是否有配置组
     * @return 是否有配置组
     */
    bool hasConfigurationGroup() const;
    
    /**
     * @brief 设置配置内容
     * @param content 配置内容
     */
    void setConfigurationContent(const std::string& content);
    
    /**
     * @brief 获取配置内容
     * @return 配置内容
     */
    const std::string& getConfigurationContent() const;
    
    /**
     * @brief 设置是否启用
     * @param enabled 是否启用
     */
    void setEnabled(bool enabled);
    
    /**
     * @brief 是否启用
     * @return 是否启用
     */
    bool isEnabled() const;
    
    /**
     * @brief 克隆节点
     * @return 克隆的节点
     */
    std::shared_ptr<BaseNode> clone() const override;
    
    /**
     * @brief 转换为字符串表示
     * @return 字符串表示
     */
    std::string toString() const override;
    
    /**
     * @brief 接受访问者
     * @param visitor 访问者
     */
    void accept(NodeVisitor& visitor) override {}

private:
    ConfigurationType m_configurationType;
    std::string m_configurationName;
    std::unordered_map<std::string, std::string> m_configurationItems;
    std::string m_configurationGroup;
    std::string m_configurationContent;
    bool m_enabled;
    
    /**
     * @brief 获取配置类型名称
     * @param type 配置类型
     * @return 类型名称
     */
    static std::string getConfigurationTypeName(ConfigurationType type);
};

} // namespace CHTL