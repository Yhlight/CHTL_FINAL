#include "CHTL/ConfigurationManager.h"
#include <iostream>
#include <sstream>
#include <algorithm>

namespace CHTL {

ConfigurationManager::ConfigurationManager(std::shared_ptr<CHTLContext> context)
    : m_context(context)
    , m_debugMode(false)
    , m_totalConfigurations(0)
    , m_appliedConfigurationsCount(0)
    , m_failedConfigurationsCount(0)
{
}

ConfigurationManager::~ConfigurationManager() {
}

bool ConfigurationManager::addConfiguration(std::shared_ptr<ConfigurationNode> configurationNode) {
    if (!configurationNode) return false;
    
    m_totalConfigurations++;
    
    if (m_debugMode) {
        std::cout << "[ConfigurationManager] Adding configuration: " << configurationNode->getConfigurationName() << std::endl;
    }
    
    // 检查配置是否已存在
    std::string name = configurationNode->getConfigurationName();
    if (m_configurationMap.find(name) != m_configurationMap.end()) {
        addWarning("Configuration already exists: " + name);
        return false;
    }
    
    // 添加配置
    m_configurations.push_back(configurationNode);
    m_configurationMap[name] = configurationNode;
    
    // 添加到配置组
    if (configurationNode->hasConfigurationGroup()) {
        std::string group = configurationNode->getConfigurationGroup();
        m_configurationGroups[group].push_back(configurationNode);
    }
    
    return true;
}

std::shared_ptr<ConfigurationNode> ConfigurationManager::getConfiguration(const std::string& name) {
    auto it = m_configurationMap.find(name);
    if (it != m_configurationMap.end()) {
        return it->second;
    }
    return nullptr;
}

bool ConfigurationManager::hasConfiguration(const std::string& name) {
    return m_configurationMap.find(name) != m_configurationMap.end();
}

std::string ConfigurationManager::getConfigurationValue(const std::string& name, const std::string& key) {
    auto config = getConfiguration(name);
    if (config) {
        return config->getConfigurationItem(key);
    }
    return "";
}

bool ConfigurationManager::setConfigurationValue(const std::string& name, const std::string& key, const std::string& value) {
    auto config = getConfiguration(name);
    if (config) {
        config->addConfigurationItem(key, value);
        return true;
    }
    return false;
}

const std::vector<std::shared_ptr<ConfigurationNode>>& ConfigurationManager::getAllConfigurations() const {
    return m_configurations;
}

std::vector<std::shared_ptr<ConfigurationNode>> ConfigurationManager::getConfigurationGroup(const std::string& group) {
    auto it = m_configurationGroups.find(group);
    if (it != m_configurationGroups.end()) {
        return it->second;
    }
    return {};
}

std::vector<std::shared_ptr<ConfigurationNode>> ConfigurationManager::getKeywordConfigurations() {
    std::vector<std::shared_ptr<ConfigurationNode>> result;
    for (const auto& config : m_configurations) {
        if (config->getConfigurationType() == ConfigurationType::KEYWORD) {
            result.push_back(config);
        }
    }
    return result;
}

std::vector<std::shared_ptr<ConfigurationNode>> ConfigurationManager::getModuleConfigurations() {
    std::vector<std::shared_ptr<ConfigurationNode>> result;
    for (const auto& config : m_configurations) {
        if (config->getConfigurationType() == ConfigurationType::MODULE) {
            result.push_back(config);
        }
    }
    return result;
}

std::vector<std::shared_ptr<ConfigurationNode>> ConfigurationManager::getCompilerConfigurations() {
    std::vector<std::shared_ptr<ConfigurationNode>> result;
    for (const auto& config : m_configurations) {
        if (config->getConfigurationType() == ConfigurationType::COMPILER) {
            result.push_back(config);
        }
    }
    return result;
}

std::vector<std::shared_ptr<ConfigurationNode>> ConfigurationManager::getOutputConfigurations() {
    std::vector<std::shared_ptr<ConfigurationNode>> result;
    for (const auto& config : m_configurations) {
        if (config->getConfigurationType() == ConfigurationType::OUTPUT) {
            result.push_back(config);
        }
    }
    return result;
}

std::vector<std::shared_ptr<ConfigurationNode>> ConfigurationManager::getDebugConfigurations() {
    std::vector<std::shared_ptr<ConfigurationNode>> result;
    for (const auto& config : m_configurations) {
        if (config->getConfigurationType() == ConfigurationType::DEBUG) {
            result.push_back(config);
        }
    }
    return result;
}

bool ConfigurationManager::applyConfiguration(std::shared_ptr<ConfigurationNode> configurationNode) {
    if (!configurationNode || !configurationNode->isEnabled()) {
        return false;
    }
    
    if (m_debugMode) {
        std::cout << "[ConfigurationManager] Applying configuration: " << configurationNode->getConfigurationName() << std::endl;
    }
    
    // 检查是否已经应用过
    std::string name = configurationNode->getConfigurationName();
    if (m_appliedConfigurations.find(name) != m_appliedConfigurations.end()) {
        addWarning("Configuration already applied: " + name);
        return false;
    }
    
    bool success = false;
    switch (configurationNode->getConfigurationType()) {
        case ConfigurationType::KEYWORD:
            success = applyKeywordConfiguration(configurationNode);
            break;
        case ConfigurationType::MODULE:
            success = applyModuleConfiguration(configurationNode);
            break;
        case ConfigurationType::COMPILER:
            success = applyCompilerConfiguration(configurationNode);
            break;
        case ConfigurationType::OUTPUT:
            success = applyOutputConfiguration(configurationNode);
            break;
        case ConfigurationType::DEBUG:
            success = applyDebugConfiguration(configurationNode);
            break;
        default:
            addError("Unknown configuration type: " + std::to_string(static_cast<int>(configurationNode->getConfigurationType())));
            break;
    }
    
    if (success) {
        m_appliedConfigurationsCount++;
        m_appliedConfigurations.insert(name);
    } else {
        m_failedConfigurationsCount++;
    }
    
    return success;
}

bool ConfigurationManager::applyAllConfigurations() {
    bool allSuccess = true;
    
    for (const auto& config : m_configurations) {
        if (!applyConfiguration(config)) {
            allSuccess = false;
        }
    }
    
    return allSuccess;
}

void ConfigurationManager::clear() {
    m_configurations.clear();
    m_configurationMap.clear();
    m_configurationGroups.clear();
    m_appliedConfigurations.clear();
    m_totalConfigurations = 0;
    m_appliedConfigurationsCount = 0;
    m_failedConfigurationsCount = 0;
}

void ConfigurationManager::setDebugMode(bool debug) {
    m_debugMode = debug;
}

bool ConfigurationManager::isDebugMode() const {
    return m_debugMode;
}

std::string ConfigurationManager::getStatistics() const {
    std::ostringstream oss;
    oss << "Configuration Statistics:\n";
    oss << "  Total configurations: " << m_totalConfigurations << "\n";
    oss << "  Applied: " << m_appliedConfigurationsCount << "\n";
    oss << "  Failed: " << m_failedConfigurationsCount << "\n";
    oss << "  Success rate: " << (m_totalConfigurations > 0 ? (m_appliedConfigurationsCount * 100.0 / m_totalConfigurations) : 0) << "%\n";
    return oss.str();
}

bool ConfigurationManager::applyKeywordConfiguration(std::shared_ptr<ConfigurationNode> configurationNode) {
    if (m_debugMode) {
        std::cout << "[ConfigurationManager] Applying keyword configuration: " << configurationNode->getConfigurationName() << std::endl;
    }
    
    // 应用关键字配置
    const auto& items = configurationNode->getConfigurationItems();
    for (const auto& item : items) {
        if (m_debugMode) {
            std::cout << "[ConfigurationManager] Setting keyword: " << item.first << " = " << item.second << std::endl;
        }
        // 这里应该实际应用关键字配置
        // 例如：更新词法分析器的关键字列表
    }
    
    return true;
}

bool ConfigurationManager::applyModuleConfiguration(std::shared_ptr<ConfigurationNode> configurationNode) {
    if (m_debugMode) {
        std::cout << "[ConfigurationManager] Applying module configuration: " << configurationNode->getConfigurationName() << std::endl;
    }
    
    // 应用模块配置
    const auto& items = configurationNode->getConfigurationItems();
    for (const auto& item : items) {
        if (m_debugMode) {
            std::cout << "[ConfigurationManager] Setting module: " << item.first << " = " << item.second << std::endl;
        }
        // 这里应该实际应用模块配置
        // 例如：更新模块加载器配置
    }
    
    return true;
}

bool ConfigurationManager::applyCompilerConfiguration(std::shared_ptr<ConfigurationNode> configurationNode) {
    if (m_debugMode) {
        std::cout << "[ConfigurationManager] Applying compiler configuration: " << configurationNode->getConfigurationName() << std::endl;
    }
    
    // 应用编译器配置
    const auto& items = configurationNode->getConfigurationItems();
    for (const auto& item : items) {
        if (m_debugMode) {
            std::cout << "[ConfigurationManager] Setting compiler: " << item.first << " = " << item.second << std::endl;
        }
        // 这里应该实际应用编译器配置
        // 例如：更新编译器选项
    }
    
    return true;
}

bool ConfigurationManager::applyOutputConfiguration(std::shared_ptr<ConfigurationNode> configurationNode) {
    if (m_debugMode) {
        std::cout << "[ConfigurationManager] Applying output configuration: " << configurationNode->getConfigurationName() << std::endl;
    }
    
    // 应用输出配置
    const auto& items = configurationNode->getConfigurationItems();
    for (const auto& item : items) {
        if (m_debugMode) {
            std::cout << "[ConfigurationManager] Setting output: " << item.first << " = " << item.second << std::endl;
        }
        // 这里应该实际应用输出配置
        // 例如：更新代码生成器配置
    }
    
    return true;
}

bool ConfigurationManager::applyDebugConfiguration(std::shared_ptr<ConfigurationNode> configurationNode) {
    if (m_debugMode) {
        std::cout << "[ConfigurationManager] Applying debug configuration: " << configurationNode->getConfigurationName() << std::endl;
    }
    
    // 应用调试配置
    const auto& items = configurationNode->getConfigurationItems();
    for (const auto& item : items) {
        if (m_debugMode) {
            std::cout << "[ConfigurationManager] Setting debug: " << item.first << " = " << item.second << std::endl;
        }
        // 这里应该实际应用调试配置
        // 例如：更新调试器配置
    }
    
    return true;
}

void ConfigurationManager::addError(const std::string& message) {
    if (m_context) {
        m_context->addError("ConfigurationManager Error: " + message);
    }
}

void ConfigurationManager::addWarning(const std::string& message) {
    if (m_context) {
        m_context->addWarning("ConfigurationManager Warning: " + message);
    }
}

} // namespace CHTL