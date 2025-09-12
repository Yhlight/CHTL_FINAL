#include "CHTL/ConfigurationSystem.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

// ConfigurationValue 实现
ConfigurationValue::ConfigurationValue() : type_(ConfigurationType::STRING), bool_value_(false), int_value_(0) {
}

ConfigurationValue::ConfigurationValue(bool value) : type_(ConfigurationType::BOOLEAN), bool_value_(value), int_value_(0) {
}

ConfigurationValue::ConfigurationValue(int value) : type_(ConfigurationType::INTEGER), bool_value_(false), int_value_(value) {
}

ConfigurationValue::ConfigurationValue(const std::string& value) : type_(ConfigurationType::STRING), bool_value_(false), int_value_(0), string_value_(value) {
}

ConfigurationValue::ConfigurationValue(const std::vector<std::string>& value) : type_(ConfigurationType::ARRAY), bool_value_(false), int_value_(0), array_value_(value) {
}

ConfigurationType ConfigurationValue::getType() const {
    return type_;
}

bool ConfigurationValue::getBoolean() const {
    return bool_value_;
}

int ConfigurationValue::getInteger() const {
    return int_value_;
}

const std::string& ConfigurationValue::getString() const {
    return string_value_;
}

const std::vector<std::string>& ConfigurationValue::getArray() const {
    return array_value_;
}

void ConfigurationValue::setBoolean(bool value) {
    type_ = ConfigurationType::BOOLEAN;
    bool_value_ = value;
}

void ConfigurationValue::setInteger(int value) {
    type_ = ConfigurationType::INTEGER;
    int_value_ = value;
}

void ConfigurationValue::setString(const std::string& value) {
    type_ = ConfigurationType::STRING;
    string_value_ = value;
}

void ConfigurationValue::setArray(const std::vector<std::string>& value) {
    type_ = ConfigurationType::ARRAY;
    array_value_ = value;
}

std::string ConfigurationValue::toString() const {
    switch (type_) {
        case ConfigurationType::BOOLEAN:
            return bool_value_ ? "true" : "false";
        case ConfigurationType::INTEGER:
            return std::to_string(int_value_);
        case ConfigurationType::STRING:
            return string_value_;
        case ConfigurationType::ARRAY: {
            std::ostringstream oss;
            oss << "[";
            for (size_t i = 0; i < array_value_.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << array_value_[i] << "\"";
            }
            oss << "]";
            return oss.str();
        }
        default:
            return "";
    }
}

// NameConfiguration 实现
NameConfiguration::NameConfiguration() : OPTION_COUNT(3) {
    setDefaultValues();
}

void NameConfiguration::setDefaultValues() {
    CUSTOM_STYLE = "@Style";
    CUSTOM_ELEMENT = "@Element";
    CUSTOM_VAR = "@Var";
    TEMPLATE_STYLE = "@Style";
    TEMPLATE_ELEMENT = "@Element";
    TEMPLATE_VAR = "@Var";
    ORIGIN_HTML = "@Html";
    ORIGIN_STYLE = "@Style";
    ORIGIN_JAVASCRIPT = "@JavaScript";
    IMPORT_HTML = "@Html";
    IMPORT_STYLE = "@Style";
    IMPORT_JAVASCRIPT = "@JavaScript";
    IMPORT_CHTL = "@Chtl";
    IMPORT_CJMOD = "@CJmod";
    KEYWORD_INHERIT = "inherit";
    KEYWORD_DELETE = "delete";
    KEYWORD_INSERT = "insert";
    KEYWORD_AFTER = "after";
    KEYWORD_BEFORE = "before";
    KEYWORD_REPLACE = "replace";
    KEYWORD_ATTOP = "at top";
    KEYWORD_ATBOTTOM = "at bottom";
    KEYWORD_FROM = "from";
    KEYWORD_AS = "as";
    KEYWORD_EXCEPT = "except";
    KEYWORD_USE = "use";
    KEYWORD_HTML5 = "html5";
    KEYWORD_TEXT = "text";
    KEYWORD_STYLE = "style";
    KEYWORD_SCRIPT = "script";
    KEYWORD_CUSTOM = "[Custom]";
    KEYWORD_TEMPLATE = "[Template]";
    KEYWORD_ORIGIN = "[Origin]";
    KEYWORD_IMPORT = "[Import]";
    KEYWORD_NAMESPACE = "[Namespace]";
    CONFIGURATION_CONFIG = "@Config";
    IMPORT_CONFIG = "@Config";
    
    CUSTOM_STYLE_GROUP = {"@Style", "@style", "@CSS", "@Css", "@css"};
}

