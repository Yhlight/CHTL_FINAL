#ifndef CHTL_CONFIGURATION_PROCESSOR_H
#define CHTL_CONFIGURATION_PROCESSOR_H

#include "../CHTLNode/ConfigurationNode.h"
#include "../CHTLNode/BaseNode.h"
#include <string>
#include <map>
#include <vector>
#include <memory>
#include <functional>

namespace CHTL {

class CHTLConfigurationProcessor {
private:
    // 配置处理器状态
    bool debugMode;
    bool strictMode;
    bool enableConfigurations;
    bool enableValidation;
    bool enableWarnings;
    bool enableErrors;
    
    // 配置存储
    std::vector<std::shared_ptr<ConfigurationNode>> configurations;
    std::map<std::string, std::shared_ptr<ConfigurationNode>> configurationMap;
    std::map<std::string, std::vector<std::shared_ptr<ConfigurationNode>>> configurationGroups;
    
    // 配置类型映射
    std::map<std::string, ConfigurationNode::ConfigurationType> configurationTypeMap;
    std::map<ConfigurationNode::ConfigurationType, std::string> configurationTypeNames;
    
    // 配置规则映射
    std::map<std::string, std::function<bool(const std::string&)>> ruleFunctions;
    std::map<std::string, std::string> ruleValues;
    
    // 配置验证
    std::vector<std::string> validationErrors;
    std::vector<std::string> validationWarnings;
    std::vector<std::string> configurationErrors;
    
    // 配置统计
    int totalConfigurations;
    int activeConfigurations;
    int processedConfigurations;
    int validConfigurations;
    int errorConfigurations;
    int warningConfigurations;
    
    // 配置设置
    std::map<std::string, std::string> configurationSettings;
    std::map<std::string, bool> configurationFlags;
    std::map<std::string, int> configurationLimits;
    std::map<std::string, std::string> configurationDefaults;
    
    // 配置优先级
    std::map<std::string, int> configurationPriorities;
    std::map<int, std::vector<std::shared_ptr<ConfigurationNode>>> priorityConfigurations;

public:
    CHTLConfigurationProcessor();
    virtual ~CHTLConfigurationProcessor() = default;
    
    // 配置方法
    void setDebugMode(bool debug) { debugMode = debug; }
    bool isDebugMode() const { return debugMode; }
    
    void setStrictMode(bool strict) { strictMode = strict; }
    bool isStrictMode() const { return strictMode; }
    
    void setEnableConfigurations(bool enable) { enableConfigurations = enable; }
    bool isConfigurationsEnabled() const { return enableConfigurations; }
    
    void setEnableValidation(bool enable) { enableValidation = enable; }
    bool isValidationEnabled() const { return enableValidation; }
    
    void setEnableWarnings(bool enable) { enableWarnings = enable; }
    bool isWarningsEnabled() const { return enableWarnings; }
    
    void setEnableErrors(bool enable) { enableErrors = enable; }
    bool isErrorsEnabled() const { return enableErrors; }
    
    // 配置管理
    void addConfiguration(std::shared_ptr<ConfigurationNode> configuration);
    void removeConfiguration(const std::string& name);
    void removeConfiguration(std::shared_ptr<ConfigurationNode> configuration);
    std::shared_ptr<ConfigurationNode> getConfiguration(const std::string& name) const;
    std::vector<std::shared_ptr<ConfigurationNode>> getConfigurations() const;
    std::vector<std::shared_ptr<ConfigurationNode>> getConfigurationsByType(ConfigurationNode::ConfigurationType type) const;
    std::vector<std::shared_ptr<ConfigurationNode>> getConfigurationsByGroup(const std::string& group) const;
    
    // 配置组管理
    void addConfigurationToGroup(const std::string& groupName, std::shared_ptr<ConfigurationNode> configuration);
    void removeConfigurationFromGroup(const std::string& groupName, std::shared_ptr<ConfigurationNode> configuration);
    std::vector<std::string> getConfigurationGroups() const;
    
    // 配置类型管理
    void registerConfigurationType(const std::string& name, ConfigurationNode::ConfigurationType type);
    void unregisterConfigurationType(const std::string& name);
    ConfigurationNode::ConfigurationType getConfigurationType(const std::string& name) const;
    std::string getConfigurationTypeName(ConfigurationNode::ConfigurationType type) const;
    std::vector<std::string> getRegisteredConfigurationTypes() const;
    
