#include "ConfigurationManager.h"
#include <iostream>
#include <algorithm>

namespace CHTL {

ConfigurationManager::ConfigurationManager() {
    // 初始化默认配置
    std::map<std::string, ConfigOption> defaultConfig;
    defaultConfig["indent_size"] = {ConfigOptionType::Number, "4", {}};
    defaultConfig["use_tabs"] = {ConfigOptionType::Boolean, "false", {}};
    defaultConfig["line_ending"] = {ConfigOptionType::String, "lf", {}};
    configurations["default"] = defaultConfig;
}

void ConfigurationManager::addConfiguration(const std::string& name, const std::map<std::string, ConfigOption>& options) {
    configurations[name] = options;
    std::cout << "Added configuration: " << name << std::endl;
}

void ConfigurationManager::removeConfiguration(const std::string& name) {
    if (name != "default") { // 不能删除默认配置
        configurations.erase(name);
        std::cout << "Removed configuration: " << name << std::endl;
    } else {
        std::cerr << "Warning: Cannot remove default configuration." << std::endl;
    }
}

bool ConfigurationManager::hasConfiguration(const std::string& name) const {
    return configurations.find(name) != configurations.end();
}

void ConfigurationManager::setOption(const std::string& configName, const std::string& optionName, const ConfigOption& option) {
    if (configurations.find(configName) == configurations.end()) {
        configurations[configName] = {};
    }
    configurations[configName][optionName] = option;
    std::cout << "Set option " << optionName << " in configuration " << configName << std::endl;
}

ConfigOption ConfigurationManager::getOption(const std::string& configName, const std::string& optionName) const {
    auto configIt = configurations.find(configName);
    if (configIt != configurations.end()) {
        auto optionIt = configIt->second.find(optionName);
        if (optionIt != configIt->second.end()) {
            return optionIt->second;
        }
    }
    
    // 返回默认值
    ConfigOption defaultOption;
    defaultOption.type = ConfigOptionType::String;
    defaultOption.value = "";
    return defaultOption;
}

bool ConfigurationManager::hasOption(const std::string& configName, const std::string& optionName) const {
    auto configIt = configurations.find(configName);
    if (configIt != configurations.end()) {
        return configIt->second.find(optionName) != configIt->second.end();
    }
    return false;
}

void ConfigurationManager::addCustomKeyword(const std::string& keyword) {
    if (std::find(customKeywords.begin(), customKeywords.end(), keyword) == customKeywords.end()) {
        customKeywords.push_back(keyword);
        std::cout << "Added custom keyword: " << keyword << std::endl;
    }
}

void ConfigurationManager::removeCustomKeyword(const std::string& keyword) {
    auto it = std::find(customKeywords.begin(), customKeywords.end(), keyword);
    if (it != customKeywords.end()) {
        customKeywords.erase(it);
        std::cout << "Removed custom keyword: " << keyword << std::endl;
    }
}

bool ConfigurationManager::isCustomKeyword(const std::string& keyword) const {
    return std::find(customKeywords.begin(), customKeywords.end(), keyword) != customKeywords.end();
}

void ConfigurationManager::addCustomOriginType(const std::string& originType) {
    if (std::find(customOriginTypes.begin(), customOriginTypes.end(), originType) != customOriginTypes.end()) {
        customOriginTypes.push_back(originType);
        std::cout << "Added custom origin type: " << originType << std::endl;
    }
}

void ConfigurationManager::removeCustomOriginType(const std::string& originType) {
    auto it = std::find(customOriginTypes.begin(), customOriginTypes.end(), originType);
    if (it != customOriginTypes.end()) {
        customOriginTypes.erase(it);
        std::cout << "Removed custom origin type: " << originType << std::endl;
    }
}

bool ConfigurationManager::isCustomOriginType(const std::string& originType) const {
    return std::find(customOriginTypes.begin(), customOriginTypes.end(), originType) != customOriginTypes.end();
}

void ConfigurationManager::importConfiguration(const std::string& configName, const std::string& importPath) {
    importedConfigurations[configName] = importPath;
    std::cout << "Imported configuration " << configName << " from " << importPath << std::endl;
}

std::map<std::string, std::map<std::string, ConfigOption>> ConfigurationManager::getAllConfigurations() const {
    return configurations;
}

std::vector<std::string> ConfigurationManager::getCustomKeywords() const {
    return customKeywords;
}

std::vector<std::string> ConfigurationManager::getCustomOriginTypes() const {
    return customOriginTypes;
}

} // namespace CHTL