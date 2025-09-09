#include "ConfigManager.hpp"
#include <sstream>
#include <regex>
#include <algorithm>

namespace CHTL {

ConfigManager& ConfigManager::getInstance() {
    static ConfigManager instance;
    return instance;
}

void ConfigManager::addConfig(const std::string& name, ConfigType type, 
                             const std::map<std::string, std::string>& properties) {
    if (!validateConfigName(name)) {
        return;
    }
    
    auto configDef = std::make_shared<ConfigDefinition>();
    configDef->name = name;
    configDef->type = type;
    configDef->properties = properties;
    configDef->isActive = true;
    
    configs_[name] = configDef;
}

void ConfigManager::addNameConfig(const std::string& name, 
                                 const std::map<std::string, std::string>& properties) {
    addConfig(name, ConfigType::NAME, properties);
}

void ConfigManager::addOriginTypeConfig(const std::string& name, 
                                       const std::map<std::string, std::string>& properties) {
    addConfig(name, ConfigType::ORIGIN_TYPE, properties);
}

void ConfigManager::addConfigGroup(const std::string& name, 
                                  const std::map<std::string, std::string>& properties) {
    addConfig(name, ConfigType::CONFIG, properties);
}

void ConfigManager::addModuleConfig(const std::string& name, 
                                   const std::vector<std::string>& modules,
                                   const std::map<std::string, std::string>& properties) {
    addConfig(name, ConfigType::MODULE, properties);
    auto configDef = getConfig(name);
    if (configDef) {
        configDef->modules = modules;
    }
}

void ConfigManager::addCustomConfig(const std::string& name, 
                                   const std::map<std::string, std::string>& customSettings) {
    addConfig(name, ConfigType::CUSTOM, {});
    auto configDef = getConfig(name);
    if (configDef) {
        configDef->customSettings = customSettings;
    }
}

std::shared_ptr<ConfigManager::ConfigDefinition> ConfigManager::getConfig(const std::string& name) const {
    auto it = configs_.find(name);
    return (it != configs_.end()) ? it->second : nullptr;
}

bool ConfigManager::hasConfig(const std::string& name) const {
    return configs_.find(name) != configs_.end();
}

void ConfigManager::setConfigProperty(const std::string& name, const std::string& property, 
                                     const std::string& value) {
    auto configDef = getConfig(name);
    if (configDef && validateConfigProperty(property, value)) {
        configDef->properties[property] = value;
    }
}

std::string ConfigManager::getConfigProperty(const std::string& name, const std::string& property) const {
    auto configDef = getConfig(name);
    if (configDef) {
        auto it = configDef->properties.find(property);
        if (it != configDef->properties.end()) {
            return it->second;
        }
    }
    return "";
}

std::map<std::string, std::string> ConfigManager::getConfigProperties(const std::string& name) const {
    auto configDef = getConfig(name);
    return configDef ? configDef->properties : std::map<std::string, std::string>();
}

void ConfigManager::setKeyword(const std::string& name, const std::string& keyword, 
                              const std::string& value) {
    auto configDef = getConfig(name);
    if (configDef && validateKeyword(keyword, value)) {
        configDef->keywords[keyword] = value;
    }
}

std::string ConfigManager::getKeyword(const std::string& name, const std::string& keyword) const {
    auto configDef = getConfig(name);
    if (configDef) {
        auto it = configDef->keywords.find(keyword);
        if (it != configDef->keywords.end()) {
            return it->second;
        }
    }
    return "";
}

std::map<std::string, std::string> ConfigManager::getKeywords(const std::string& name) const {
    auto configDef = getConfig(name);
    return configDef ? configDef->keywords : std::map<std::string, std::string>();
}

void ConfigManager::addModule(const std::string& name, const std::string& module) {
    auto configDef = getConfig(name);
    if (configDef && validateModule(module)) {
        configDef->modules.push_back(module);
    }
}

std::vector<std::string> ConfigManager::getModules(const std::string& name) const {
    auto configDef = getConfig(name);
    return configDef ? configDef->modules : std::vector<std::string>();
}

void ConfigManager::setCustomSetting(const std::string& name, const std::string& setting, 
                                    const std::string& value) {
    auto configDef = getConfig(name);
    if (configDef && validateCustomSetting(setting, value)) {
        configDef->customSettings[setting] = value;
    }
}

std::string ConfigManager::getCustomSetting(const std::string& name, const std::string& setting) const {
    auto configDef = getConfig(name);
    if (configDef) {
        auto it = configDef->customSettings.find(setting);
        if (it != configDef->customSettings.end()) {
            return it->second;
        }
    }
    return "";
}

std::map<std::string, std::string> ConfigManager::getCustomSettings(const std::string& name) const {
    auto configDef = getConfig(name);
    return configDef ? configDef->customSettings : std::map<std::string, std::string>();
}

void ConfigManager::activateConfig(const std::string& name) {
    auto configDef = getConfig(name);
    if (configDef) {
        configDef->isActive = true;
    }
}

void ConfigManager::deactivateConfig(const std::string& name) {
    auto configDef = getConfig(name);
    if (configDef) {
        configDef->isActive = false;
    }
}

bool ConfigManager::isConfigActive(const std::string& name) const {
    auto configDef = getConfig(name);
    return configDef && configDef->isActive;
}

std::vector<std::string> ConfigManager::getAllConfigNames() const {
    std::vector<std::string> names;
    for (const auto& [name, configDef] : configs_) {
        names.push_back(name);
    }
    return names;
}

std::vector<std::string> ConfigManager::getConfigNamesByType(ConfigType type) const {
    std::vector<std::string> names;
    for (const auto& [name, configDef] : configs_) {
        if (configDef->type == type) {
            names.push_back(name);
        }
    }
    return names;
}

void ConfigManager::clearConfig(const std::string& name) {
    auto configDef = getConfig(name);
    if (configDef) {
        configDef->properties.clear();
        configDef->keywords.clear();
        configDef->modules.clear();
        configDef->customSettings.clear();
    }
}

void ConfigManager::clear() {
    configs_.clear();
    configDependencies_.clear();
    defaultConfig_ = "";
}

void ConfigManager::removeConfig(const std::string& name) {
    configs_.erase(name);
    configDependencies_.erase(name);
    
    if (defaultConfig_ == name) {
        defaultConfig_ = "";
    }
}

void ConfigManager::importConfig(const std::string& name, const std::string& sourceName) {
    auto sourceConfig = getConfig(sourceName);
    if (sourceConfig) {
        auto configDef = std::make_shared<ConfigDefinition>(*sourceConfig);
        configDef->name = name;
        configs_[name] = configDef;
    }
}

std::map<std::string, std::string> ConfigManager::exportConfig(const std::string& name) const {
    auto configDef = getConfig(name);
    if (configDef) {
        std::map<std::string, std::string> exportData;
        exportData["name"] = configDef->name;
        exportData["type"] = std::to_string(static_cast<int>(configDef->type));
        exportData["active"] = configDef->isActive ? "true" : "false";
        
        // 导出属性
        for (const auto& [key, value] : configDef->properties) {
            exportData["property_" + key] = value;
        }
        
        // 导出关键字
        for (const auto& [key, value] : configDef->keywords) {
            exportData["keyword_" + key] = value;
        }
        
        // 导出模块
        for (size_t i = 0; i < configDef->modules.size(); ++i) {
            exportData["module_" + std::to_string(i)] = configDef->modules[i];
        }
        
        // 导出自定义设置
        for (const auto& [key, value] : configDef->customSettings) {
            exportData["custom_" + key] = value;
        }
        
        return exportData;
    }
    
    return {};
}

void ConfigManager::mergeConfig(const std::string& targetName, const std::string& sourceName) {
    auto targetConfig = getConfig(targetName);
    auto sourceConfig = getConfig(sourceName);
    
    if (!targetConfig || !sourceConfig) {
        return;
    }
    
    // 合并属性
    for (const auto& [key, value] : sourceConfig->properties) {
        targetConfig->properties[key] = value;
    }
    
    // 合并关键字
    for (const auto& [key, value] : sourceConfig->keywords) {
        targetConfig->keywords[key] = value;
    }
    
    // 合并模块
    for (const auto& module : sourceConfig->modules) {
        if (std::find(targetConfig->modules.begin(), targetConfig->modules.end(), module) == targetConfig->modules.end()) {
            targetConfig->modules.push_back(module);
        }
    }
    
    // 合并自定义设置
    for (const auto& [key, value] : sourceConfig->customSettings) {
        targetConfig->customSettings[key] = value;
    }
}

bool ConfigManager::validateConfig(const std::string& name) const {
    auto configDef = getConfig(name);
    if (!configDef) {
        return false;
    }
    
    // 验证属性
    for (const auto& [property, value] : configDef->properties) {
        if (!validateConfigProperty(property, value)) {
            return false;
        }
    }
    
    // 验证关键字
    for (const auto& [keyword, value] : configDef->keywords) {
        if (!validateKeyword(keyword, value)) {
            return false;
        }
    }
    
    // 验证模块
    for (const auto& module : configDef->modules) {
        if (!validateModule(module)) {
            return false;
        }
    }
    
    // 验证自定义设置
    for (const auto& [setting, value] : configDef->customSettings) {
        if (!validateCustomSetting(setting, value)) {
            return false;
        }
    }
    
    return true;
}

std::vector<std::string> ConfigManager::getConfigValidationErrors(const std::string& name) const {
    std::vector<std::string> errors;
    auto configDef = getConfig(name);
    
    if (!configDef) {
        errors.push_back("Configuration not found: " + name);
        return errors;
    }
    
    // 验证属性
    for (const auto& [property, value] : configDef->properties) {
        if (!validateConfigProperty(property, value)) {
            errors.push_back("Invalid property: " + property + " = " + value);
        }
    }
    
    // 验证关键字
    for (const auto& [keyword, value] : configDef->keywords) {
        if (!validateKeyword(keyword, value)) {
            errors.push_back("Invalid keyword: " + keyword + " = " + value);
        }
    }
    
    // 验证模块
    for (const auto& module : configDef->modules) {
        if (!validateModule(module)) {
            errors.push_back("Invalid module: " + module);
        }
    }
    
    // 验证自定义设置
    for (const auto& [setting, value] : configDef->customSettings) {
        if (!validateCustomSetting(setting, value)) {
            errors.push_back("Invalid custom setting: " + setting + " = " + value);
        }
    }
    
    return errors;
}

void ConfigManager::setDefaultConfig(const std::string& name) {
    if (hasConfig(name)) {
        defaultConfig_ = name;
    }
}

std::string ConfigManager::getDefaultConfig() const {
    return defaultConfig_;
}

void ConfigManager::resetToDefault() {
    if (!defaultConfig_.empty()) {
        auto defaultConfigDef = getConfig(defaultConfig_);
        if (defaultConfigDef) {
            defaultConfigDef->isActive = true;
        }
    }
}

std::vector<std::string> ConfigManager::getConfigHierarchy(const std::string& name) const {
    return parseConfigHierarchy(name);
}

void ConfigManager::addConfigDependency(const std::string& name, const std::string& dependency) {
    if (hasConfig(name) && hasConfig(dependency)) {
        configDependencies_[name].push_back(dependency);
    }
}

std::vector<std::string> ConfigManager::getConfigDependencies(const std::string& name) const {
    auto it = configDependencies_.find(name);
    return (it != configDependencies_.end()) ? it->second : std::vector<std::string>();
}

bool ConfigManager::checkConfigDependencies(const std::string& name) const {
    auto dependencies = getConfigDependencies(name);
    
    for (const auto& dependency : dependencies) {
        if (!hasConfig(dependency) || !isConfigActive(dependency)) {
            return false;
        }
        
        if (checkCircularDependency(name, dependency)) {
            return false;
        }
    }
    
    return true;
}

bool ConfigManager::validateConfigName(const std::string& name) const {
    if (name.empty()) {
        return false;
    }
    
    std::regex nameRegex(R"([a-zA-Z_][a-zA-Z0-9_]*)");
    return std::regex_match(name, nameRegex);
}

bool ConfigManager::validateConfigProperty(const std::string& property, const std::string& value) const {
    if (property.empty() || value.empty()) {
        return false;
    }
    
    // 根据属性类型进行验证
    if (property == "INDEX_INITIAL_COUNT") {
        return std::regex_match(value, std::regex(R"(\d+)"));
    } else if (property == "DEBUG_MODE") {
        return value == "true" || value == "false";
    } else if (property == "DISABLE_NAME_GROUP") {
        return value == "true" || value == "false";
    } else if (property == "OPTION_COUNT") {
        return std::regex_match(value, std::regex(R"(\d+)"));
    } else if (property == "DISABLE_STYLE_AUTO_ADD_CLASS") {
        return value == "true" || value == "false";
    } else if (property == "DISABLE_STYLE_AUTO_ADD_ID") {
        return value == "true" || value == "false";
    } else if (property == "DISABLE_DEFAULT_NAMESPACE") {
        return value == "true" || value == "false";
    }
    
    return true;
}

bool ConfigManager::validateKeyword(const std::string& keyword, const std::string& value) const {
    if (keyword.empty() || value.empty()) {
        return false;
    }
    
    std::regex keywordRegex(R"([a-zA-Z_][a-zA-Z0-9_]*)");
    return std::regex_match(keyword, keywordRegex) && std::regex_match(value, keywordRegex);
}

bool ConfigManager::validateModule(const std::string& module) const {
    if (module.empty()) {
        return false;
    }
    
    std::regex moduleRegex(R"([a-zA-Z_][a-zA-Z0-9_.]*)");
    return std::regex_match(module, moduleRegex);
}

bool ConfigManager::validateCustomSetting(const std::string& setting, const std::string& value) const {
    if (setting.empty()) {
        return false;
    }
    
    std::regex settingRegex(R"([a-zA-Z_][a-zA-Z0-9_]*)");
    return std::regex_match(setting, settingRegex);
}

void ConfigManager::createDefaultConfigs() {
    // 创建默认配置
    addConfig("default", ConfigType::CONFIG, {
        {"INDEX_INITIAL_COUNT", "0"},
        {"DEBUG_MODE", "false"},
        {"DISABLE_NAME_GROUP", "false"},
        {"OPTION_COUNT", "10"},
        {"DISABLE_STYLE_AUTO_ADD_CLASS", "false"},
        {"DISABLE_STYLE_AUTO_ADD_ID", "false"},
        {"DISABLE_DEFAULT_NAMESPACE", "false"}
    });
    
    setDefaultConfig("default");
}

std::vector<std::string> ConfigManager::parseConfigHierarchy(const std::string& name) const {
    std::vector<std::string> hierarchy;
    std::string current = name;
    
    while (!current.empty()) {
        hierarchy.push_back(current);
        auto configDef = getConfig(current);
        if (configDef && !configDef->properties.empty()) {
            auto it = configDef->properties.find("parent");
            if (it != configDef->properties.end()) {
                current = it->second;
            } else {
                break;
            }
        } else {
            break;
        }
    }
    
    return hierarchy;
}

bool ConfigManager::checkCircularDependency(const std::string& name, const std::string& dependency) const {
    if (name == dependency) {
        return true;
    }
    
    auto dependencies = getConfigDependencies(dependency);
    for (const auto& dep : dependencies) {
        if (checkCircularDependency(name, dep)) {
            return true;
        }
    }
    
    return false;
}

} // namespace CHTL