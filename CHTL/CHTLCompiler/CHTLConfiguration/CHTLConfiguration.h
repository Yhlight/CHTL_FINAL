#ifndef CHTL_CONFIGURATION_H
#define CHTL_CONFIGURATION_H

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include "CHTLNode/CHTLNode.h"

namespace CHTL {

// Configuration types
enum class ConfigurationType {
    IndexCount,
    DebugMode,
    KeywordRename,
    DisableAutoClassId,
    DisableDefaultNamespace,
    CustomOriginTypes,
    StyleAutomationRules,
    ElementAutomationRules,
    TemplateAutomationRules
};

// Configuration value types
enum class ConfigurationValueType {
    Boolean,
    Integer,
    String,
    StringList,
    Map
};

// Configuration value
struct ConfigurationValue {
    ConfigurationValueType type;
    std::string string_value;
    int int_value;
    bool bool_value;
    std::vector<std::string> string_list_value;
    std::map<std::string, std::string> map_value;
    
    ConfigurationValue();
    ConfigurationValue(bool value);
    ConfigurationValue(int value);
    ConfigurationValue(const std::string& value);
    ConfigurationValue(const std::vector<std::string>& value);
    ConfigurationValue(const std::map<std::string, std::string>& value);
};

// Configuration group
struct ConfigurationGroup {
    std::string name;
    std::string parent_name;
    std::map<std::string, ConfigurationValue> settings;
    std::vector<std::string> child_groups;
    bool is_default;
    bool is_active;
    int priority;
};

// Automation rule
struct AutomationRule {
    std::string rule_name;
    std::string target_type;  // style, element, template
    std::string condition;
    std::string action;
    std::map<std::string, std::string> parameters;
    bool is_enabled;
    int priority;
};

// Configuration node for AST (using existing ConfigurationNode from CHTLNode.h)
// The ConfigurationNode class is already defined in CHTLNode.h

// Configuration manager for handling configurations
class CHTLConfigurationManager {
public:
    CHTLConfigurationManager();
    ~CHTLConfigurationManager();
    
    // Configuration group management
    bool createConfigurationGroup(const std::string& name, const std::string& parent = "", bool is_default = false);
    bool deleteConfigurationGroup(const std::string& name);
    bool renameConfigurationGroup(const std::string& old_name, const std::string& new_name);
    bool activateConfigurationGroup(const std::string& name);
    bool deactivateConfigurationGroup(const std::string& name);
    
    // Configuration setting management
    bool setConfiguration(const std::string& group_name, const std::string& setting_name, const ConfigurationValue& value);
    bool getConfiguration(const std::string& group_name, const std::string& setting_name, ConfigurationValue& value) const;
    bool removeConfiguration(const std::string& group_name, const std::string& setting_name);
    bool hasConfiguration(const std::string& group_name, const std::string& setting_name) const;
    
    // Configuration queries
    std::shared_ptr<ConfigurationGroup> getConfigurationGroup(const std::string& name) const;
    std::vector<std::string> getConfigurationGroupNames() const;
    std::vector<std::string> getActiveConfigurationGroups() const;
    std::vector<std::string> getConfigurationNames(const std::string& group_name) const;
    bool configurationGroupExists(const std::string& name) const;
    
    // Automation rule management
    bool addAutomationRule(const AutomationRule& rule);
    bool removeAutomationRule(const std::string& rule_name);
    bool updateAutomationRule(const std::string& rule_name, const AutomationRule& rule);
    bool enableAutomationRule(const std::string& rule_name);
    bool disableAutomationRule(const std::string& rule_name);
    
    // Automation rule queries
    std::shared_ptr<AutomationRule> getAutomationRule(const std::string& rule_name) const;
    std::vector<std::string> getAutomationRuleNames() const;
    std::vector<std::string> getAutomationRuleNames(const std::string& target_type) const;
    bool automationRuleExists(const std::string& rule_name) const;
    
    // Configuration processing
    std::string processConfigurationDeclaration(const std::string& declaration);
    std::string processConfigurationValue(const std::string& value, ConfigurationValueType type);
    std::string processAutomationRule(const AutomationRule& rule);
    
    // Configuration validation
    bool validateConfigurationGroup(const std::string& name) const;
    bool validateConfigurationValue(const ConfigurationValue& value, ConfigurationType type) const;
    bool validateAutomationRule(const AutomationRule& rule) const;
    bool validateConfigurationName(const std::string& name) const;
    
    // Configuration inheritance
    ConfigurationValue getInheritedConfiguration(const std::string& group_name, const std::string& setting_name) const;
    std::map<std::string, ConfigurationValue> getAllInheritedConfigurations(const std::string& group_name) const;
    bool hasInheritedConfiguration(const std::string& group_name, const std::string& setting_name) const;
    
    // Utility methods
    void clearAllConfigurations();
    void clearAllAutomationRules();
    std::string getDefaultConfigurationGroup() const;
    void setDefaultConfigurationGroup(const std::string& name);
    
    // Configuration statistics
    int getConfigurationGroupCount() const;
    int getActiveConfigurationGroupCount() const;
    int getAutomationRuleCount() const;
    int getEnabledAutomationRuleCount() const;
    std::map<ConfigurationType, int> getConfigurationCountByType() const;
    
    // Configuration export/import
    std::string exportConfigurationGroup(const std::string& group_name) const;
    bool importConfigurationGroup(const std::string& group_name, const std::string& config_data);
    std::string exportAllConfigurations() const;
    bool importAllConfigurations(const std::string& config_data);
    
private:
    std::map<std::string, std::shared_ptr<ConfigurationGroup>> configuration_groups;
    std::map<std::string, std::shared_ptr<AutomationRule>> automation_rules;
    std::string default_configuration_group;
    
    // Helper methods
    std::string generateConfigurationKey(const std::string& name) const;
    std::string generateAutomationRuleKey(const std::string& name) const;
    bool isValidConfigurationGroupName(const std::string& name) const;
    bool isValidConfigurationName(const std::string& name) const;
    bool isValidAutomationRuleName(const std::string& name) const;
    std::string parseConfigurationValue(const std::string& value, ConfigurationValueType type);
    std::string serializeConfigurationValue(const ConfigurationValue& value) const;
    ConfigurationValue deserializeConfigurationValue(const std::string& data, ConfigurationValueType type) const;
    std::vector<std::string> getConfigurationGroupHierarchy(const std::string& group_name) const;
    void updateConfigurationGroupHierarchy();
    bool isConfigurationGroupActive(const std::string& group_name) const;
    std::string generateConfigurationGroupKey(const std::string& name) const;
};

} // namespace CHTL

#endif // CHTL_CONFIGURATION_H