bool NameConfiguration::hasGroupOption(const std::string& key) const {
    if (key == "CUSTOM_STYLE") {
        return !CUSTOM_STYLE_GROUP.empty();
    }
    return false;
}

std::vector<std::string> NameConfiguration::getGroupOption(const std::string& key) const {
    if (key == "CUSTOM_STYLE") {
        return CUSTOM_STYLE_GROUP;
    }
    return {};
}

void NameConfiguration::setGroupOption(const std::string& key, const std::vector<std::string>& values) {
    if (key == "CUSTOM_STYLE") {
        CUSTOM_STYLE_GROUP = values;
    }
}

// ConfigurationGroup 实现
ConfigurationGroup::ConfigurationGroup(const std::string& name) : name_(name), has_name_configuration_(false) {
}

ConfigurationGroup::~ConfigurationGroup() = default;

const std::string& ConfigurationGroup::getName() const {
    return name_;
}

void ConfigurationGroup::setName(const std::string& name) {
    name_ = name;
}

bool ConfigurationGroup::isNamed() const {
    return !name_.empty();
}

void ConfigurationGroup::setConfiguration(const std::string& key, const ConfigurationValue& value) {
    configurations_[key] = value;
}

ConfigurationValue ConfigurationGroup::getConfiguration(const std::string& key) const {
    auto it = configurations_.find(key);
    if (it != configurations_.end()) {
        return it->second;
    }
    return ConfigurationValue();
}

bool ConfigurationGroup::hasConfiguration(const std::string& key) const {
    return configurations_.find(key) != configurations_.end();
}

void ConfigurationGroup::removeConfiguration(const std::string& key) {
    configurations_.erase(key);
}

void ConfigurationGroup::setNameConfiguration(const NameConfiguration& nameConfig) {
    name_configuration_ = nameConfig;
    has_name_configuration_ = true;
}

const NameConfiguration& ConfigurationGroup::getNameConfiguration() const {
    return name_configuration_;
}

NameConfiguration& ConfigurationGroup::getNameConfiguration() {
    return name_configuration_;
}

bool ConfigurationGroup::hasNameConfiguration() const {
    return has_name_configuration_;
}

std::vector<std::string> ConfigurationGroup::getConfigurationKeys() const {
    std::vector<std::string> keys;
    for (const auto& pair : configurations_) {
        keys.push_back(pair.first);
    }
    return keys;
}

std::map<std::string, ConfigurationValue> ConfigurationGroup::getAllConfigurations() const {
    return configurations_;
}

bool ConfigurationGroup::validate() const {
    return validateNameConfiguration() && validateGroupOptions();
}

std::vector<std::string> ConfigurationGroup::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (!validateNameConfiguration()) {
        errors.push_back("Name configuration validation failed");
    }
    
    if (!validateGroupOptions()) {
        errors.push_back("Group options validation failed");
    }
    
    return errors;
}

void ConfigurationGroup::clear() {
    configurations_.clear();
    name_configuration_ = NameConfiguration();
    has_name_configuration_ = false;
}

bool ConfigurationGroup::validateNameConfiguration() const {
    if (!has_name_configuration_) {
        return true;
    }
    
    // 验证必需的关键字
    if (name_configuration_.CUSTOM_STYLE.empty() ||
        name_configuration_.CUSTOM_ELEMENT.empty() ||
        name_configuration_.CUSTOM_VAR.empty()) {
        return false;
    }
    
    return true;
}

bool ConfigurationGroup::validateGroupOptions() const {
    if (!has_name_configuration_) {
        return true;
    }
    
    // 验证组选项数量
    if (name_configuration_.CUSTOM_STYLE_GROUP.size() > name_configuration_.OPTION_COUNT) {
        return false;
    }
    
    return true;
}

// ConfigurationNode 实现
ConfigurationNode::ConfigurationNode(const std::string& name) : configuration_name_(name), has_name_configuration_(false) {
}

ConfigurationNode::~ConfigurationNode() = default;

const std::string& ConfigurationNode::getConfigurationName() const {
    return configuration_name_;
}

void ConfigurationNode::setConfigurationName(const std::string& name) {
    configuration_name_ = name;
}

void ConfigurationNode::addConfiguration(const std::string& key, const ConfigurationValue& value) {
    configurations_[key] = value;
}

