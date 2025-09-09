#include "CHTLConfiguration.h"
#include <algorithm>
#include <sstream>
#include <regex>

namespace CHTL {

ConfigurationValue::ConfigurationValue() : type(ConfigurationValueType::String), int_value(0), bool_value(false) {
}

ConfigurationValue::ConfigurationValue(bool value) : type(ConfigurationValueType::Boolean), int_value(0), bool_value(value) {
}

ConfigurationValue::ConfigurationValue(int value) : type(ConfigurationValueType::Integer), int_value(value), bool_value(false) {
}

ConfigurationValue::ConfigurationValue(const std::string& value) : type(ConfigurationValueType::String), int_value(0), bool_value(false), string_value(value) {
}

ConfigurationValue::ConfigurationValue(const std::vector<std::string>& value) : type(ConfigurationValueType::StringList), int_value(0), bool_value(false), string_list_value(value) {
}

ConfigurationValue::ConfigurationValue(const std::map<std::string, std::string>& value) : type(ConfigurationValueType::Map), int_value(0), bool_value(false), map_value(value) {
}

// ConfigurationNode is already implemented in CHTLNode.cpp

CHTLConfigurationManager::CHTLConfigurationManager() {
    // Initialize with default configuration group
    auto default_group = std::make_shared<ConfigurationGroup>();
    default_group->name = "default";
    default_group->parent_name = "";
    default_group->is_default = true;
    default_group->is_active = true;
    default_group->priority = 0;
    
    // Set default configurations
    default_group->settings["index_count"] = ConfigurationValue(0);
    default_group->settings["debug_mode"] = ConfigurationValue(false);
    default_group->settings["disable_auto_class_id"] = ConfigurationValue(false);
    default_group->settings["disable_default_namespace"] = ConfigurationValue(false);
    
    configuration_groups["default"] = default_group;
    default_configuration_group = "default";
}

CHTLConfigurationManager::~CHTLConfigurationManager() {
    // Cleanup
}

bool CHTLConfigurationManager::createConfigurationGroup(const std::string& name, const std::string& parent, bool is_default) {
    if (!isValidConfigurationGroupName(name)) {
        return false;
    }
    
    std::string key = generateConfigurationKey(name);
    if (configuration_groups.find(key) != configuration_groups.end()) {
        return false; // Group already exists
    }
    
    auto group = std::make_shared<ConfigurationGroup>();
    group->name = name;
    group->parent_name = parent;
    group->is_default = is_default;
    group->is_active = true;
    group->priority = 0;
    
    configuration_groups[key] = group;
    
    // Add to parent's child list
    if (!parent.empty()) {
        std::string parent_key = generateConfigurationKey(parent);
        auto parent_it = configuration_groups.find(parent_key);
        if (parent_it != configuration_groups.end()) {
            parent_it->second->child_groups.push_back(name);
        }
    }
    
    if (is_default) {
        default_configuration_group = name;
    }
    
    updateConfigurationGroupHierarchy();
    return true;
}

bool CHTLConfigurationManager::deleteConfigurationGroup(const std::string& name) {
    std::string key = generateConfigurationKey(name);
    auto it = configuration_groups.find(key);
    if (it == configuration_groups.end()) {
        return false;
    }
    
    // Check if group has children
    if (!it->second->child_groups.empty()) {
        return false; // Cannot delete group with children
    }
    
    // Remove from parent's child list
    if (!it->second->parent_name.empty()) {
        std::string parent_key = generateConfigurationKey(it->second->parent_name);
        auto parent_it = configuration_groups.find(parent_key);
        if (parent_it != configuration_groups.end()) {
            auto& children = parent_it->second->child_groups;
            children.erase(std::remove(children.begin(), children.end(), name), children.end());
        }
    }
    
    configuration_groups.erase(it);
    updateConfigurationGroupHierarchy();
    return true;
}

bool CHTLConfigurationManager::renameConfigurationGroup(const std::string& old_name, const std::string& new_name) {
    if (!isValidConfigurationGroupName(new_name)) {
        return false;
    }
    
    std::string old_key = generateConfigurationKey(old_name);
    auto it = configuration_groups.find(old_key);
    if (it == configuration_groups.end()) {
        return false;
    }
    
    std::string new_key = generateConfigurationKey(new_name);
    if (configuration_groups.find(new_key) != configuration_groups.end()) {
        return false; // New name already exists
    }
    
    // Update group
    it->second->name = new_name;
    configuration_groups[new_key] = it->second;
    configuration_groups.erase(old_key);
    
    // Update parent's child list
    if (!it->second->parent_name.empty()) {
        std::string parent_key = generateConfigurationKey(it->second->parent_name);
        auto parent_it = configuration_groups.find(parent_key);
        if (parent_it != configuration_groups.end()) {
            auto& children = parent_it->second->child_groups;
            std::replace(children.begin(), children.end(), old_name, new_name);
        }
    }
    
    updateConfigurationGroupHierarchy();
    return true;
}

bool CHTLConfigurationManager::activateConfigurationGroup(const std::string& name) {
    std::string key = generateConfigurationKey(name);
    auto it = configuration_groups.find(key);
    if (it == configuration_groups.end()) {
        return false;
    }
    
    it->second->is_active = true;
    return true;
}

bool CHTLConfigurationManager::deactivateConfigurationGroup(const std::string& name) {
    std::string key = generateConfigurationKey(name);
    auto it = configuration_groups.find(key);
    if (it == configuration_groups.end()) {
        return false;
    }
    
    it->second->is_active = false;
    return true;
}

bool CHTLConfigurationManager::setConfiguration(const std::string& group_name, const std::string& setting_name, const ConfigurationValue& value) {
    if (!isValidConfigurationName(setting_name)) {
        return false;
    }
    
    std::string key = generateConfigurationKey(group_name);
    auto it = configuration_groups.find(key);
    if (it == configuration_groups.end()) {
        return false;
    }
    
    it->second->settings[setting_name] = value;
    return true;
}

bool CHTLConfigurationManager::getConfiguration(const std::string& group_name, const std::string& setting_name, ConfigurationValue& value) const {
    std::string key = generateConfigurationKey(group_name);
    auto it = configuration_groups.find(key);
    if (it == configuration_groups.end()) {
        return false;
    }
    
    auto setting_it = it->second->settings.find(setting_name);
    if (setting_it == it->second->settings.end()) {
        return false;
    }
    
    value = setting_it->second;
    return true;
}

bool CHTLConfigurationManager::removeConfiguration(const std::string& group_name, const std::string& setting_name) {
    std::string key = generateConfigurationKey(group_name);
    auto it = configuration_groups.find(key);
    if (it == configuration_groups.end()) {
        return false;
    }
    
    auto setting_it = it->second->settings.find(setting_name);
    if (setting_it == it->second->settings.end()) {
        return false;
    }
    
    it->second->settings.erase(setting_it);
    return true;
}

bool CHTLConfigurationManager::hasConfiguration(const std::string& group_name, const std::string& setting_name) const {
    std::string key = generateConfigurationKey(group_name);
    auto it = configuration_groups.find(key);
    if (it == configuration_groups.end()) {
        return false;
    }
    
    return it->second->settings.find(setting_name) != it->second->settings.end();
}

std::shared_ptr<ConfigurationGroup> CHTLConfigurationManager::getConfigurationGroup(const std::string& name) const {
    std::string key = generateConfigurationKey(name);
    auto it = configuration_groups.find(key);
    return (it != configuration_groups.end()) ? it->second : nullptr;
}

std::vector<std::string> CHTLConfigurationManager::getConfigurationGroupNames() const {
    std::vector<std::string> names;
    for (const auto& pair : configuration_groups) {
        names.push_back(pair.second->name);
    }
    return names;
}

std::vector<std::string> CHTLConfigurationManager::getActiveConfigurationGroups() const {
    std::vector<std::string> names;
    for (const auto& pair : configuration_groups) {
        if (pair.second->is_active) {
            names.push_back(pair.second->name);
        }
    }
    return names;
}

std::vector<std::string> CHTLConfigurationManager::getConfigurationNames(const std::string& group_name) const {
    std::string key = generateConfigurationKey(group_name);
    auto it = configuration_groups.find(key);
    if (it == configuration_groups.end()) {
        return {};
    }
    
    std::vector<std::string> names;
    for (const auto& pair : it->second->settings) {
        names.push_back(pair.first);
    }
    return names;
}

bool CHTLConfigurationManager::configurationGroupExists(const std::string& name) const {
    std::string key = generateConfigurationKey(name);
    return configuration_groups.find(key) != configuration_groups.end();
}

bool CHTLConfigurationManager::addAutomationRule(const AutomationRule& rule) {
    if (!validateAutomationRule(rule)) {
        return false;
    }
    
    std::string key = generateAutomationRuleKey(rule.rule_name);
    if (automation_rules.find(key) != automation_rules.end()) {
        return false; // Rule already exists
    }
    
    automation_rules[key] = std::make_shared<AutomationRule>(rule);
    return true;
}

bool CHTLConfigurationManager::removeAutomationRule(const std::string& rule_name) {
    std::string key = generateAutomationRuleKey(rule_name);
    auto it = automation_rules.find(key);
    if (it == automation_rules.end()) {
        return false;
    }
    
    automation_rules.erase(it);
    return true;
}

bool CHTLConfigurationManager::updateAutomationRule(const std::string& rule_name, const AutomationRule& rule) {
    if (!validateAutomationRule(rule)) {
        return false;
    }
    
    std::string key = generateAutomationRuleKey(rule_name);
    auto it = automation_rules.find(key);
    if (it == automation_rules.end()) {
        return false;
    }
    
    it->second = std::make_shared<AutomationRule>(rule);
    return true;
}

bool CHTLConfigurationManager::enableAutomationRule(const std::string& rule_name) {
    std::string key = generateAutomationRuleKey(rule_name);
    auto it = automation_rules.find(key);
    if (it == automation_rules.end()) {
        return false;
    }
    
    it->second->is_enabled = true;
    return true;
}

bool CHTLConfigurationManager::disableAutomationRule(const std::string& rule_name) {
    std::string key = generateAutomationRuleKey(rule_name);
    auto it = automation_rules.find(key);
    if (it == automation_rules.end()) {
        return false;
    }
    
    it->second->is_enabled = false;
    return true;
}

std::shared_ptr<AutomationRule> CHTLConfigurationManager::getAutomationRule(const std::string& rule_name) const {
    std::string key = generateAutomationRuleKey(rule_name);
    auto it = automation_rules.find(key);
    return (it != automation_rules.end()) ? it->second : nullptr;
}

std::vector<std::string> CHTLConfigurationManager::getAutomationRuleNames() const {
    std::vector<std::string> names;
    for (const auto& pair : automation_rules) {
        names.push_back(pair.second->rule_name);
    }
    return names;
}

std::vector<std::string> CHTLConfigurationManager::getAutomationRuleNames(const std::string& target_type) const {
    std::vector<std::string> names;
    for (const auto& pair : automation_rules) {
        if (pair.second->target_type == target_type) {
            names.push_back(pair.second->rule_name);
        }
    }
    return names;
}

bool CHTLConfigurationManager::automationRuleExists(const std::string& rule_name) const {
    std::string key = generateAutomationRuleKey(rule_name);
    return automation_rules.find(key) != automation_rules.end();
}

std::string CHTLConfigurationManager::processConfigurationDeclaration(const std::string& declaration) {
    // Simple configuration declaration processing
    // This would parse configuration declarations and create configuration groups
    return declaration;
}

std::string CHTLConfigurationManager::processConfigurationValue(const std::string& value, ConfigurationValueType type) {
    return parseConfigurationValue(value, type);
}

std::string CHTLConfigurationManager::processAutomationRule(const AutomationRule& rule) {
    // Simple automation rule processing
    // This would process automation rules and apply them
    return rule.action;
}

bool CHTLConfigurationManager::validateConfigurationGroup(const std::string& name) const {
    return configurationGroupExists(name);
}

bool CHTLConfigurationManager::validateConfigurationValue(const ConfigurationValue& value, ConfigurationType type) const {
    // Simple validation based on type
    switch (type) {
        case ConfigurationType::IndexCount:
            return value.type == ConfigurationValueType::Integer;
        case ConfigurationType::DebugMode:
            return value.type == ConfigurationValueType::Boolean;
        case ConfigurationType::KeywordRename:
            return value.type == ConfigurationValueType::Map;
        case ConfigurationType::DisableAutoClassId:
            return value.type == ConfigurationValueType::Boolean;
        case ConfigurationType::DisableDefaultNamespace:
            return value.type == ConfigurationValueType::Boolean;
        case ConfigurationType::CustomOriginTypes:
            return value.type == ConfigurationValueType::StringList;
        case ConfigurationType::StyleAutomationRules:
            return value.type == ConfigurationValueType::StringList;
        case ConfigurationType::ElementAutomationRules:
            return value.type == ConfigurationValueType::StringList;
        case ConfigurationType::TemplateAutomationRules:
            return value.type == ConfigurationValueType::StringList;
        default:
            return false;
    }
}

bool CHTLConfigurationManager::validateAutomationRule(const AutomationRule& rule) const {
    // Validate rule name
    if (rule.rule_name.empty()) {
        return false;
    }
    
    // Validate target type
    if (rule.target_type.empty()) {
        return false;
    }
    
    // Validate condition and action
    if (rule.condition.empty() || rule.action.empty()) {
        return false;
    }
    
    return true;
}

bool CHTLConfigurationManager::validateConfigurationName(const std::string& name) const {
    return isValidConfigurationName(name);
}

ConfigurationValue CHTLConfigurationManager::getInheritedConfiguration(const std::string& group_name, const std::string& setting_name) const {
    // Get configuration from group hierarchy
    std::vector<std::string> hierarchy = getConfigurationGroupHierarchy(group_name);
    
    for (const auto& group_name_in_hierarchy : hierarchy) {
        std::string key = generateConfigurationKey(group_name_in_hierarchy);
        auto it = configuration_groups.find(key);
        if (it != configuration_groups.end()) {
            auto setting_it = it->second->settings.find(setting_name);
            if (setting_it != it->second->settings.end()) {
                return setting_it->second;
            }
        }
    }
    
    return ConfigurationValue();
}

std::map<std::string, ConfigurationValue> CHTLConfigurationManager::getAllInheritedConfigurations(const std::string& group_name) const {
    std::map<std::string, ConfigurationValue> all_configs;
    std::vector<std::string> hierarchy = getConfigurationGroupHierarchy(group_name);
    
    for (const auto& group_name_in_hierarchy : hierarchy) {
        std::string key = generateConfigurationKey(group_name_in_hierarchy);
        auto it = configuration_groups.find(key);
        if (it != configuration_groups.end()) {
            for (const auto& setting_pair : it->second->settings) {
                if (all_configs.find(setting_pair.first) == all_configs.end()) {
                    all_configs[setting_pair.first] = setting_pair.second;
                }
            }
        }
    }
    
    return all_configs;
}

bool CHTLConfigurationManager::hasInheritedConfiguration(const std::string& group_name, const std::string& setting_name) const {
    ConfigurationValue value = getInheritedConfiguration(group_name, setting_name);
    return value.type != ConfigurationValueType::String || !value.string_value.empty();
}

void CHTLConfigurationManager::clearAllConfigurations() {
    configuration_groups.clear();
    
    // Recreate default group
    auto default_group = std::make_shared<ConfigurationGroup>();
    default_group->name = "default";
    default_group->parent_name = "";
    default_group->is_default = true;
    default_group->is_active = true;
    default_group->priority = 0;
    
    default_group->settings["index_count"] = ConfigurationValue(0);
    default_group->settings["debug_mode"] = ConfigurationValue(false);
    default_group->settings["disable_auto_class_id"] = ConfigurationValue(false);
    default_group->settings["disable_default_namespace"] = ConfigurationValue(false);
    
    configuration_groups["default"] = default_group;
    default_configuration_group = "default";
}

void CHTLConfigurationManager::clearAllAutomationRules() {
    automation_rules.clear();
}

std::string CHTLConfigurationManager::getDefaultConfigurationGroup() const {
    return default_configuration_group;
}

void CHTLConfigurationManager::setDefaultConfigurationGroup(const std::string& name) {
    if (configurationGroupExists(name)) {
        default_configuration_group = name;
    }
}

int CHTLConfigurationManager::getConfigurationGroupCount() const {
    return configuration_groups.size();
}

int CHTLConfigurationManager::getActiveConfigurationGroupCount() const {
    int count = 0;
    for (const auto& pair : configuration_groups) {
        if (pair.second->is_active) {
            count++;
        }
    }
    return count;
}

int CHTLConfigurationManager::getAutomationRuleCount() const {
    return automation_rules.size();
}

int CHTLConfigurationManager::getEnabledAutomationRuleCount() const {
    int count = 0;
    for (const auto& pair : automation_rules) {
        if (pair.second->is_enabled) {
            count++;
        }
    }
    return count;
}

std::map<ConfigurationType, int> CHTLConfigurationManager::getConfigurationCountByType() const {
    std::map<ConfigurationType, int> counts;
    
    for (const auto& pair : configuration_groups) {
        for (const auto& setting_pair : pair.second->settings) {
            // This is a simplified implementation
            // In a real implementation, we would track configuration types
            (void)setting_pair; // Suppress unused variable warning
            counts[ConfigurationType::IndexCount]++;
        }
    }
    
    return counts;
}

std::string CHTLConfigurationManager::exportConfigurationGroup(const std::string& group_name) const {
    std::string key = generateConfigurationKey(group_name);
    auto it = configuration_groups.find(key);
    if (it == configuration_groups.end()) {
        return "";
    }
    
    // Simple export - just return group name
    return it->second->name;
}

bool CHTLConfigurationManager::importConfigurationGroup(const std::string& group_name, const std::string& /* config_data */) {
    // Simple import - just create a basic group
    return createConfigurationGroup(group_name);
}

std::string CHTLConfigurationManager::exportAllConfigurations() const {
    std::stringstream ss;
    for (const auto& pair : configuration_groups) {
        ss << pair.second->name << " ";
    }
    return ss.str();
}

bool CHTLConfigurationManager::importAllConfigurations(const std::string& /* config_data */) {
    // Simple import - just clear and recreate default
    clearAllConfigurations();
    return true;
}

std::string CHTLConfigurationManager::generateConfigurationKey(const std::string& name) const {
    return name;
}

std::string CHTLConfigurationManager::generateAutomationRuleKey(const std::string& name) const {
    return name;
}

bool CHTLConfigurationManager::isValidConfigurationGroupName(const std::string& name) const {
    return !name.empty() && std::regex_match(name, std::regex("^[a-zA-Z_][a-zA-Z0-9_]*$"));
}

bool CHTLConfigurationManager::isValidConfigurationName(const std::string& name) const {
    return !name.empty() && std::regex_match(name, std::regex("^[a-zA-Z_][a-zA-Z0-9_]*$"));
}

bool CHTLConfigurationManager::isValidAutomationRuleName(const std::string& name) const {
    return !name.empty() && std::regex_match(name, std::regex("^[a-zA-Z_][a-zA-Z0-9_]*$"));
}

std::string CHTLConfigurationManager::parseConfigurationValue(const std::string& value, ConfigurationValueType /* type */) {
    return value;
}

std::string CHTLConfigurationManager::serializeConfigurationValue(const ConfigurationValue& value) const {
    std::stringstream ss;
    ss << static_cast<int>(value.type) << ":" << value.string_value << ":" << value.int_value << ":" << value.bool_value;
    return ss.str();
}

ConfigurationValue CHTLConfigurationManager::deserializeConfigurationValue(const std::string& /* data */, ConfigurationValueType type) const {
    ConfigurationValue value;
    value.type = type;
    return value;
}

std::vector<std::string> CHTLConfigurationManager::getConfigurationGroupHierarchy(const std::string& group_name) const {
    std::vector<std::string> hierarchy;
    std::string current = group_name;
    
    while (!current.empty()) {
        hierarchy.push_back(current);
        std::string key = generateConfigurationKey(current);
        auto it = configuration_groups.find(key);
        if (it == configuration_groups.end()) {
            break;
        }
        current = it->second->parent_name;
    }
    
    return hierarchy;
}

void CHTLConfigurationManager::updateConfigurationGroupHierarchy() {
    // Update hierarchy information
    // This is a simplified implementation
}

bool CHTLConfigurationManager::isConfigurationGroupActive(const std::string& group_name) const {
    std::string key = generateConfigurationKey(group_name);
    auto it = configuration_groups.find(key);
    return (it != configuration_groups.end()) ? it->second->is_active : false;
}

std::string CHTLConfigurationManager::generateConfigurationGroupKey(const std::string& name) const {
    return generateConfigurationKey(name);
}

} // namespace CHTL