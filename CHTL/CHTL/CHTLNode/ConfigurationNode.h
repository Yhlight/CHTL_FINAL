#ifndef CHTL_CONFIGURATION_NODE_H
#define CHTL_CONFIGURATION_NODE_H

#include "BaseNode.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

class ConfigurationNode : public BaseNode {
private:
    // 配置类型
    enum class ConfigurationType {
        GLOBAL,         // 全局配置
        LOCAL,          // 局部配置
        MODULE,         // 模块配置
        TEMPLATE,       // 模板配置
        CUSTOM          // 自定义配置
    };
    
    ConfigurationType configurationType;
    std::string configurationName;
    std::string configurationGroup;
    
    // 配置参数
    std::map<std::string, std::string> configurationParameters;
    std::map<std::string, std::vector<std::string>> configurationArrays;
    std::map<std::string, std::map<std::string, std::string>> configurationObjects;
    
    // 配置规则
    std::vector<std::string> configurationRules;
    std::map<std::string, std::string> ruleValues;
    std::map<std::string, std::function<bool(const std::string&)>> ruleFunctions;
    
    // 配置选项
    std::map<std::string, bool> configurationOptions;
    std::map<std::string, int> configurationLimits;
    std::map<std::string, std::string> configurationDefaults;
    
    // 配置状态
    bool isActive;
    bool isProcessed;
    bool isValid;
    std::vector<std::string> configurationErrors;
    std::vector<std::string> configurationWarnings;
    
    // 配置元数据
    std::map<std::string, std::string> metadata;
    int priority;
    bool isRequired;
    bool isOptional;
    bool isInherited;

public:
    ConfigurationNode(const std::string& name, ConfigurationType type = ConfigurationType::GLOBAL);
    virtual ~ConfigurationNode() = default;
    
    // 配置类型
    ConfigurationType getConfigurationType() const { return configurationType; }
    void setConfigurationType(ConfigurationType type) { configurationType = type; }
    std::string getConfigurationTypeName() const;
    
    // 配置名称
    const std::string& getConfigurationName() const { return configurationName; }
    void setConfigurationName(const std::string& name) { configurationName = name; }
    
    // 配置组
    const std::string& getConfigurationGroup() const { return configurationGroup; }
    void setConfigurationGroup(const std::string& group) { configurationGroup = group; }
    
    // 配置参数
    void setConfigurationParameter(const std::string& key, const std::string& value);
    std::string getConfigurationParameter(const std::string& key) const;
    bool hasConfigurationParameter(const std::string& key) const;
    void removeConfigurationParameter(const std::string& key);
    const std::map<std::string, std::string>& getConfigurationParameters() const { return configurationParameters; }
    
    // 配置数组
    void setConfigurationArray(const std::string& key, const std::vector<std::string>& values);
    std::vector<std::string> getConfigurationArray(const std::string& key) const;
    bool hasConfigurationArray(const std::string& key) const;
    void removeConfigurationArray(const std::string& key);
    void addConfigurationArrayItem(const std::string& key, const std::string& item);
    void removeConfigurationArrayItem(const std::string& key, const std::string& item);
    const std::map<std::string, std::vector<std::string>>& getConfigurationArrays() const { return configurationArrays; }
    
    // 配置对象
    void setConfigurationObject(const std::string& key, const std::map<std::string, std::string>& object);
    std::map<std::string, std::string> getConfigurationObject(const std::string& key) const;
    bool hasConfigurationObject(const std::string& key) const;
    void removeConfigurationObject(const std::string& key);
    void setConfigurationObjectProperty(const std::string& key, const std::string& property, const std::string& value);
    std::string getConfigurationObjectProperty(const std::string& key, const std::string& property) const;
    void removeConfigurationObjectProperty(const std::string& key, const std::string& property);
    const std::map<std::string, std::map<std::string, std::string>>& getConfigurationObjects() const { return configurationObjects; }
    
    // 配置规则
    void addConfigurationRule(const std::string& rule);
    void removeConfigurationRule(const std::string& rule);
    const std::vector<std::string>& getConfigurationRules() const { return configurationRules; }
    bool hasConfigurationRule(const std::string& rule) const;
    
    void setRuleValue(const std::string& rule, const std::string& value);
    std::string getRuleValue(const std::string& rule) const;
    bool hasRuleValue(const std::string& rule) const;
    void removeRuleValue(const std::string& rule);
    const std::map<std::string, std::string>& getRuleValues() const { return ruleValues; }
    
    void setRuleFunction(const std::string& rule, std::function<bool(const std::string&)> function);
    std::function<bool(const std::string&)> getRuleFunction(const std::string& rule) const;
    bool hasRuleFunction(const std::string& rule) const;
    void removeRuleFunction(const std::string& rule);
    
    // 配置选项
    void setConfigurationOption(const std::string& key, bool value);
    bool getConfigurationOption(const std::string& key) const;
    bool hasConfigurationOption(const std::string& key) const;
    void removeConfigurationOption(const std::string& key);
    const std::map<std::string, bool>& getConfigurationOptions() const { return configurationOptions; }
    
