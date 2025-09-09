#pragma once

#include "BaseNode.h"

namespace CHTL {

/**
 * @brief 配置节点
 * 表示CHTL配置定义
 */
class ConfigNode : public BaseNode {
public:
    ConfigNode(const std::string& name = "");
    virtual ~ConfigNode();
    
    // 配置名称
    void setConfigName(const std::string& name);
    std::string getConfigName() const;
    bool hasConfigName() const;
    
    // 配置项
    void setConfigValue(const std::string& key, const std::string& value);
    std::string getConfigValue(const std::string& key) const;
    bool hasConfigValue(const std::string& key) const;
    const std::unordered_map<std::string, std::string>& getConfigValues() const;
    
    // 名称配置组
    void setNameConfig(const std::string& key, const std::string& value);
    std::string getNameConfig(const std::string& key) const;
    bool hasNameConfig(const std::string& key) const;
    const std::unordered_map<std::string, std::string>& getNameConfigs() const;
    
    // 组选项配置
    void setGroupOption(const std::string& key, const std::vector<std::string>& values);
    std::vector<std::string> getGroupOption(const std::string& key) const;
    bool hasGroupOption(const std::string& key) const;
    const std::unordered_map<std::string, std::vector<std::string>>& getGroupOptions() const;
    
    // 原始嵌入类型配置
    void setOriginType(const std::string& typeName, const std::string& prefix);
    std::string getOriginType(const std::string& typeName) const;
    bool hasOriginType(const std::string& typeName) const;
    const std::unordered_map<std::string, std::string>& getOriginTypes() const;
    
    // 局部样式块自动化规则
    void setStyleAutoAddClass(bool enabled);
    bool getStyleAutoAddClass() const;
    
    void setStyleAutoAddID(bool enabled);
    bool getStyleAutoAddID() const;
    
    // 局部脚本自动化规则
    void setScriptAutoAddClass(bool enabled);
    bool getScriptAutoAddClass() const;
    
    void setScriptAutoAddID(bool enabled);
    bool getScriptAutoAddID() const;
    
    // 默认命名空间控制
    void setDisableDefaultNamespace(bool disabled);
    bool isDefaultNamespaceDisabled() const;
    
    // 自定义原始嵌入类型控制
    void setDisableCustomOriginType(bool disabled);
    bool isCustomOriginTypeDisabled() const;
    
    // 访问者模式
    virtual void accept(NodeVisitor& visitor) override;
    
    // 克隆节点
    virtual std::shared_ptr<BaseNode> clone() const override;
    
    // 节点验证
    virtual bool validate() const override;
    
    // 调试信息
    virtual std::string toString() const override;

private:
    std::string m_configName;
    std::unordered_map<std::string, std::string> m_configValues;
    std::unordered_map<std::string, std::string> m_nameConfigs;
    std::unordered_map<std::string, std::vector<std::string>> m_groupOptions;
    std::unordered_map<std::string, std::string> m_originTypes;
    
    // 自动化规则
    bool m_styleAutoAddClass;
    bool m_styleAutoAddID;
    bool m_scriptAutoAddClass;
    bool m_scriptAutoAddID;
    bool m_disableDefaultNamespace;
    bool m_disableCustomOriginType;
};

} // namespace CHTL