    // 配置规则管理
    void registerRuleFunction(const std::string& name, std::function<bool(const std::string&)> function);
    void unregisterRuleFunction(const std::string& name);
    std::function<bool(const std::string&)> getRuleFunction(const std::string& name) const;
    std::vector<std::string> getRegisteredRuleFunctions() const;
    
    void setRuleValue(const std::string& name, const std::string& value);
    std::string getRuleValue(const std::string& name) const;
    bool hasRuleValue(const std::string& name) const;
    void removeRuleValue(const std::string& name);
    
    // 配置处理
    bool processConfigurations();
    bool processConfiguration(std::shared_ptr<ConfigurationNode> configuration);
    bool processConfigurationGroup(const std::string& groupName);
    bool processConfigurationType(ConfigurationNode::ConfigurationType type);
    
    // 配置验证
    bool validateConfigurations() const;
    bool validateConfiguration(std::shared_ptr<ConfigurationNode> configuration) const;
    bool validateConfigurationGroup(const std::string& groupName) const;
    bool validateConfigurationType(ConfigurationNode::ConfigurationType type) const;
    
    // 配置应用
    bool applyConfigurations();
    bool applyConfiguration(std::shared_ptr<ConfigurationNode> configuration);
    bool applyConfigurationGroup(const std::string& groupName);
    bool applyConfigurationType(ConfigurationNode::ConfigurationType type);
    
    // 配置解析
    std::shared_ptr<ConfigurationNode> parseConfiguration(const std::string& configurationText);
    std::vector<std::shared_ptr<ConfigurationNode>> parseConfigurations(const std::string& configurationsText);
    std::string parseConfigurationParameter(const std::string& parameter);
    std::vector<std::string> parseConfigurationArray(const std::string& array);
    std::map<std::string, std::string> parseConfigurationObject(const std::string& object);
    
    // 配置生成
    std::string generateConfigurations() const;
    std::string generateConfiguration(std::shared_ptr<ConfigurationNode> configuration) const;
    std::string generateConfigurationGroup(const std::string& groupName) const;
    std::string generateConfigurationType(ConfigurationNode::ConfigurationType type) const;
    
    // 配置转换
    std::string toHTML() const;
    std::string toCSS() const;
    std::string toJavaScript() const;
    std::string toConfigurationString() const;
    
    // 配置格式化
    std::string formatConfigurations() const;
    std::string minifyConfigurations() const;
    std::string beautifyConfigurations() const;
    
    // 配置压缩
    std::string compressConfigurations() const;
    std::string decompressConfigurations() const;
    
    // 配置编码
    std::string encodeConfigurations() const;
    std::string decodeConfigurations() const;
    
    // 配置统计
    int getTotalConfigurations() const { return totalConfigurations; }
    int getActiveConfigurations() const { return activeConfigurations; }
    int getProcessedConfigurations() const { return processedConfigurations; }
    int getValidConfigurations() const { return validConfigurations; }
    int getErrorConfigurations() const { return errorConfigurations; }
    int getWarningConfigurations() const { return warningConfigurations; }
    
    // 配置设置
    void setConfigurationSetting(const std::string& key, const std::string& value);
    std::string getConfigurationSetting(const std::string& key) const;
    bool hasConfigurationSetting(const std::string& key) const;
    void removeConfigurationSetting(const std::string& key);
    
    void setConfigurationFlag(const std::string& key, bool value);
    bool getConfigurationFlag(const std::string& key) const;
    bool hasConfigurationFlag(const std::string& key) const;
    void removeConfigurationFlag(const std::string& key);
    
    void setConfigurationLimit(const std::string& key, int value);
    int getConfigurationLimit(const std::string& key) const;
    bool hasConfigurationLimit(const std::string& key) const;
    void removeConfigurationLimit(const std::string& key);
    
    void setConfigurationDefault(const std::string& key, const std::string& value);
    std::string getConfigurationDefault(const std::string& key) const;
    bool hasConfigurationDefault(const std::string& key) const;
    void removeConfigurationDefault(const std::string& key);
    
    // 配置优先级
    void setConfigurationPriority(const std::string& name, int priority);
    int getConfigurationPriority(const std::string& name) const;
    bool hasConfigurationPriority(const std::string& name) const;
    void removeConfigurationPriority(const std::string& name);
    std::vector<std::shared_ptr<ConfigurationNode>> getConfigurationsByPriority(int priority) const;
    std::vector<int> getConfigurationPriorities() const;
    
