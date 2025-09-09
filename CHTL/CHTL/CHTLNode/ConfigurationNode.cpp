#include "ConfigurationNode.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <cctype>

namespace CHTL {

ConfigurationNode::ConfigurationNode(const std::string& name, ConfigurationType type)
    : BaseNode(NodeType::CONFIGURATION, name, ""), configurationType(type), configurationName(name),
      isActive(true), isProcessed(false), isValid(true), priority(0), isRequired(false),
      isOptional(false), isInherited(false) {
}

std::string ConfigurationNode::getConfigurationTypeName() const {
    switch (configurationType) {
        case ConfigurationType::GLOBAL: return "global";
        case ConfigurationType::LOCAL: return "local";
        case ConfigurationType::MODULE: return "module";
        case ConfigurationType::TEMPLATE: return "template";
        case ConfigurationType::CUSTOM: return "custom";
        default: return "unknown";
    }
}

void ConfigurationNode::setConfigurationParameter(const std::string& key, const std::string& value) {
    configurationParameters[key] = value;
}

std::string ConfigurationNode::getConfigurationParameter(const std::string& key) const {
    auto it = configurationParameters.find(key);
    return (it != configurationParameters.end()) ? it->second : "";
}

bool ConfigurationNode::hasConfigurationParameter(const std::string& key) const {
    return configurationParameters.find(key) != configurationParameters.end();
}

void ConfigurationNode::removeConfigurationParameter(const std::string& key) {
    configurationParameters.erase(key);
}

void ConfigurationNode::setConfigurationArray(const std::string& key, const std::vector<std::string>& values) {
    configurationArrays[key] = values;
}

std::vector<std::string> ConfigurationNode::getConfigurationArray(const std::string& key) const {
    auto it = configurationArrays.find(key);
    return (it != configurationArrays.end()) ? it->second : std::vector<std::string>();
}

bool ConfigurationNode::hasConfigurationArray(const std::string& key) const {
    return configurationArrays.find(key) != configurationArrays.end();
}

void ConfigurationNode::removeConfigurationArray(const std::string& key) {
    configurationArrays.erase(key);
}

void ConfigurationNode::addConfigurationArrayItem(const std::string& key, const std::string& item) {
    configurationArrays[key].push_back(item);
}

void ConfigurationNode::removeConfigurationArrayItem(const std::string& key, const std::string& item) {
    auto it = configurationArrays.find(key);
    if (it != configurationArrays.end()) {
        auto& array = it->second;
        array.erase(std::remove(array.begin(), array.end(), item), array.end());
    }
}

void ConfigurationNode::setConfigurationObject(const std::string& key, const std::map<std::string, std::string>& object) {
    configurationObjects[key] = object;
}

std::map<std::string, std::string> ConfigurationNode::getConfigurationObject(const std::string& key) const {
    auto it = configurationObjects.find(key);
    return (it != configurationObjects.end()) ? it->second : std::map<std::string, std::string>();
}

bool ConfigurationNode::hasConfigurationObject(const std::string& key) const {
    return configurationObjects.find(key) != configurationObjects.end();
}

void ConfigurationNode::removeConfigurationObject(const std::string& key) {
    configurationObjects.erase(key);
}

void ConfigurationNode::setConfigurationObjectProperty(const std::string& key, const std::string& property, const std::string& value) {
    configurationObjects[key][property] = value;
}

std::string ConfigurationNode::getConfigurationObjectProperty(const std::string& key, const std::string& property) const {
    auto it = configurationObjects.find(key);
    if (it != configurationObjects.end()) {
        auto propIt = it->second.find(property);
        return (propIt != it->second.end()) ? propIt->second : "";
    }
    return "";
}

void ConfigurationNode::removeConfigurationObjectProperty(const std::string& key, const std::string& property) {
    auto it = configurationObjects.find(key);
    if (it != configurationObjects.end()) {
        it->second.erase(property);
    }
}

void ConfigurationNode::addConfigurationRule(const std::string& rule) {
    if (std::find(configurationRules.begin(), configurationRules.end(), rule) == configurationRules.end()) {
        configurationRules.push_back(rule);
    }
}

void ConfigurationNode::removeConfigurationRule(const std::string& rule) {
    configurationRules.erase(std::remove(configurationRules.begin(), configurationRules.end(), rule), configurationRules.end());
}

bool ConfigurationNode::hasConfigurationRule(const std::string& rule) const {
    return std::find(configurationRules.begin(), configurationRules.end(), rule) != configurationRules.end();
}

void ConfigurationNode::setRuleValue(const std::string& rule, const std::string& value) {
    ruleValues[rule] = value;
}

std::string ConfigurationNode::getRuleValue(const std::string& rule) const {
    auto it = ruleValues.find(rule);
    return (it != ruleValues.end()) ? it->second : "";
}

bool ConfigurationNode::hasRuleValue(const std::string& rule) const {
    return ruleValues.find(rule) != ruleValues.end();
}

void ConfigurationNode::removeRuleValue(const std::string& rule) {
    ruleValues.erase(rule);
}

void ConfigurationNode::setRuleFunction(const std::string& rule, std::function<bool(const std::string&)> function) {
    ruleFunctions[rule] = function;
}

std::function<bool(const std::string&)> ConfigurationNode::getRuleFunction(const std::string& rule) const {
    auto it = ruleFunctions.find(rule);
    return (it != ruleFunctions.end()) ? it->second : nullptr;
}

bool ConfigurationNode::hasRuleFunction(const std::string& rule) const {
    return ruleFunctions.find(rule) != ruleFunctions.end();
}

void ConfigurationNode::removeRuleFunction(const std::string& rule) {
    ruleFunctions.erase(rule);
}

void ConfigurationNode::setConfigurationOption(const std::string& key, bool value) {
    configurationOptions[key] = value;
}

bool ConfigurationNode::getConfigurationOption(const std::string& key) const {
    auto it = configurationOptions.find(key);
    return (it != configurationOptions.end()) ? it->second : false;
}

bool ConfigurationNode::hasConfigurationOption(const std::string& key) const {
    return configurationOptions.find(key) != configurationOptions.end();
}

void ConfigurationNode::removeConfigurationOption(const std::string& key) {
    configurationOptions.erase(key);
}

void ConfigurationNode::setConfigurationLimit(const std::string& key, int value) {
    configurationLimits[key] = value;
}

int ConfigurationNode::getConfigurationLimit(const std::string& key) const {
    auto it = configurationLimits.find(key);
    return (it != configurationLimits.end()) ? it->second : 0;
}

bool ConfigurationNode::hasConfigurationLimit(const std::string& key) const {
    return configurationLimits.find(key) != configurationLimits.end();
}

void ConfigurationNode::removeConfigurationLimit(const std::string& key) {
    configurationLimits.erase(key);
}

void ConfigurationNode::setConfigurationDefault(const std::string& key, const std::string& value) {
    configurationDefaults[key] = value;
}

std::string ConfigurationNode::getConfigurationDefault(const std::string& key) const {
    auto it = configurationDefaults.find(key);
    return (it != configurationDefaults.end()) ? it->second : "";
}

bool ConfigurationNode::hasConfigurationDefault(const std::string& key) const {
    return configurationDefaults.find(key) != configurationDefaults.end();
}

void ConfigurationNode::removeConfigurationDefault(const std::string& key) {
    configurationDefaults.erase(key);
}

void ConfigurationNode::addConfigurationError(const std::string& error) {
    configurationErrors.push_back(error);
}

void ConfigurationNode::addConfigurationWarning(const std::string& warning) {
    configurationWarnings.push_back(warning);
}

void ConfigurationNode::clearConfigurationMessages() {
    configurationErrors.clear();
    configurationWarnings.clear();
}

void ConfigurationNode::setMetadata(const std::string& key, const std::string& value) {
    metadata[key] = value;
}

std::string ConfigurationNode::getMetadata(const std::string& key) const {
    auto it = metadata.find(key);
    return (it != metadata.end()) ? it->second : "";
}

bool ConfigurationNode::hasMetadata(const std::string& key) const {
    return metadata.find(key) != metadata.end();
}

void ConfigurationNode::removeMetadata(const std::string& key) {
    metadata.erase(key);
}

bool ConfigurationNode::processConfiguration() {
    if (!isActive) {
        return true;
    }
    
    try {
        processConfigurationParameters();
        processConfigurationRules();
        processConfigurationOptions();
        processConfigurationLimits();
        processConfigurationDefaults();
        
        isProcessed = true;
        return true;
    } catch (const std::exception& e) {
        handleConfigurationError("处理配置时发生错误: " + std::string(e.what()));
        return false;
    }
}

bool ConfigurationNode::processGlobalConfiguration() {
    if (configurationType != ConfigurationType::GLOBAL) {
        return false;
    }
    
    return processConfiguration();
}

bool ConfigurationNode::processLocalConfiguration() {
    if (configurationType != ConfigurationType::LOCAL) {
        return false;
    }
    
    return processConfiguration();
}

bool ConfigurationNode::processModuleConfiguration() {
    if (configurationType != ConfigurationType::MODULE) {
        return false;
    }
    
    return processConfiguration();
}

bool ConfigurationNode::processTemplateConfiguration() {
    if (configurationType != ConfigurationType::TEMPLATE) {
        return false;
    }
    
    return processConfiguration();
}

bool ConfigurationNode::processCustomConfiguration() {
    if (configurationType != ConfigurationType::CUSTOM) {
        return false;
    }
    
    return processConfiguration();
}

bool ConfigurationNode::validateConfiguration() const {
    return validateParameters() && validateRules() && validateOptions() && validateLimits() && validateDefaults();
}

bool ConfigurationNode::validateParameters() const {
    for (const auto& param : configurationParameters) {
        if (param.first.empty() || param.second.empty()) {
            return false;
        }
    }
    return true;
}

bool ConfigurationNode::validateRules() const {
    for (const auto& rule : configurationRules) {
        if (rule.empty()) {
            return false;
        }
    }
    return true;
}

bool ConfigurationNode::validateOptions() const {
    for (const auto& option : configurationOptions) {
        if (option.first.empty()) {
            return false;
        }
    }
    return true;
}

bool ConfigurationNode::validateLimits() const {
    for (const auto& limit : configurationLimits) {
        if (limit.first.empty() || limit.second < 0) {
            return false;
        }
    }
    return true;
}

bool ConfigurationNode::validateDefaults() const {
    for (const auto& def : configurationDefaults) {
        if (def.first.empty() || def.second.empty()) {
            return false;
        }
    }
    return true;
}

bool ConfigurationNode::applyConfiguration() {
    if (!isProcessed) {
        return false;
    }
    
    try {
        applyConfigurationParameters();
        applyConfigurationRules();
        applyConfigurationOptions();
        applyConfigurationLimits();
        applyConfigurationDefaults();
        
        return true;
    } catch (const std::exception& e) {
        handleConfigurationError("应用配置时发生错误: " + std::string(e.what()));
        return false;
    }
}

bool ConfigurationNode::applyConfigurationRules() {
    for (const auto& rule : configurationRules) {
        if (hasRuleFunction(rule)) {
            auto func = getRuleFunction(rule);
            if (func && !func(rule)) {
                return false;
            }
        }
    }
    return true;
}

bool ConfigurationNode::applyConfigurationOptions() {
    for (const auto& option : configurationOptions) {
        // 应用配置选项
    }
    return true;
}

bool ConfigurationNode::applyConfigurationLimits() {
    for (const auto& limit : configurationLimits) {
        // 应用配置限制
    }
    return true;
}

bool ConfigurationNode::applyConfigurationDefaults() {
    for (const auto& def : configurationDefaults) {
        // 应用配置默认值
    }
    return true;
}

std::string ConfigurationNode::toHTML() const {
    std::ostringstream oss;
    oss << "<!-- Configuration: " << configurationName << " -->\n";
    oss << "<!-- Type: " << getConfigurationTypeName() << " -->\n";
    oss << "<!-- Group: " << configurationGroup << " -->\n";
    
    if (!configurationParameters.empty()) {
        oss << "<!-- Parameters: ";
        for (const auto& param : configurationParameters) {
            oss << param.first << "=" << param.second << " ";
        }
        oss << "-->\n";
    }
    
    if (!configurationRules.empty()) {
        oss << "<!-- Rules: ";
        for (const auto& rule : configurationRules) {
            oss << rule << " ";
        }
        oss << "-->\n";
    }
    
    return oss.str();
}

std::string ConfigurationNode::toCSS() const {
    std::ostringstream oss;
    oss << "/* Configuration: " << configurationName << " */\n";
    oss << "/* Type: " << getConfigurationTypeName() << " */\n";
    oss << "/* Group: " << configurationGroup << " */\n";
    
    if (!configurationParameters.empty()) {
        oss << "/* Parameters: ";
        for (const auto& param : configurationParameters) {
            oss << param.first << "=" << param.second << " ";
        }
        oss << "*/\n";
    }
    
    return oss.str();
}

std::string ConfigurationNode::toJavaScript() const {
    std::ostringstream oss;
    oss << "// Configuration: " << configurationName << "\n";
    oss << "// Type: " << getConfigurationTypeName() << "\n";
    oss << "// Group: " << configurationGroup << "\n";
    
    if (!configurationParameters.empty()) {
        oss << "// Parameters: ";
        for (const auto& param : configurationParameters) {
            oss << param.first << "=" << param.second << " ";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::string ConfigurationNode::toConfigurationString() const {
    std::ostringstream oss;
    oss << "configuration " << configurationName << " {\n";
    oss << "  type: " << getConfigurationTypeName() << ";\n";
    oss << "  group: " << configurationGroup << ";\n";
    
    if (!configurationParameters.empty()) {
        oss << "  parameters: {\n";
        for (const auto& param : configurationParameters) {
            oss << "    " << param.first << ": \"" << param.second << "\";\n";
        }
        oss << "  }\n";
    }
    
    if (!configurationArrays.empty()) {
        oss << "  arrays: {\n";
        for (const auto& array : configurationArrays) {
            oss << "    " << array.first << ": [";
            for (size_t i = 0; i < array.second.size(); ++i) {
                if (i > 0) oss << ", ";
                oss << "\"" << array.second[i] << "\"";
            }
            oss << "];\n";
        }
        oss << "  }\n";
    }
    
    if (!configurationObjects.empty()) {
        oss << "  objects: {\n";
        for (const auto& obj : configurationObjects) {
            oss << "    " << obj.first << ": {\n";
            for (const auto& prop : obj.second) {
                oss << "      " << prop.first << ": \"" << prop.second << "\";\n";
            }
            oss << "    }\n";
        }
        oss << "  }\n";
    }
    
    if (!configurationRules.empty()) {
        oss << "  rules: [";
        for (size_t i = 0; i < configurationRules.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << "\"" << configurationRules[i] << "\"";
        }
        oss << "];\n";
    }
    
    if (!configurationOptions.empty()) {
        oss << "  options: {\n";
        for (const auto& option : configurationOptions) {
            oss << "    " << option.first << ": " << (option.second ? "true" : "false") << ";\n";
        }
        oss << "  }\n";
    }
    
    if (!configurationLimits.empty()) {
        oss << "  limits: {\n";
        for (const auto& limit : configurationLimits) {
            oss << "    " << limit.first << ": " << limit.second << ";\n";
        }
        oss << "  }\n";
    }
    
    if (!configurationDefaults.empty()) {
        oss << "  defaults: {\n";
        for (const auto& def : configurationDefaults) {
            oss << "    " << def.first << ": \"" << def.second << "\";\n";
        }
        oss << "  }\n";
    }
    
    oss << "}\n";
    return oss.str();
}

std::string ConfigurationNode::formatConfiguration() const {
    return toConfigurationString();
}

std::string ConfigurationNode::minifyConfiguration() const {
    std::string result = toConfigurationString();
    
    // 移除多余的空格和换行
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex("\\s*\\{\\s*"), "{");
    result = std::regex_replace(result, std::regex("\\s*\\}\\s*"), "}");
    result = std::regex_replace(result, std::regex("\\s*;\\s*"), ";");
    result = std::regex_replace(result, std::regex("\\s*:\\s*"), ":");
    
    return result;
}

std::string ConfigurationNode::beautifyConfiguration() const {
    std::string result = toConfigurationString();
    
    // 美化格式
    result = std::regex_replace(result, std::regex("\\{"), "{\n  ");
    result = std::regex_replace(result, std::regex(";"), ";\n");
    result = std::regex_replace(result, std::regex("\\}"), "\n}\n");
    
    return result;
}

std::string ConfigurationNode::compressConfiguration() const {
    std::string result = toConfigurationString();
    
    // 压缩配置
    result = std::regex_replace(result, std::regex("\\s+"), "");
    result = std::regex_replace(result, std::regex("configuration"), "cfg");
    result = std::regex_replace(result, std::regex("parameters"), "params");
    result = std::regex_replace(result, std::regex("arrays"), "arr");
    result = std::regex_replace(result, std::regex("objects"), "obj");
    result = std::regex_replace(result, std::regex("options"), "opts");
    result = std::regex_replace(result, std::regex("limits"), "lim");
    result = std::regex_replace(result, std::regex("defaults"), "def");
    
    return result;
}

std::string ConfigurationNode::decompressConfiguration() const {
    std::string result = toConfigurationString();
    
    // 解压缩配置
    result = std::regex_replace(result, std::regex("cfg"), "configuration");
    result = std::regex_replace(result, std::regex("params"), "parameters");
    result = std::regex_replace(result, std::regex("arr"), "arrays");
    result = std::regex_replace(result, std::regex("obj"), "objects");
    result = std::regex_replace(result, std::regex("opts"), "options");
    result = std::regex_replace(result, std::regex("lim"), "limits");
    result = std::regex_replace(result, std::regex("def"), "defaults");
    
    return result;
}

std::string ConfigurationNode::encodeConfiguration() const {
    std::string result = toConfigurationString();
    
    // 编码配置
    for (char& c : result) {
        if (std::isalnum(c)) {
            c = static_cast<char>((c - 'a' + 13) % 26 + 'a');
        }
    }
    
    return result;
}

std::string ConfigurationNode::decodeConfiguration() const {
    std::string result = toConfigurationString();
    
    // 解码配置
    for (char& c : result) {
        if (std::isalnum(c)) {
            c = static_cast<char>((c - 'a' + 13) % 26 + 'a');
        }
    }
    
    return result;
}

std::string ConfigurationNode::getDebugInfo() const {
    std::ostringstream oss;
    oss << "ConfigurationNode Debug Info:\n";
    oss << "  Name: " << configurationName << "\n";
    oss << "  Type: " << getConfigurationTypeName() << "\n";
    oss << "  Group: " << configurationGroup << "\n";
    oss << "  Active: " << (isActive ? "true" : "false") << "\n";
    oss << "  Processed: " << (isProcessed ? "true" : "false") << "\n";
    oss << "  Valid: " << (isValid ? "true" : "false") << "\n";
    oss << "  Priority: " << priority << "\n";
    oss << "  Required: " << (isRequired ? "true" : "false") << "\n";
    oss << "  Optional: " << (isOptional ? "true" : "false") << "\n";
    oss << "  Inherited: " << (isInherited ? "true" : "false") << "\n";
    
    if (!configurationParameters.empty()) {
        oss << "  Parameters: ";
        for (const auto& param : configurationParameters) {
            oss << param.first << "=" << param.second << " ";
        }
        oss << "\n";
    }
    
    if (!configurationRules.empty()) {
        oss << "  Rules: ";
        for (const auto& rule : configurationRules) {
            oss << rule << " ";
        }
        oss << "\n";
    }
    
    if (!configurationErrors.empty()) {
        oss << "  Errors: ";
        for (const auto& error : configurationErrors) {
            oss << error << " ";
        }
        oss << "\n";
    }
    
    if (!configurationWarnings.empty()) {
        oss << "  Warnings: ";
        for (const auto& warning : configurationWarnings) {
            oss << warning << " ";
        }
        oss << "\n";
    }
    
    return oss.str();
}

std::shared_ptr<BaseNode> ConfigurationNode::clone() const {
    auto cloned = std::make_shared<ConfigurationNode>(configurationName, configurationType);
    
    // 复制基本属性
    cloned->setValue(value);
    cloned->setAttributes(attributes);
    
    // 复制配置属性
    cloned->configurationGroup = configurationGroup;
    cloned->configurationParameters = configurationParameters;
    cloned->configurationArrays = configurationArrays;
    cloned->configurationObjects = configurationObjects;
    cloned->configurationRules = configurationRules;
    cloned->ruleValues = ruleValues;
    cloned->ruleFunctions = ruleFunctions;
    cloned->configurationOptions = configurationOptions;
    cloned->configurationLimits = configurationLimits;
    cloned->configurationDefaults = configurationDefaults;
    cloned->isActive = isActive;
    cloned->isProcessed = isProcessed;
    cloned->isValid = isValid;
    cloned->configurationErrors = configurationErrors;
    cloned->configurationWarnings = configurationWarnings;
    cloned->metadata = metadata;
    cloned->priority = priority;
    cloned->isRequired = isRequired;
    cloned->isOptional = isOptional;
    cloned->isInherited = isInherited;
    
    return cloned;
}

bool ConfigurationNode::operator==(const ConfigurationNode& other) const {
    return configurationName == other.configurationName &&
           configurationType == other.configurationType &&
           configurationGroup == other.configurationGroup &&
           configurationParameters == other.configurationParameters &&
           configurationArrays == other.configurationArrays &&
           configurationObjects == other.configurationObjects &&
           configurationRules == other.configurationRules &&
           ruleValues == other.ruleValues &&
           configurationOptions == other.configurationOptions &&
           configurationLimits == other.configurationLimits &&
           configurationDefaults == other.configurationDefaults &&
           isActive == other.isActive &&
           isProcessed == other.isProcessed &&
           isValid == other.isValid &&
           priority == other.priority &&
           isRequired == other.isRequired &&
           isOptional == other.isOptional &&
           isInherited == other.isInherited;
}

bool ConfigurationNode::operator!=(const ConfigurationNode& other) const {
    return !(*this == other);
}

// 私有辅助方法实现
std::string ConfigurationNode::processConfigurationParameter(const std::string& key, const std::string& value) const {
    if (key.empty() || value.empty()) {
        return "";
    }
    
    std::string result = value;
    
    // 处理配置参数
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex("^\\s+|\\s+$"), "");
    
    return result;
}

std::string ConfigurationNode::validateConfigurationParameter(const std::string& key, const std::string& value) const {
    if (key.empty() || value.empty()) {
        return "";
    }
    
    // 验证配置参数
    if (value.find("invalid") != std::string::npos) {
        return "";
    }
    
    return value;
}

std::string ConfigurationNode::sanitizeConfigurationParameter(const std::string& key, const std::string& value) const {
    if (key.empty() || value.empty()) {
        return "";
    }
    
    std::string result = value;
    
    // 清理配置参数
    result = std::regex_replace(result, std::regex("[^a-zA-Z0-9\\s\\-_=<>!&|()\\[\\]{}:;,\\.]"), "");
    
    return result;
}

std::string ConfigurationNode::parseConfigurationRule(const std::string& rule) const {
    if (rule.empty()) {
        return "";
    }
    
    // 解析配置规则
    return processConfigurationParameter("rule", rule);
}

std::string ConfigurationNode::evaluateConfigurationRule(const std::string& rule) const {
    if (rule.empty()) {
        return "";
    }
    
    // 评估配置规则
    return processConfigurationParameter("rule", rule);
}

std::string ConfigurationNode::optimizeConfigurationRule(const std::string& rule) const {
    if (rule.empty()) {
        return "";
    }
    
    // 优化配置规则
    return processConfigurationParameter("rule", rule);
}

void ConfigurationNode::processConfigurationParameters() {
    for (auto& param : configurationParameters) {
        param.second = processConfigurationParameter(param.first, param.second);
    }
}

void ConfigurationNode::processConfigurationRules() {
    for (const auto& rule : configurationRules) {
        // 处理配置规则
    }
}

void ConfigurationNode::processConfigurationOptions() {
    for (const auto& option : configurationOptions) {
        // 处理配置选项
    }
}

void ConfigurationNode::processConfigurationLimits() {
    for (const auto& limit : configurationLimits) {
        // 处理配置限制
    }
}

void ConfigurationNode::processConfigurationDefaults() {
    for (const auto& def : configurationDefaults) {
        // 处理配置默认值
    }
}

bool ConfigurationNode::validateConfigurationParameters() const {
    for (const auto& param : configurationParameters) {
        if (param.first.empty() || param.second.empty()) {
            return false;
        }
    }
    return true;
}

bool ConfigurationNode::validateConfigurationRules() const {
    for (const auto& rule : configurationRules) {
        if (rule.empty()) {
            return false;
        }
    }
    return true;
}

bool ConfigurationNode::validateConfigurationOptions() const {
    for (const auto& option : configurationOptions) {
        if (option.first.empty()) {
            return false;
        }
    }
    return true;
}

bool ConfigurationNode::validateConfigurationLimits() const {
    for (const auto& limit : configurationLimits) {
        if (limit.first.empty() || limit.second < 0) {
            return false;
        }
    }
    return true;
}

bool ConfigurationNode::validateConfigurationDefaults() const {
    for (const auto& def : configurationDefaults) {
        if (def.first.empty() || def.second.empty()) {
            return false;
        }
    }
    return true;
}

void ConfigurationNode::applyConfigurationParameters() {
    for (const auto& param : configurationParameters) {
        // 应用配置参数
    }
}

void ConfigurationNode::applyConfigurationRules() {
    for (const auto& rule : configurationRules) {
        // 应用配置规则
    }
}

void ConfigurationNode::applyConfigurationOptions() {
    for (const auto& option : configurationOptions) {
        // 应用配置选项
    }
}

void ConfigurationNode::applyConfigurationLimits() {
    for (const auto& limit : configurationLimits) {
        // 应用配置限制
    }
}

void ConfigurationNode::applyConfigurationDefaults() {
    for (const auto& def : configurationDefaults) {
        // 应用配置默认值
    }
}

void ConfigurationNode::handleConfigurationError(const std::string& error) {
    addConfigurationError(error);
    isValid = false;
}

void ConfigurationNode::handleConfigurationWarning(const std::string& warning) {
    addConfigurationWarning(warning);
}

} // namespace CHTL