ConfigurationValue ConfigurationNode::getConfiguration(const std::string& key) const {
    auto it = configurations_.find(key);
    if (it != configurations_.end()) {
        return it->second;
    }
    return ConfigurationValue();
}

bool ConfigurationNode::hasConfiguration(const std::string& key) const {
    return configurations_.find(key) != configurations_.end();
}

void ConfigurationNode::removeConfiguration(const std::string& key) {
    configurations_.erase(key);
}

void ConfigurationNode::setNameConfiguration(const NameConfiguration& nameConfig) {
    name_configuration_ = nameConfig;
    has_name_configuration_ = true;
}

const NameConfiguration& ConfigurationNode::getNameConfiguration() const {
    return name_configuration_;
}

NameConfiguration& ConfigurationNode::getNameConfiguration() {
    return name_configuration_;
}

bool ConfigurationNode::hasNameConfiguration() const {
    return has_name_configuration_;
}

std::string ConfigurationNode::generateHTML() const {
    return generateConfigurationString();
}

std::string ConfigurationNode::generateConfigurationString() const {
    std::ostringstream oss;
    
    if (configuration_name_.empty()) {
        oss << "[Configuration]" << std::endl;
    } else {
        oss << "[Configuration] @Config " << configuration_name_ << std::endl;
    }
    
    oss << "{" << std::endl;
    
    // 生成配置项
    for (const auto& pair : configurations_) {
        oss << "    " << pair.first << " = " << pair.second.toString() << ";" << std::endl;
    }
    
    // 生成Name配置
    if (has_name_configuration_) {
        oss << generateNameBlock();
    }
    
    oss << "}" << std::endl;
    
    return oss.str();
}

bool ConfigurationNode::validate() const {
    if (has_name_configuration_) {
        return !name_configuration_.CUSTOM_STYLE.empty() &&
               !name_configuration_.CUSTOM_ELEMENT.empty() &&
               !name_configuration_.CUSTOM_VAR.empty();
    }
    return true;
}

std::vector<std::string> ConfigurationNode::getValidationErrors() const {
    std::vector<std::string> errors;
    
    if (has_name_configuration_) {
        if (name_configuration_.CUSTOM_STYLE.empty()) {
            errors.push_back("CUSTOM_STYLE is required");
        }
        if (name_configuration_.CUSTOM_ELEMENT.empty()) {
            errors.push_back("CUSTOM_ELEMENT is required");
        }
        if (name_configuration_.CUSTOM_VAR.empty()) {
            errors.push_back("CUSTOM_VAR is required");
        }
    }
    
    return errors;
}

std::string ConfigurationNode::generateConfigurationBlock() const {
    std::ostringstream oss;
    for (const auto& pair : configurations_) {
        oss << "    " << pair.first << " = " << pair.second.toString() << ";" << std::endl;
    }
    return oss.str();
}