    // 配置错误和警告
    const std::vector<std::string>& getValidationErrors() const { return validationErrors; }
    const std::vector<std::string>& getValidationWarnings() const { return validationWarnings; }
    const std::vector<std::string>& getConfigurationErrors() const { return configurationErrors; }
    void addValidationError(const std::string& error);
    void addValidationWarning(const std::string& warning);
    void addConfigurationError(const std::string& error);
    void clearValidationMessages();
    void clearConfigurationErrors();
    
    // 配置重置
    void reset();
    void resetConfigurations();
    void resetConfigurationGroups();
    void resetConfigurationTypes();
    void resetConfigurationRules();
    void resetConfigurationSettings();
    void resetConfigurationPriorities();
    
    // 配置清理
    void clearConfigurations();
    void clearConfigurationGroups();
    void clearConfigurationTypes();
    void clearConfigurationRules();
    void clearConfigurationSettings();
    void clearConfigurationPriorities();
    
    // 配置优化
    void optimizeConfigurations();
    void optimizeConfigurationGroups();
    void optimizeConfigurationTypes();
    void optimizeConfigurationRules();
    
    // 配置调试
    std::string getDebugInfo() const;
    std::string getConfigurationDebugInfo(std::shared_ptr<ConfigurationNode> configuration) const;
    std::string getConfigurationGroupDebugInfo(const std::string& groupName) const;
    std::string getConfigurationTypeDebugInfo(ConfigurationNode::ConfigurationType type) const;
    
    // 配置导出
    std::string exportConfigurations() const;
    std::string exportConfigurationGroup(const std::string& groupName) const;
    std::string exportConfigurationType(ConfigurationNode::ConfigurationType type) const;
    
    // 配置导入
    bool importConfigurations(const std::string& configurationsText);
    bool importConfigurationGroup(const std::string& groupName, const std::string& groupText);
    bool importConfigurationType(ConfigurationNode::ConfigurationType type, const std::string& typeText);
    
    // 配置克隆
    std::shared_ptr<CHTLConfigurationProcessor> clone() const;
    
    // 配置比较
    bool operator==(const CHTLConfigurationProcessor& other) const;
    bool operator!=(const CHTLConfigurationProcessor& other) const;

private:
    // 辅助方法
    void initializeConfigurationTypes();
    void initializeRuleFunctions();
    void updateConfigurationStatistics();
    void updateConfigurationMappings();
    void updateConfigurationPriorities();
    
    // 配置处理
    bool processGlobalConfigurations();
    bool processLocalConfigurations();
    bool processModuleConfigurations();
    bool processTemplateConfigurations();
    bool processCustomConfigurations();
    
    // 配置验证
    bool validateGlobalConfigurations() const;
    bool validateLocalConfigurations() const;
    bool validateModuleConfigurations() const;
    bool validateTemplateConfigurations() const;
    bool validateCustomConfigurations() const;
    
    // 配置应用
    bool applyGlobalConfigurations();
    bool applyLocalConfigurations();
    bool applyModuleConfigurations();
    bool applyTemplateConfigurations();
    bool applyCustomConfigurations();
    
    // 配置解析
    std::shared_ptr<ConfigurationNode> parseGlobalConfiguration(const std::string& configurationText);
    std::shared_ptr<ConfigurationNode> parseLocalConfiguration(const std::string& configurationText);
    std::shared_ptr<ConfigurationNode> parseModuleConfiguration(const std::string& configurationText);
    std::shared_ptr<ConfigurationNode> parseTemplateConfiguration(const std::string& configurationText);
    std::shared_ptr<ConfigurationNode> parseCustomConfiguration(const std::string& configurationText);
    
    // 配置生成
    std::string generateGlobalConfigurations() const;
    std::string generateLocalConfigurations() const;
    std::string generateModuleConfigurations() const;
    std::string generateTemplateConfigurations() const;
    std::string generateCustomConfigurations() const;
    
    // 错误处理
    void handleConfigurationError(const std::string& error);
    void handleConfigurationWarning(const std::string& warning);
    
    // 配置优化
    void optimizeConfiguration(std::shared_ptr<ConfigurationNode> configuration);
    void optimizeConfigurationGroup(const std::string& groupName);
    void optimizeConfigurationType(ConfigurationNode::ConfigurationType type);
    void optimizeConfigurationRule(const std::string& rule);
    
    // 配置调试
    std::string formatConfigurationDebugInfo(std::shared_ptr<ConfigurationNode> configuration) const;
    std::string formatConfigurationGroupDebugInfo(const std::string& groupName) const;
    std::string formatConfigurationTypeDebugInfo(ConfigurationNode::ConfigurationType type) const;
};

} // namespace CHTL

#endif // CHTL_CONFIGURATION_PROCESSOR_H