    // 配置限制
    void setConfigurationLimit(const std::string& key, int value);
    int getConfigurationLimit(const std::string& key) const;
    bool hasConfigurationLimit(const std::string& key) const;
    void removeConfigurationLimit(const std::string& key);
    const std::map<std::string, int>& getConfigurationLimits() const { return configurationLimits; }
    
    // 配置默认值
    void setConfigurationDefault(const std::string& key, const std::string& value);
    std::string getConfigurationDefault(const std::string& key) const;
    bool hasConfigurationDefault(const std::string& key) const;
    void removeConfigurationDefault(const std::string& key);
    const std::map<std::string, std::string>& getConfigurationDefaults() const { return configurationDefaults; }
    
    // 配置状态
    bool isActiveConfiguration() const { return isActive; }
    void setActive(bool active) { isActive = active; }
    
    bool isProcessedConfiguration() const { return isProcessed; }
    void setProcessed(bool processed) { isProcessed = processed; }
    
    bool isValidConfiguration() const { return isValid; }
    void setValid(bool valid) { isValid = valid; }
    
    // 配置错误和警告
    const std::vector<std::string>& getConfigurationErrors() const { return configurationErrors; }
    const std::vector<std::string>& getConfigurationWarnings() const { return configurationWarnings; }
    void addConfigurationError(const std::string& error);
    void addConfigurationWarning(const std::string& warning);
    void clearConfigurationMessages();
    
    // 配置元数据
    void setMetadata(const std::string& key, const std::string& value);
    std::string getMetadata(const std::string& key) const;
    bool hasMetadata(const std::string& key) const;
    void removeMetadata(const std::string& key);
    const std::map<std::string, std::string>& getMetadata() const { return metadata; }
    
    // 配置特性
    int getPriority() const { return priority; }
    void setPriority(int pri) { priority = pri; }
    
    bool isRequiredConfiguration() const { return isRequired; }
    void setRequired(bool required) { isRequired = required; }
    
    bool isOptionalConfiguration() const { return isOptional; }
    void setOptional(bool optional) { isOptional = optional; }
    
    bool isInheritedConfiguration() const { return isInherited; }
    void setInherited(bool inherited) { isInherited = inherited; }
    
    // 配置处理
    bool processConfiguration();
    bool processGlobalConfiguration();
    bool processLocalConfiguration();
    bool processModuleConfiguration();
    bool processTemplateConfiguration();
    bool processCustomConfiguration();
    
    // 配置验证
    bool validateConfiguration() const;
    bool validateParameters() const;
    bool validateRules() const;
    bool validateOptions() const;
    bool validateLimits() const;
    bool validateDefaults() const;
    
    // 配置应用
    bool applyConfiguration();
    bool applyConfigurationRules();
    bool applyConfigurationOptions();
    bool applyConfigurationLimits();
    bool applyConfigurationDefaults();
    
    // 配置转换
    std::string toHTML() const override;
    std::string toCSS() const;
    std::string toJavaScript() const;
    std::string toConfigurationString() const;
    
    // 配置格式化
    std::string formatConfiguration() const;
    std::string minifyConfiguration() const;
    std::string beautifyConfiguration() const;
    
    // 配置压缩
    std::string compressConfiguration() const;
    std::string decompressConfiguration() const;
    
    // 配置编码
    std::string encodeConfiguration() const;
    std::string decodeConfiguration() const;
    
    // 调试信息
    std::string getDebugInfo() const override;
    
    // 克隆
    std::shared_ptr<BaseNode> clone() const override;
    
    // 比较操作
    bool operator==(const ConfigurationNode& other) const;
    bool operator!=(const ConfigurationNode& other) const;

private:
    // 辅助方法
    std::string processConfigurationParameter(const std::string& key, const std::string& value) const;
    std::string validateConfigurationParameter(const std::string& key, const std::string& value) const;
    std::string sanitizeConfigurationParameter(const std::string& key, const std::string& value) const;
    
    // 规则处理
    std::string parseConfigurationRule(const std::string& rule) const;
    std::string evaluateConfigurationRule(const std::string& rule) const;
    std::string optimizeConfigurationRule(const std::string& rule) const;
    
    // 配置处理
    void processConfigurationParameters();
    void processConfigurationRules();
    void processConfigurationOptions();
    void processConfigurationLimits();
    void processConfigurationDefaults();
    
    // 配置验证
    bool validateConfigurationParameters() const;
    bool validateConfigurationRules() const;
    bool validateConfigurationOptions() const;
    bool validateConfigurationLimits() const;
    bool validateConfigurationDefaults() const;
    
    // 配置应用
    void applyConfigurationParameters();
    void applyConfigurationRules();
    void applyConfigurationOptions();
    void applyConfigurationLimits();
    void applyConfigurationDefaults();
    
    // 错误处理
    void handleConfigurationError(const std::string& error);
    void handleConfigurationWarning(const std::string& warning);
};

} // namespace CHTL

#endif // CHTL_CONFIGURATION_NODE_H