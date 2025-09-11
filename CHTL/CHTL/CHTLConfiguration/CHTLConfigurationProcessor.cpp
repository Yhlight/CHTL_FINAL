#include "CHTLConfigurationProcessor.h"
#include <algorithm>
#include <sstream>
#include <regex>
#include <cctype>

namespace CHTL {

CHTLConfigurationProcessor::CHTLConfigurationProcessor()
    : debugMode(false), strictMode(false), enableConfigurations(true), enableValidation(true),
      enableWarnings(true), enableErrors(true), totalConfigurations(0), activeConfigurations(0),
      processedConfigurations(0), validConfigurations(0), errorConfigurations(0), warningConfigurations(0) {
    initializeConfigurationTypes();
    initializeRuleFunctions();
}

void CHTLConfigurationProcessor::initializeConfigurationTypes() {
    configurationTypeMap["global"] = ConfigurationNode::ConfigurationType::GLOBAL;
    configurationTypeMap["local"] = ConfigurationNode::ConfigurationType::LOCAL;
    configurationTypeMap["module"] = ConfigurationNode::ConfigurationType::MODULE;
    configurationTypeMap["template"] = ConfigurationNode::ConfigurationType::TEMPLATE;
    configurationTypeMap["custom"] = ConfigurationNode::ConfigurationType::CUSTOM;
    
    configurationTypeNames[ConfigurationNode::ConfigurationType::GLOBAL] = "global";
    configurationTypeNames[ConfigurationNode::ConfigurationType::LOCAL] = "local";
    configurationTypeNames[ConfigurationNode::ConfigurationType::MODULE] = "module";
    configurationTypeNames[ConfigurationNode::ConfigurationType::TEMPLATE] = "template";
    configurationTypeNames[ConfigurationNode::ConfigurationType::CUSTOM] = "custom";
}

void CHTLConfigurationProcessor::initializeRuleFunctions() {
    // 注册默认规则函数
    ruleFunctions["is_valid"] = [](const std::string& value) { return !value.empty(); };
    ruleFunctions["is_not_empty"] = [](const std::string& value) { return !value.empty(); };
    ruleFunctions["is_number"] = [](const std::string& value) { 
        return !value.empty() && std::all_of(value.begin(), value.end(), ::isdigit); 
    };
    ruleFunctions["is_alpha"] = [](const std::string& value) { 
        return !value.empty() && std::all_of(value.begin(), value.end(), ::isalpha); 
    };
    ruleFunctions["is_alnum"] = [](const std::string& value) { 
        return !value.empty() && std::all_of(value.begin(), value.end(), ::isalnum); 
    };
}

void CHTLConfigurationProcessor::addConfiguration(std::shared_ptr<ConfigurationNode> configuration) {
    if (!configuration) return;
    
    configurations.push_back(configuration);
    configurationMap[configuration->getConfigurationName()] = configuration;
    
    if (configuration->isActiveConfiguration()) {
        activeConfigurations++;
    }
    
    updateConfigurationStatistics();
    updateConfigurationMappings();
}

void CHTLConfigurationProcessor::removeConfiguration(const std::string& name) {
    auto it = configurationMap.find(name);
    if (it != configurationMap.end()) {
        auto configuration = it->second;
        configurationMap.erase(it);
        
        configurations.erase(std::remove(configurations.begin(), configurations.end(), configuration), configurations.end());
        
        if (configuration->isActiveConfiguration()) {
            activeConfigurations--;
        }
        
        updateConfigurationStatistics();
        updateConfigurationMappings();
    }
}

void CHTLConfigurationProcessor::removeConfiguration(std::shared_ptr<ConfigurationNode> configuration) {
    if (!configuration) return;
    
    removeConfiguration(configuration->getConfigurationName());
}

std::shared_ptr<ConfigurationNode> CHTLConfigurationProcessor::getConfiguration(const std::string& name) const {
    auto it = configurationMap.find(name);
    return (it != configurationMap.end()) ? it->second : nullptr;
}

std::vector<std::shared_ptr<ConfigurationNode>> CHTLConfigurationProcessor::getConfigurations() const {
    return configurations;
}

std::vector<std::shared_ptr<ConfigurationNode>> CHTLConfigurationProcessor::getConfigurationsByType(ConfigurationNode::ConfigurationType type) const {
    std::vector<std::shared_ptr<ConfigurationNode>> result;
    for (const auto& configuration : configurations) {
        if (configuration->getConfigurationType() == type) {
            result.push_back(configuration);
        }
    }
    return result;
}

std::vector<std::shared_ptr<ConfigurationNode>> CHTLConfigurationProcessor::getConfigurationsByGroup(const std::string& group) const {
    auto it = configurationGroups.find(group);
    return (it != configurationGroups.end()) ? it->second : std::vector<std::shared_ptr<ConfigurationNode>>();
}

void CHTLConfigurationProcessor::addConfigurationToGroup(const std::string& groupName, std::shared_ptr<ConfigurationNode> configuration) {
    if (!configuration) return;
    
    configurationGroups[groupName].push_back(configuration);
}

void CHTLConfigurationProcessor::removeConfigurationFromGroup(const std::string& groupName, std::shared_ptr<ConfigurationNode> configuration) {
    auto it = configurationGroups.find(groupName);
    if (it != configurationGroups.end()) {
        auto& group = it->second;
        group.erase(std::remove(group.begin(), group.end(), configuration), group.end());
    }
}

std::vector<std::string> CHTLConfigurationProcessor::getConfigurationGroups() const {
    std::vector<std::string> groups;
    for (const auto& pair : configurationGroups) {
        groups.push_back(pair.first);
    }
    return groups;
}

void CHTLConfigurationProcessor::registerConfigurationType(const std::string& name, ConfigurationNode::ConfigurationType type) {
    configurationTypeMap[name] = type;
    configurationTypeNames[type] = name;
}

void CHTLConfigurationProcessor::unregisterConfigurationType(const std::string& name) {
    auto it = configurationTypeMap.find(name);
    if (it != configurationTypeMap.end()) {
        configurationTypeNames.erase(it->second);
        configurationTypeMap.erase(it);
    }
}

ConfigurationNode::ConfigurationType CHTLConfigurationProcessor::getConfigurationType(const std::string& name) const {
    auto it = configurationTypeMap.find(name);
    return (it != configurationTypeMap.end()) ? it->second : ConfigurationNode::ConfigurationType::GLOBAL;
}

std::string CHTLConfigurationProcessor::getConfigurationTypeName(ConfigurationNode::ConfigurationType type) const {
    auto it = configurationTypeNames.find(type);
    return (it != configurationTypeNames.end()) ? it->second : "unknown";
}

std::vector<std::string> CHTLConfigurationProcessor::getRegisteredConfigurationTypes() const {
    std::vector<std::string> types;
    for (const auto& pair : configurationTypeMap) {
        types.push_back(pair.first);
    }
    return types;
}

void CHTLConfigurationProcessor::registerRuleFunction(const std::string& name, std::function<bool(const std::string&)> function) {
    ruleFunctions[name] = function;
}

void CHTLConfigurationProcessor::unregisterRuleFunction(const std::string& name) {
    ruleFunctions.erase(name);
}

std::function<bool(const std::string&)> CHTLConfigurationProcessor::getRuleFunction(const std::string& name) const {
    auto it = ruleFunctions.find(name);
    return (it != ruleFunctions.end()) ? it->second : nullptr;
}

std::vector<std::string> CHTLConfigurationProcessor::getRegisteredRuleFunctions() const {
    std::vector<std::string> functions;
    for (const auto& pair : ruleFunctions) {
        functions.push_back(pair.first);
    }
    return functions;
}

void CHTLConfigurationProcessor::setRuleValue(const std::string& name, const std::string& value) {
    ruleValues[name] = value;
}

std::string CHTLConfigurationProcessor::getRuleValue(const std::string& name) const {
    auto it = ruleValues.find(name);
    return (it != ruleValues.end()) ? it->second : "";
}

bool CHTLConfigurationProcessor::hasRuleValue(const std::string& name) const {
    return ruleValues.find(name) != ruleValues.end();
}

void CHTLConfigurationProcessor::removeRuleValue(const std::string& name) {
    ruleValues.erase(name);
}

bool CHTLConfigurationProcessor::processConfigurations() {
    if (!enableConfigurations) {
        return true;
    }
    
    bool success = true;
    
    for (auto& configuration : configurations) {
        if (!processConfiguration(configuration)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLConfigurationProcessor::processConfiguration(std::shared_ptr<ConfigurationNode> configuration) {
    if (!configuration || !configuration->isActiveConfiguration()) {
        return true;
    }
    
    try {
        bool result = configuration->processConfiguration();
        if (result) {
            processedConfigurations++;
        }
        return result;
    } catch (const std::exception& e) {
        handleConfigurationError("处理配置时发生错误: " + std::string(e.what()));
        return false;
    }
}

bool CHTLConfigurationProcessor::processConfigurationGroup(const std::string& groupName) {
    auto groupConfigurations = getConfigurationsByGroup(groupName);
    bool success = true;
    
    for (auto& configuration : groupConfigurations) {
        if (!processConfiguration(configuration)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLConfigurationProcessor::processConfigurationType(ConfigurationNode::ConfigurationType type) {
    auto typeConfigurations = getConfigurationsByType(type);
    bool success = true;
    
    for (auto& configuration : typeConfigurations) {
        if (!processConfiguration(configuration)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLConfigurationProcessor::validateConfigurations() const {
    if (!enableValidation) {
        return true;
    }
    
    bool success = true;
    
    for (const auto& configuration : configurations) {
        if (!validateConfiguration(configuration)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLConfigurationProcessor::validateConfiguration(std::shared_ptr<ConfigurationNode> configuration) const {
    if (!configuration) {
        return false;
    }
    
    return configuration->validateConfiguration();
}

bool CHTLConfigurationProcessor::validateConfigurationGroup(const std::string& groupName) const {
    auto groupConfigurations = getConfigurationsByGroup(groupName);
    bool success = true;
    
    for (const auto& configuration : groupConfigurations) {
        if (!validateConfiguration(configuration)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLConfigurationProcessor::validateConfigurationType(ConfigurationNode::ConfigurationType type) const {
    auto typeConfigurations = getConfigurationsByType(type);
    bool success = true;
    
    for (const auto& configuration : typeConfigurations) {
        if (!validateConfiguration(configuration)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLConfigurationProcessor::applyConfigurations() {
    if (!enableConfigurations) {
        return true;
    }
    
    bool success = true;
    
    for (auto& configuration : configurations) {
        if (!applyConfiguration(configuration)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLConfigurationProcessor::applyConfiguration(std::shared_ptr<ConfigurationNode> configuration) {
    if (!configuration || !configuration->isProcessedConfiguration()) {
        return false;
    }
    
    try {
        bool result = configuration->applyConfiguration();
        if (result && configuration->isValidConfiguration()) {
            validConfigurations++;
        }
        return result;
    } catch (const std::exception& e) {
        handleConfigurationError("应用配置时发生错误: " + std::string(e.what()));
        return false;
    }
}

bool CHTLConfigurationProcessor::applyConfigurationGroup(const std::string& groupName) {
    auto groupConfigurations = getConfigurationsByGroup(groupName);
    bool success = true;
    
    for (auto& configuration : groupConfigurations) {
        if (!applyConfiguration(configuration)) {
            success = false;
        }
    }
    
    return success;
}

bool CHTLConfigurationProcessor::applyConfigurationType(ConfigurationNode::ConfigurationType type) {
    auto typeConfigurations = getConfigurationsByType(type);
    bool success = true;
    
    for (auto& configuration : typeConfigurations) {
        if (!applyConfiguration(configuration)) {
            success = false;
        }
    }
    
    return success;
}

std::shared_ptr<ConfigurationNode> CHTLConfigurationProcessor::parseConfiguration(const std::string& configurationText) {
    if (configurationText.empty()) {
        return nullptr;
    }
    
    // 解析配置文本
    auto configuration = std::make_shared<ConfigurationNode>("parsed_configuration");
    
    // 简单的配置解析
    std::istringstream iss(configurationText);
    std::string line;
    while (std::getline(iss, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        size_t pos = line.find('=');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            // 清理键值对
            key = std::regex_replace(key, std::regex("^\\s+|\\s+$"), "");
            value = std::regex_replace(value, std::regex("^\\s+|\\s+$"), "");
            
            configuration->setConfigurationParameter(key, value);
        }
    }
    
    return configuration;
}

std::vector<std::shared_ptr<ConfigurationNode>> CHTLConfigurationProcessor::parseConfigurations(const std::string& configurationsText) {
    std::vector<std::shared_ptr<ConfigurationNode>> result;
    
    if (configurationsText.empty()) {
        return result;
    }
    
    // 简单的多配置解析
    std::istringstream iss(configurationsText);
    std::string line;
    std::shared_ptr<ConfigurationNode> currentConfig = nullptr;
    
    while (std::getline(iss, line)) {
        if (line.empty() || line[0] == '#') continue;
        
        if (line.find("configuration") == 0) {
            if (currentConfig) {
                result.push_back(currentConfig);
            }
            currentConfig = std::make_shared<ConfigurationNode>("parsed_configuration");
        } else if (currentConfig) {
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                
                key = std::regex_replace(key, std::regex("^\\s+|\\s+$"), "");
                value = std::regex_replace(value, std::regex("^\\s+|\\s+$"), "");
                
                currentConfig->setConfigurationParameter(key, value);
            }
        }
    }
    
    if (currentConfig) {
        result.push_back(currentConfig);
    }
    
    return result;
}

std::string CHTLConfigurationProcessor::parseConfigurationParameter(const std::string& parameter) {
    if (parameter.empty()) {
        return "";
    }
    
    std::string result = parameter;
    result = std::regex_replace(result, std::regex("^\\s+|\\s+$"), "");
    return result;
}

std::vector<std::string> CHTLConfigurationProcessor::parseConfigurationArray(const std::string& array) {
    std::vector<std::string> result;
    
    if (array.empty()) {
        return result;
    }
    
    // 简单的数组解析
    std::string cleaned = std::regex_replace(array, std::regex("\\[|\\]"), "");
    std::istringstream iss(cleaned);
    std::string item;
    
    while (std::getline(iss, item, ',')) {
        item = std::regex_replace(item, std::regex("^\\s+|\\s+$"), "");
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    
    return result;
}

std::map<std::string, std::string> CHTLConfigurationProcessor::parseConfigurationObject(const std::string& object) {
    std::map<std::string, std::string> result;
    
    if (object.empty()) {
        return result;
    }
    
    // 简单的对象解析
    std::string cleaned = std::regex_replace(object, std::regex("\\{|\\}"), "");
    std::istringstream iss(cleaned);
    std::string line;
    
    while (std::getline(iss, line, ',')) {
        size_t pos = line.find(':');
        if (pos != std::string::npos) {
            std::string key = line.substr(0, pos);
            std::string value = line.substr(pos + 1);
            
            key = std::regex_replace(key, std::regex("^\\s+|\\s+$"), "");
            value = std::regex_replace(value, std::regex("^\\s+|\\s+$"), "");
            
            result[key] = value;
        }
    }
    
    return result;
}

std::string CHTLConfigurationProcessor::generateConfigurations() const {
    std::ostringstream oss;
    
    for (const auto& configuration : configurations) {
        oss << configuration->toConfigurationString() << "\n";
    }
    
    return oss.str();
}

std::string CHTLConfigurationProcessor::generateConfiguration(std::shared_ptr<ConfigurationNode> configuration) const {
    if (!configuration) {
        return "";
    }
    
    return configuration->toConfigurationString();
}

std::string CHTLConfigurationProcessor::generateConfigurationGroup(const std::string& groupName) const {
    auto groupConfigurations = getConfigurationsByGroup(groupName);
    std::ostringstream oss;
    
    for (const auto& configuration : groupConfigurations) {
        oss << configuration->toConfigurationString() << "\n";
    }
    
    return oss.str();
}

std::string CHTLConfigurationProcessor::generateConfigurationType(ConfigurationNode::ConfigurationType type) const {
    auto typeConfigurations = getConfigurationsByType(type);
    std::ostringstream oss;
    
    for (const auto& configuration : typeConfigurations) {
        oss << configuration->toConfigurationString() << "\n";
    }
    
    return oss.str();
}

std::string CHTLConfigurationProcessor::toHTML() const {
    std::ostringstream oss;
    oss << "<!-- Configuration Processor -->\n";
    oss << "<!-- Total Configurations: " << totalConfigurations << " -->\n";
    oss << "<!-- Active Configurations: " << activeConfigurations << " -->\n";
    oss << "<!-- Processed Configurations: " << processedConfigurations << " -->\n";
    oss << "<!-- Valid Configurations: " << validConfigurations << " -->\n";
    
    for (const auto& configuration : configurations) {
        oss << configuration->toHTML() << "\n";
    }
    
    return oss.str();
}

std::string CHTLConfigurationProcessor::toCSS() const {
    std::ostringstream oss;
    oss << "/* Configuration Processor */\n";
    oss << "/* Total Configurations: " << totalConfigurations << " */\n";
    oss << "/* Active Configurations: " << activeConfigurations << " */\n";
    oss << "/* Processed Configurations: " << processedConfigurations << " */\n";
    oss << "/* Valid Configurations: " << validConfigurations << " */\n";
    
    for (const auto& configuration : configurations) {
        oss << configuration->toCSS() << "\n";
    }
    
    return oss.str();
}

std::string CHTLConfigurationProcessor::toJavaScript() const {
    std::ostringstream oss;
    oss << "// Configuration Processor\n";
    oss << "// Total Configurations: " << totalConfigurations << "\n";
    oss << "// Active Configurations: " << activeConfigurations << "\n";
    oss << "// Processed Configurations: " << processedConfigurations << "\n";
    oss << "// Valid Configurations: " << validConfigurations << "\n";
    
    for (const auto& configuration : configurations) {
        oss << configuration->toJavaScript() << "\n";
    }
    
    return oss.str();
}

std::string CHTLConfigurationProcessor::toConfigurationString() const {
    return generateConfigurations();
}

std::string CHTLConfigurationProcessor::formatConfigurations() const {
    return generateConfigurations();
}

std::string CHTLConfigurationProcessor::minifyConfigurations() const {
    std::string result = generateConfigurations();
    
    // 移除多余的空格和换行
    result = std::regex_replace(result, std::regex("\\s+"), " ");
    result = std::regex_replace(result, std::regex("\\s*\\{\\s*"), "{");
    result = std::regex_replace(result, std::regex("\\s*\\}\\s*"), "}");
    result = std::regex_replace(result, std::regex("\\s*;\\s*"), ";");
    result = std::regex_replace(result, std::regex("\\s*:\\s*"), ":");
    
    return result;
}

std::string CHTLConfigurationProcessor::beautifyConfigurations() const {
    std::string result = generateConfigurations();
    
    // 美化格式
    result = std::regex_replace(result, std::regex("\\{"), "{\n  ");
    result = std::regex_replace(result, std::regex(";"), ";\n");
    result = std::regex_replace(result, std::regex("\\}"), "\n}\n");
    
    return result;
}

std::string CHTLConfigurationProcessor::compressConfigurations() const {
    std::string result = generateConfigurations();
    
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

std::string CHTLConfigurationProcessor::decompressConfigurations() const {
    std::string result = generateConfigurations();
    
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

std::string CHTLConfigurationProcessor::encodeConfigurations() const {
    std::string result = generateConfigurations();
    
    // 编码配置
    for (char& c : result) {
        if (std::isalnum(c)) {
            c = static_cast<char>((c - 'a' + 13) % 26 + 'a');
        }
    }
    
    return result;
}

std::string CHTLConfigurationProcessor::decodeConfigurations() const {
    std::string result = generateConfigurations();
    
    // 解码配置
    for (char& c : result) {
        if (std::isalnum(c)) {
            c = static_cast<char>((c - 'a' + 13) % 26 + 'a');
        }
    }
    
    return result;
}

void CHTLConfigurationProcessor::setConfigurationSetting(const std::string& key, const std::string& value) {
    configurationSettings[key] = value;
}

std::string CHTLConfigurationProcessor::getConfigurationSetting(const std::string& key) const {
    auto it = configurationSettings.find(key);
    return (it != configurationSettings.end()) ? it->second : "";
}

bool CHTLConfigurationProcessor::hasConfigurationSetting(const std::string& key) const {
    return configurationSettings.find(key) != configurationSettings.end();
}

void CHTLConfigurationProcessor::removeConfigurationSetting(const std::string& key) {
    configurationSettings.erase(key);
}

void CHTLConfigurationProcessor::setConfigurationFlag(const std::string& key, bool value) {
    configurationFlags[key] = value;
}

bool CHTLConfigurationProcessor::getConfigurationFlag(const std::string& key) const {
    auto it = configurationFlags.find(key);
    return (it != configurationFlags.end()) ? it->second : false;
}

bool CHTLConfigurationProcessor::hasConfigurationFlag(const std::string& key) const {
    return configurationFlags.find(key) != configurationFlags.end();
}

void CHTLConfigurationProcessor::removeConfigurationFlag(const std::string& key) {
    configurationFlags.erase(key);
}

void CHTLConfigurationProcessor::setConfigurationLimit(const std::string& key, int value) {
    configurationLimits[key] = value;
}

int CHTLConfigurationProcessor::getConfigurationLimit(const std::string& key) const {
    auto it = configurationLimits.find(key);
    return (it != configurationLimits.end()) ? it->second : 0;
}

bool CHTLConfigurationProcessor::hasConfigurationLimit(const std::string& key) const {
    return configurationLimits.find(key) != configurationLimits.end();
}

void CHTLConfigurationProcessor::removeConfigurationLimit(const std::string& key) {
    configurationLimits.erase(key);
}

void CHTLConfigurationProcessor::setConfigurationDefault(const std::string& key, const std::string& value) {
    configurationDefaults[key] = value;
}

std::string CHTLConfigurationProcessor::getConfigurationDefault(const std::string& key) const {
    auto it = configurationDefaults.find(key);
    return (it != configurationDefaults.end()) ? it->second : "";
}

bool CHTLConfigurationProcessor::hasConfigurationDefault(const std::string& key) const {
    return configurationDefaults.find(key) != configurationDefaults.end();
}

void CHTLConfigurationProcessor::removeConfigurationDefault(const std::string& key) {
    configurationDefaults.erase(key);
}

void CHTLConfigurationProcessor::setConfigurationPriority(const std::string& name, int priority) {
    configurationPriorities[name] = priority;
}

int CHTLConfigurationProcessor::getConfigurationPriority(const std::string& name) const {
    auto it = configurationPriorities.find(name);
    return (it != configurationPriorities.end()) ? it->second : 0;
}

bool CHTLConfigurationProcessor::hasConfigurationPriority(const std::string& name) const {
    return configurationPriorities.find(name) != configurationPriorities.end();
}

void CHTLConfigurationProcessor::removeConfigurationPriority(const std::string& name) {
    configurationPriorities.erase(name);
}

std::vector<std::shared_ptr<ConfigurationNode>> CHTLConfigurationProcessor::getConfigurationsByPriority(int priority) const {
    auto it = priorityConfigurations.find(priority);
    return (it != priorityConfigurations.end()) ? it->second : std::vector<std::shared_ptr<ConfigurationNode>>();
}

std::vector<int> CHTLConfigurationProcessor::getConfigurationPriorities() const {
    std::vector<int> priorities;
    for (const auto& pair : priorityConfigurations) {
        priorities.push_back(pair.first);
    }
    std::sort(priorities.begin(), priorities.end());
    return priorities;
}

void CHTLConfigurationProcessor::addValidationError(const std::string& error) {
    validationErrors.push_back(error);
}

void CHTLConfigurationProcessor::addValidationWarning(const std::string& warning) {
    validationWarnings.push_back(warning);
}

void CHTLConfigurationProcessor::addConfigurationError(const std::string& error) {
    configurationErrors.push_back(error);
}

void CHTLConfigurationProcessor::clearValidationMessages() {
    validationErrors.clear();
    validationWarnings.clear();
}

void CHTLConfigurationProcessor::clearConfigurationErrors() {
    configurationErrors.clear();
}

void CHTLConfigurationProcessor::reset() {
    resetConfigurations();
    resetConfigurationGroups();
    resetConfigurationTypes();
    resetConfigurationRules();
    resetConfigurationSettings();
    resetConfigurationPriorities();
}

void CHTLConfigurationProcessor::resetConfigurations() {
    configurations.clear();
    configurationMap.clear();
    updateConfigurationStatistics();
}

void CHTLConfigurationProcessor::resetConfigurationGroups() {
    configurationGroups.clear();
}

void CHTLConfigurationProcessor::resetConfigurationTypes() {
    configurationTypeMap.clear();
    configurationTypeNames.clear();
    initializeConfigurationTypes();
}

void CHTLConfigurationProcessor::resetConfigurationRules() {
    ruleFunctions.clear();
    ruleValues.clear();
    initializeRuleFunctions();
}

void CHTLConfigurationProcessor::resetConfigurationSettings() {
    configurationSettings.clear();
    configurationFlags.clear();
    configurationLimits.clear();
    configurationDefaults.clear();
}

void CHTLConfigurationProcessor::resetConfigurationPriorities() {
    configurationPriorities.clear();
    priorityConfigurations.clear();
}

void CHTLConfigurationProcessor::clearConfigurations() {
    configurations.clear();
    configurationMap.clear();
    updateConfigurationStatistics();
}

void CHTLConfigurationProcessor::clearConfigurationGroups() {
    configurationGroups.clear();
}

void CHTLConfigurationProcessor::clearConfigurationTypes() {
    configurationTypeMap.clear();
    configurationTypeNames.clear();
}

void CHTLConfigurationProcessor::clearConfigurationRules() {
    ruleFunctions.clear();
    ruleValues.clear();
}

void CHTLConfigurationProcessor::clearConfigurationSettings() {
    configurationSettings.clear();
    configurationFlags.clear();
    configurationLimits.clear();
    configurationDefaults.clear();
}

void CHTLConfigurationProcessor::clearConfigurationPriorities() {
    configurationPriorities.clear();
    priorityConfigurations.clear();
}

void CHTLConfigurationProcessor::optimizeConfigurations() {
    for (auto& configuration : configurations) {
        optimizeConfiguration(configuration);
    }
}

void CHTLConfigurationProcessor::optimizeConfiguration(std::shared_ptr<ConfigurationNode> configuration) {
    if (!configuration) return;
    
    // 优化配置
    configuration->processConfiguration();
}

void CHTLConfigurationProcessor::optimizeConfigurationGroup(const std::string& groupName) {
    auto groupConfigurations = getConfigurationsByGroup(groupName);
    for (auto& configuration : groupConfigurations) {
        optimizeConfiguration(configuration);
    }
}

void CHTLConfigurationProcessor::optimizeConfigurationType(ConfigurationNode::ConfigurationType type) {
    auto typeConfigurations = getConfigurationsByType(type);
    for (auto& configuration : typeConfigurations) {
        optimizeConfiguration(configuration);
    }
}

void CHTLConfigurationProcessor::optimizeConfigurationRule(const std::string& rule) {
    // 优化配置规则
}

std::string CHTLConfigurationProcessor::getDebugInfo() const {
    std::ostringstream oss;
    oss << "CHTLConfigurationProcessor Debug Info:\n";
    oss << "  Total Configurations: " << totalConfigurations << "\n";
    oss << "  Active Configurations: " << activeConfigurations << "\n";
    oss << "  Processed Configurations: " << processedConfigurations << "\n";
    oss << "  Valid Configurations: " << validConfigurations << "\n";
    oss << "  Error Configurations: " << errorConfigurations << "\n";
    oss << "  Warning Configurations: " << warningConfigurations << "\n";
    oss << "  Debug Mode: " << (debugMode ? "true" : "false") << "\n";
    oss << "  Strict Mode: " << (strictMode ? "true" : "false") << "\n";
    oss << "  Configurations Enabled: " << (enableConfigurations ? "true" : "false") << "\n";
    oss << "  Validation Enabled: " << (enableValidation ? "true" : "false") << "\n";
    oss << "  Warnings Enabled: " << (enableWarnings ? "true" : "false") << "\n";
    oss << "  Errors Enabled: " << (enableErrors ? "true" : "false") << "\n";
    
    if (!validationErrors.empty()) {
        oss << "  Validation Errors: ";
        for (size_t i = 0; i < validationErrors.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << validationErrors[i];
        }
        oss << "\n";
    }
    
    if (!validationWarnings.empty()) {
        oss << "  Validation Warnings: ";
        for (size_t i = 0; i < validationWarnings.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << validationWarnings[i];
        }
        oss << "\n";
    }
    
    if (!configurationErrors.empty()) {
        oss << "  Configuration Errors: ";
        for (size_t i = 0; i < configurationErrors.size(); ++i) {
            if (i > 0) oss << ", ";
            oss << configurationErrors[i];
        }
        oss << "\n";
    }
    
    return oss.str();
}

void CHTLConfigurationProcessor::updateConfigurationStatistics() {
    totalConfigurations = configurations.size();
    activeConfigurations = 0;
    processedConfigurations = 0;
    validConfigurations = 0;
    errorConfigurations = 0;
    warningConfigurations = 0;
    
    for (const auto& configuration : configurations) {
        if (configuration->isActiveConfiguration()) {
            activeConfigurations++;
        }
        if (configuration->isProcessedConfiguration()) {
            processedConfigurations++;
        }
        if (configuration->isValidConfiguration()) {
            validConfigurations++;
        }
        if (!configuration->getConfigurationErrors().empty()) {
            errorConfigurations++;
        }
        if (!configuration->getConfigurationWarnings().empty()) {
            warningConfigurations++;
        }
    }
}

void CHTLConfigurationProcessor::updateConfigurationMappings() {
    // 更新配置映射
    configurationGroups.clear();
    
    for (const auto& configuration : configurations) {
        // 更新配置组映射
        if (!configuration->getConfigurationGroup().empty()) {
            configurationGroups[configuration->getConfigurationGroup()].push_back(configuration);
        }
    }
}

void CHTLConfigurationProcessor::updateConfigurationPriorities() {
    priorityConfigurations.clear();
    
    for (const auto& configuration : configurations) {
        int priority = configuration->getPriority();
        priorityConfigurations[priority].push_back(configuration);
    }
}

void CHTLConfigurationProcessor::handleConfigurationError(const std::string& error) {
    addConfigurationError(error);
    if (enableErrors) {
        // 处理配置错误
    }
}

void CHTLConfigurationProcessor::handleConfigurationWarning(const std::string& warning) {
    addValidationWarning(warning);
    if (enableWarnings) {
        // 处理配置警告
    }
}

// 私有辅助方法实现
bool CHTLConfigurationProcessor::processGlobalConfigurations() {
    return processConfigurationType(ConfigurationNode::ConfigurationType::GLOBAL);
}

bool CHTLConfigurationProcessor::processLocalConfigurations() {
    return processConfigurationType(ConfigurationNode::ConfigurationType::LOCAL);
}

bool CHTLConfigurationProcessor::processModuleConfigurations() {
    return processConfigurationType(ConfigurationNode::ConfigurationType::MODULE);
}

bool CHTLConfigurationProcessor::processTemplateConfigurations() {
    return processConfigurationType(ConfigurationNode::ConfigurationType::TEMPLATE);
}

bool CHTLConfigurationProcessor::processCustomConfigurations() {
    return processConfigurationType(ConfigurationNode::ConfigurationType::CUSTOM);
}

bool CHTLConfigurationProcessor::validateGlobalConfigurations() const {
    return validateConfigurationType(ConfigurationNode::ConfigurationType::GLOBAL);
}

bool CHTLConfigurationProcessor::validateLocalConfigurations() const {
    return validateConfigurationType(ConfigurationNode::ConfigurationType::LOCAL);
}

bool CHTLConfigurationProcessor::validateModuleConfigurations() const {
    return validateConfigurationType(ConfigurationNode::ConfigurationType::MODULE);
}

bool CHTLConfigurationProcessor::validateTemplateConfigurations() const {
    return validateConfigurationType(ConfigurationNode::ConfigurationType::TEMPLATE);
}

bool CHTLConfigurationProcessor::validateCustomConfigurations() const {
    return validateConfigurationType(ConfigurationNode::ConfigurationType::CUSTOM);
}

bool CHTLConfigurationProcessor::applyGlobalConfigurations() {
    return applyConfigurationType(ConfigurationNode::ConfigurationType::GLOBAL);
}

bool CHTLConfigurationProcessor::applyLocalConfigurations() {
    return applyConfigurationType(ConfigurationNode::ConfigurationType::LOCAL);
}

bool CHTLConfigurationProcessor::applyModuleConfigurations() {
    return applyConfigurationType(ConfigurationNode::ConfigurationType::MODULE);
}

bool CHTLConfigurationProcessor::applyTemplateConfigurations() {
    return applyConfigurationType(ConfigurationNode::ConfigurationType::TEMPLATE);
}

bool CHTLConfigurationProcessor::applyCustomConfigurations() {
    return applyConfigurationType(ConfigurationNode::ConfigurationType::CUSTOM);
}

std::shared_ptr<ConfigurationNode> CHTLConfigurationProcessor::parseGlobalConfiguration(const std::string& configurationText) {
    auto configuration = parseConfiguration(configurationText);
    if (configuration) {
        configuration->setConfigurationType(ConfigurationNode::ConfigurationType::GLOBAL);
    }
    return configuration;
}

std::shared_ptr<ConfigurationNode> CHTLConfigurationProcessor::parseLocalConfiguration(const std::string& configurationText) {
    auto configuration = parseConfiguration(configurationText);
    if (configuration) {
        configuration->setConfigurationType(ConfigurationNode::ConfigurationType::LOCAL);
    }
    return configuration;
}

std::shared_ptr<ConfigurationNode> CHTLConfigurationProcessor::parseModuleConfiguration(const std::string& configurationText) {
    auto configuration = parseConfiguration(configurationText);
    if (configuration) {
        configuration->setConfigurationType(ConfigurationNode::ConfigurationType::MODULE);
    }
    return configuration;
}

std::shared_ptr<ConfigurationNode> CHTLConfigurationProcessor::parseTemplateConfiguration(const std::string& configurationText) {
    auto configuration = parseConfiguration(configurationText);
    if (configuration) {
        configuration->setConfigurationType(ConfigurationNode::ConfigurationType::TEMPLATE);
    }
    return configuration;
}

std::shared_ptr<ConfigurationNode> CHTLConfigurationProcessor::parseCustomConfiguration(const std::string& configurationText) {
    auto configuration = parseConfiguration(configurationText);
    if (configuration) {
        configuration->setConfigurationType(ConfigurationNode::ConfigurationType::CUSTOM);
    }
    return configuration;
}

std::string CHTLConfigurationProcessor::generateGlobalConfigurations() const {
    return generateConfigurationType(ConfigurationNode::ConfigurationType::GLOBAL);
}

std::string CHTLConfigurationProcessor::generateLocalConfigurations() const {
    return generateConfigurationType(ConfigurationNode::ConfigurationType::LOCAL);
}

std::string CHTLConfigurationProcessor::generateModuleConfigurations() const {
    return generateConfigurationType(ConfigurationNode::ConfigurationType::MODULE);
}

std::string CHTLConfigurationProcessor::generateTemplateConfigurations() const {
    return generateConfigurationType(ConfigurationNode::ConfigurationType::TEMPLATE);
}

std::string CHTLConfigurationProcessor::generateCustomConfigurations() const {
    return generateConfigurationType(ConfigurationNode::ConfigurationType::CUSTOM);
}

} // namespace CHTL