std::string ConfigurationNode::generateNameBlock() const {
    std::ostringstream oss;
    oss << "    [Name]" << std::endl;
    oss << "    {" << std::endl;
    
    // 生成关键字配置
    oss << "        CUSTOM_STYLE = " << name_configuration_.CUSTOM_STYLE << ";" << std::endl;
    oss << "        CUSTOM_ELEMENT = " << name_configuration_.CUSTOM_ELEMENT << ";" << std::endl;
    oss << "        CUSTOM_VAR = " << name_configuration_.CUSTOM_VAR << ";" << std::endl;
    oss << "        TEMPLATE_STYLE = " << name_configuration_.TEMPLATE_STYLE << ";" << std::endl;
    oss << "        TEMPLATE_ELEMENT = " << name_configuration_.TEMPLATE_ELEMENT << ";" << std::endl;
    oss << "        TEMPLATE_VAR = " << name_configuration_.TEMPLATE_VAR << ";" << std::endl;
    oss << "        ORIGIN_HTML = " << name_configuration_.ORIGIN_HTML << ";" << std::endl;
    oss << "        ORIGIN_STYLE = " << name_configuration_.ORIGIN_STYLE << ";" << std::endl;
    oss << "        ORIGIN_JAVASCRIPT = " << name_configuration_.ORIGIN_JAVASCRIPT << ";" << std::endl;
    oss << "        IMPORT_HTML = " << name_configuration_.IMPORT_HTML << ";" << std::endl;
    oss << "        IMPORT_STYLE = " << name_configuration_.IMPORT_STYLE << ";" << std::endl;
    oss << "        IMPORT_JAVASCRIPT = " << name_configuration_.IMPORT_JAVASCRIPT << ";" << std::endl;
    oss << "        IMPORT_CHTL = " << name_configuration_.IMPORT_CHTL << ";" << std::endl;
    oss << "        IMPORT_CJMOD = " << name_configuration_.IMPORT_CJMOD << ";" << std::endl;
    oss << "        KEYWORD_INHERIT = " << name_configuration_.KEYWORD_INHERIT << ";" << std::endl;
    oss << "        KEYWORD_DELETE = " << name_configuration_.KEYWORD_DELETE << ";" << std::endl;
    oss << "        KEYWORD_INSERT = " << name_configuration_.KEYWORD_INSERT << ";" << std::endl;
    oss << "        KEYWORD_AFTER = " << name_configuration_.KEYWORD_AFTER << ";" << std::endl;
    oss << "        KEYWORD_BEFORE = " << name_configuration_.KEYWORD_BEFORE << ";" << std::endl;
    oss << "        KEYWORD_REPLACE = " << name_configuration_.KEYWORD_REPLACE << ";" << std::endl;
    oss << "        KEYWORD_ATTOP = " << name_configuration_.KEYWORD_ATTOP << ";" << std::endl;
    oss << "        KEYWORD_ATBOTTOM = " << name_configuration_.KEYWORD_ATBOTTOM << ";" << std::endl;
    oss << "        KEYWORD_FROM = " << name_configuration_.KEYWORD_FROM << ";" << std::endl;
    oss << "        KEYWORD_AS = " << name_configuration_.KEYWORD_AS << ";" << std::endl;
    oss << "        KEYWORD_EXCEPT = " << name_configuration_.KEYWORD_EXCEPT << ";" << std::endl;
    oss << "        KEYWORD_USE = " << name_configuration_.KEYWORD_USE << ";" << std::endl;
    oss << "        KEYWORD_HTML5 = " << name_configuration_.KEYWORD_HTML5 << ";" << std::endl;
    oss << "        KEYWORD_TEXT = " << name_configuration_.KEYWORD_TEXT << ";" << std::endl;
    oss << "        KEYWORD_STYLE = " << name_configuration_.KEYWORD_STYLE << ";" << std::endl;
    oss << "        KEYWORD_SCRIPT = " << name_configuration_.KEYWORD_SCRIPT << ";" << std::endl;
    oss << "        KEYWORD_CUSTOM = " << name_configuration_.KEYWORD_CUSTOM << ";" << std::endl;
    oss << "        KEYWORD_TEMPLATE = " << name_configuration_.KEYWORD_TEMPLATE << ";" << std::endl;
    oss << "        KEYWORD_ORIGIN = " << name_configuration_.KEYWORD_ORIGIN << ";" << std::endl;
    oss << "        KEYWORD_IMPORT = " << name_configuration_.KEYWORD_IMPORT << ";" << std::endl;
    oss << "        KEYWORD_NAMESPACE = " << name_configuration_.KEYWORD_NAMESPACE << ";" << std::endl;
    oss << "        CONFIGURATION_CONFIG = " << name_configuration_.CONFIGURATION_CONFIG << ";" << std::endl;
    oss << "        IMPORT_CONFIG = " << name_configuration_.IMPORT_CONFIG << ";" << std::endl;
    
    // 生成组选项
    if (!name_configuration_.CUSTOM_STYLE_GROUP.empty()) {
        oss << "        CUSTOM_STYLE = [";
        for (size_t i = 0; i < name_configuration_.CUSTOM_STYLE_GROUP.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << name_configuration_.CUSTOM_STYLE_GROUP[i];
        }
        oss << "];" << std::endl;
    }
    
    oss << "        OPTION_COUNT = " << name_configuration_.OPTION_COUNT << ";" << std::endl;
    
    oss << "    }" << std::endl;
    
    return oss.str();
}

std::string ConfigurationNode::generateConfigurationItem(const std::string& key, const ConfigurationValue& value) const {
    return key + " = " + value.toString() + ";";
}

std::string ConfigurationNode::generateGroupOption(const std::string& key, const std::vector<std::string>& values) const {
    std::ostringstream oss;
    oss << key << " = [";
    for (size_t i = 0; i < values.size(); ++i) {
        if (i > 0) oss << ", ";
        oss << values[i];
    }
    oss << "];";
    return oss.str();
}

} // namespace CHTL