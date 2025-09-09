#include "CHTL/CHTLConfig.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace CHTL {

// ConfigItem 实现
ConfigItem::ConfigItem(const std::string& key, const std::string& value, 
                       const std::string& type, int line, int column)
    : m_key(key), m_value(value), m_type(type), m_line(line), m_column(column) {
}

bool ConfigItem::isBoolean() const {
    std::string lowerValue = m_value;
    std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
    return lowerValue == "true" || lowerValue == "false" || lowerValue == "1" || lowerValue == "0";
}

bool ConfigItem::isNumber() const {
    return !m_value.empty() && std::all_of(m_value.begin(), m_value.end(), ::isdigit);
}

bool ConfigItem::isString() const {
    return !m_value.empty() && (m_value[0] == '"' || m_value[0] == '\'');
}

bool ConfigItem::isArray() const {
    return !m_value.empty() && m_value[0] == '[' && m_value[m_value.length() - 1] == ']';
}

bool ConfigItem::getBooleanValue() const {
    std::string lowerValue = m_value;
    std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
    return lowerValue == "true" || lowerValue == "1";
}

int ConfigItem::getNumberValue() const {
    try {
        return std::stoi(m_value);
    } catch (const std::exception&) {
        return 0;
    }
}

std::string ConfigItem::getStringValue() const {
    if (m_value.length() >= 2 && 
        ((m_value[0] == '"' && m_value[m_value.length() - 1] == '"') ||
         (m_value[0] == '\'' && m_value[m_value.length() - 1] == '\''))) {
        return m_value.substr(1, m_value.length() - 2);
    }
    return m_value;
}

std::vector<std::string> ConfigItem::getArrayValue() const {
    std::vector<std::string> result;
    
    if (!isArray()) {
        return result;
    }
    
    std::string content = m_value.substr(1, m_value.length() - 2);
    std::stringstream ss(content);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        // 去除前后空格
        item.erase(0, item.find_first_not_of(" \t"));
        item.erase(item.find_last_not_of(" \t") + 1);
        
        // 去除引号
        if (item.length() >= 2 && 
            ((item[0] == '"' && item[item.length() - 1] == '"') ||
             (item[0] == '\'' && item[item.length() - 1] == '\''))) {
            item = item.substr(1, item.length() - 2);
        }
        
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    
    return result;
}

// ConfigGroup 实现
ConfigGroup::ConfigGroup(const std::string& name, ConfigType type)
    : m_name(name), m_type(type) {
}

void ConfigGroup::addConfigItem(std::shared_ptr<ConfigItem> item) {
    if (item) {
        m_configItems[item->getKey()] = item;
    }
}

void ConfigGroup::addConfigItem(const std::string& key, const std::string& value, 
                               const std::string& type) {
    auto item = std::make_shared<ConfigItem>(key, value, type);
    addConfigItem(item);
}

std::shared_ptr<ConfigItem> ConfigGroup::getConfigItem(const std::string& key) const {
    auto it = m_configItems.find(key);
    return (it != m_configItems.end()) ? it->second : nullptr;
}

std::string ConfigGroup::getConfigValue(const std::string& key) const {
    auto item = getConfigItem(key);
    return item ? item->getValue() : "";
}

bool ConfigGroup::hasConfigItem(const std::string& key) const {
    return m_configItems.find(key) != m_configItems.end();
}

const std::unordered_map<std::string, std::shared_ptr<ConfigItem>>& ConfigGroup::getConfigItems() const {
    return m_configItems;
}

bool ConfigGroup::validateConfig() const {
    for (const auto& [key, item] : m_configItems) {
        try {
            validateConfigItem(item);
        } catch (const std::exception&) {
            return false;
        }
    }
    return true;
}

void ConfigGroup::validateConfigItem(std::shared_ptr<ConfigItem> item) const {
    if (!item) {
        throw std::invalid_argument("Config item is null");
    }
    
    // 验证配置项的值
    if (item->getKey().empty()) {
        throw std::invalid_argument("Config item key cannot be empty");
    }
    
    if (item->getValue().empty()) {
        throw std::invalid_argument("Config item value cannot be empty");
    }
    
    // 根据类型验证值
    if (item->getType() == "boolean" && !item->isBoolean()) {
        throw std::invalid_argument("Config item value is not a valid boolean");
    }
    
    if (item->getType() == "number" && !item->isNumber()) {
        throw std::invalid_argument("Config item value is not a valid number");
    }
}

// ConfigManager 实现
ConfigManager::ConfigManager(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
    createDefaultConfigGroup();
}

void ConfigManager::addConfigGroup(std::shared_ptr<ConfigGroup> configGroup) {
    if (configGroup) {
        if (configGroup->isNamed()) {
            addConfigGroup(configGroup->getName(), configGroup);
        } else {
            m_defaultConfigGroup = configGroup;
        }
    }
}

void ConfigManager::addConfigGroup(const std::string& name, std::shared_ptr<ConfigGroup> configGroup) {
    if (configGroup) {
        m_configGroups[name] = configGroup;
    }
}

std::shared_ptr<ConfigGroup> ConfigManager::getConfigGroup(const std::string& name) const {
    auto it = m_configGroups.find(name);
    return (it != m_configGroups.end()) ? it->second : nullptr;
}

std::shared_ptr<ConfigGroup> ConfigManager::getDefaultConfigGroup() const {
    return m_defaultConfigGroup;
}

void ConfigManager::setConfigValue(const std::string& key, const std::string& value, 
                                  const std::string& groupName) {
    std::shared_ptr<ConfigGroup> configGroup;
    
    if (groupName.empty()) {
        configGroup = getDefaultConfigGroup();
    } else {
        configGroup = getConfigGroup(groupName);
    }
    
    if (configGroup) {
        configGroup->addConfigItem(key, value);
    } else {
        m_context->addWarning("Config group not found: " + groupName);
    }
}

std::string ConfigManager::getConfigValue(const std::string& key, const std::string& groupName) const {
    std::shared_ptr<ConfigGroup> configGroup;
    
    if (groupName.empty()) {
        configGroup = getDefaultConfigGroup();
    } else {
        configGroup = getConfigGroup(groupName);
    }
    
    if (configGroup) {
        return configGroup->getConfigValue(key);
    }
    
    return "";
}

bool ConfigManager::hasConfigValue(const std::string& key, const std::string& groupName) const {
    std::shared_ptr<ConfigGroup> configGroup;
    
    if (groupName.empty()) {
        configGroup = getDefaultConfigGroup();
    } else {
        configGroup = getConfigGroup(groupName);
    }
    
    if (configGroup) {
        return configGroup->hasConfigItem(key);
    }
    
    return false;
}

void ConfigManager::applyConfig(std::shared_ptr<BaseNode> root) {
    if (!root) {
        return;
    }
    
    // 应用默认配置
    if (m_defaultConfigGroup) {
        applyConfigGroup(m_defaultConfigGroup);
    }
    
    // 应用命名配置
    for (const auto& [name, configGroup] : m_configGroups) {
        applyConfigGroup(configGroup);
    }
}

void ConfigManager::applyConfigGroup(std::shared_ptr<ConfigGroup> configGroup) {
    if (!configGroup) {
        return;
    }
    
    // 应用基础配置
    applyBasicConfig(configGroup);
    
    // 应用名称配置
    applyNameConfig(configGroup);
    
    // 应用原始类型配置
    applyOriginTypeConfig(configGroup);
}

bool ConfigManager::validateAllConfigs() const {
    // 验证默认配置
    if (m_defaultConfigGroup && !validateConfigGroup(m_defaultConfigGroup)) {
        return false;
    }
    
    // 验证命名配置
    for (const auto& [name, configGroup] : m_configGroups) {
        if (!validateConfigGroup(configGroup)) {
            return false;
        }
    }
    
    return true;
}

bool ConfigManager::validateConfigGroup(std::shared_ptr<ConfigGroup> configGroup) const {
    if (!configGroup) {
        return false;
    }
    
    return configGroup->validateConfig();
}

void ConfigManager::clear() {
    m_configGroups.clear();
    m_defaultConfigGroup = nullptr;
}

void ConfigManager::removeConfigGroup(const std::string& name) {
    m_configGroups.erase(name);
}

void ConfigManager::createDefaultConfigGroup() {
    m_defaultConfigGroup = std::make_shared<ConfigGroup>("", ConfigType::BASIC);
    
    // 添加默认配置项
    m_defaultConfigGroup->addConfigItem("INDEX_INITIAL_COUNT", "0", "number");
    m_defaultConfigGroup->addConfigItem("DEBUG_MODE", "false", "boolean");
    m_defaultConfigGroup->addConfigItem("DISABLE_STYLE_AUTO_ADD_CLASS", "false", "boolean");
    m_defaultConfigGroup->addConfigItem("DISABLE_STYLE_AUTO_ADD_ID", "false", "boolean");
    m_defaultConfigGroup->addConfigItem("DISABLE_SCRIPT_AUTO_ADD_CLASS", "true", "boolean");
    m_defaultConfigGroup->addConfigItem("DISABLE_SCRIPT_AUTO_ADD_ID", "true", "boolean");
    m_defaultConfigGroup->addConfigItem("DISABLE_DEFAULT_NAMESPACE", "false", "boolean");
    m_defaultConfigGroup->addConfigItem("DISABLE_CUSTOM_ORIGIN_TYPE", "false", "boolean");
}

void ConfigManager::applyBasicConfig(std::shared_ptr<ConfigGroup> configGroup) {
    // 应用基础配置
    for (const auto& [key, item] : configGroup->getConfigItems()) {
        if (key == "DEBUG_MODE") {
            m_context->setDebugMode(item->getBooleanValue());
        } else if (key == "INDEX_INITIAL_COUNT") {
            // 应用索引初始计数配置
            // 这里需要根据具体的实现来应用
        }
    }
}

void ConfigManager::applyNameConfig(std::shared_ptr<ConfigGroup> configGroup) {
    // 应用名称配置
    // 这里需要根据具体的实现来应用关键字名称配置
}

void ConfigManager::applyOriginTypeConfig(std::shared_ptr<ConfigGroup> configGroup) {
    // 应用原始类型配置
    // 这里需要根据具体的实现来应用自定义原始嵌入类型配置
}

// ConfigParser 实现
ConfigParser::ConfigParser(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

std::shared_ptr<ConfigGroup> ConfigParser::parseConfigGroup(std::shared_ptr<ConfigNode> node) {
    if (!node) {
        return nullptr;
    }
    
    std::string name = node->getName();
    ConfigType type = node->isNamed() ? ConfigType::NAMED : ConfigType::BASIC;
    
    auto configGroup = std::make_shared<ConfigGroup>(name, type);
    
    // 解析配置项
    for (const auto& [key, value] : node->getAttributes()) {
        auto item = parseConfigItem(key, value, node->getLine(), node->getColumn());
        configGroup->addConfigItem(item);
    }
    
    return configGroup;
}

std::vector<std::shared_ptr<ConfigGroup>> ConfigParser::parseConfigGroups(std::shared_ptr<BaseNode> root) {
    std::vector<std::shared_ptr<ConfigGroup>> configGroups;
    
    if (!root) {
        return configGroups;
    }
    
    // 递归查找配置节点
    std::function<void(std::shared_ptr<BaseNode>)> findConfigs = 
        [&](std::shared_ptr<BaseNode> node) {
            if (node->getType() == NodeType::CONFIGURATION) {
                auto configNode = std::static_pointer_cast<ConfigNode>(node);
                auto configGroup = parseConfigGroup(configNode);
                if (configGroup) {
                    configGroups.push_back(configGroup);
                }
            }
            
            for (const auto& child : node->getChildren()) {
                findConfigs(child);
            }
        };
    
    findConfigs(root);
    return configGroups;
}

std::shared_ptr<ConfigItem> ConfigParser::parseConfigItem(const std::string& key, 
                                                         const std::string& value, 
                                                         int line, int column) {
    std::string parsedValue = parseConfigValue(value);
    std::string type = parseConfigType(value);
    
    return std::make_shared<ConfigItem>(key, parsedValue, type, line, column);
}

std::string ConfigParser::parseConfigValue(const std::string& value) const {
    // 解析配置值
    if (value.empty()) {
        return value;
    }
    
    // 去除前后空格
    std::string trimmed = value;
    trimmed.erase(0, trimmed.find_first_not_of(" \t"));
    trimmed.erase(trimmed.find_last_not_of(" \t") + 1);
    
    return trimmed;
}

std::string ConfigParser::parseConfigType(const std::string& value) const {
    // 根据值推断类型
    if (value.empty()) {
        return "string";
    }
    
    // 检查布尔值
    std::string lowerValue = value;
    std::transform(lowerValue.begin(), lowerValue.end(), lowerValue.begin(), ::tolower);
    if (lowerValue == "true" || lowerValue == "false") {
        return "boolean";
    }
    
    // 检查数字
    if (std::all_of(value.begin(), value.end(), ::isdigit)) {
        return "number";
    }
    
    // 检查数组
    if (value[0] == '[' && value[value.length() - 1] == ']') {
        return "array";
    }
    
    // 检查字符串
    if ((value[0] == '"' && value[value.length() - 1] == '"') ||
        (value[0] == '\'' && value[value.length() - 1] == '\'')) {
        return "string";
    }
    
    return "string";
}

std::vector<std::string> ConfigParser::parseArrayValue(const std::string& value) const {
    std::vector<std::string> result;
    
    if (value.empty() || value[0] != '[' || value[value.length() - 1] != ']') {
        return result;
    }
    
    std::string content = value.substr(1, value.length() - 2);
    std::stringstream ss(content);
    std::string item;
    
    while (std::getline(ss, item, ',')) {
        // 去除前后空格
        item.erase(0, item.find_first_not_of(" \t"));
        item.erase(item.find_last_not_of(" \t") + 1);
        
        if (!item.empty()) {
            result.push_back(item);
        }
    }
    
    return result;
}

// ConfigGenerator 实现
ConfigGenerator::ConfigGenerator(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
}

std::string ConfigGenerator::generateConfig(std::shared_ptr<ConfigGroup> configGroup) {
    if (!configGroup) {
        return "";
    }
    
    std::stringstream ss;
    
    // 生成配置组头部
    ss << generateConfigGroupHeader(configGroup);
    
    // 生成配置项
    for (const auto& [key, item] : configGroup->getConfigItems()) {
        ss << generateConfigItem(item);
    }
    
    return ss.str();
}

std::string ConfigGenerator::generateAllConfigs(const std::vector<std::shared_ptr<ConfigGroup>>& configGroups) {
    std::stringstream ss;
    
    for (const auto& configGroup : configGroups) {
        if (configGroup) {
            ss << generateConfig(configGroup) << "\n";
        }
    }
    
    return ss.str();
}

std::string ConfigGenerator::generateConfigDocumentation(std::shared_ptr<ConfigGroup> configGroup) {
    if (!configGroup) {
        return "";
    }
    
    std::stringstream ss;
    
    ss << "Configuration Group: " << (configGroup->isNamed() ? configGroup->getName() : "Default") << "\n";
    ss << "Type: " << (configGroup->getType() == ConfigType::BASIC ? "Basic" : "Named") << "\n";
    ss << "Items:\n";
    
    for (const auto& [key, item] : configGroup->getConfigItems()) {
        ss << "  " << key << " = " << item->getValue() << " (" << item->getType() << ")\n";
    }
    
    return ss.str();
}

std::string ConfigGenerator::generateConfigItem(std::shared_ptr<ConfigItem> item) const {
    if (!item) {
        return "";
    }
    
    std::stringstream ss;
    ss << "  " << item->getKey() << " = " << item->getValue() << ";\n";
    return ss.str();
}

std::string ConfigGenerator::generateConfigGroupHeader(std::shared_ptr<ConfigGroup> configGroup) const {
    std::stringstream ss;
    
    if (configGroup->isNamed()) {
        ss << "[Configuration] @Config " << configGroup->getName() << "\n";
    } else {
        ss << "[Configuration]\n";
    }
    
    ss << "{\n";
    return ss.str();
}

// ConfigProcessor 实现
ConfigProcessor::ConfigProcessor(std::shared_ptr<CHTLContext> context)
    : m_context(context) {
    m_configManager = std::make_unique<ConfigManager>(context);
    m_parser = std::make_unique<ConfigParser>(context);
    m_generator = std::make_unique<ConfigGenerator>(context);
}

void ConfigProcessor::processConfigs(std::shared_ptr<BaseNode> root) {
    if (!root) {
        return;
    }
    
    // 解析配置
    auto configGroups = m_parser->parseConfigGroups(root);
    for (const auto& configGroup : configGroups) {
        m_configManager->addConfigGroup(configGroup);
    }
    
    // 应用配置
    applyConfigs(root);
}

void ConfigProcessor::applyConfigs(std::shared_ptr<BaseNode> root) {
    if (!root) {
        return;
    }
    
    // 应用配置
    m_configManager->applyConfig(root);
}

bool ConfigProcessor::validateConfigs(std::shared_ptr<BaseNode> root) {
    if (!root) {
        return true;
    }
    
    // 解析配置
    auto configGroups = m_parser->parseConfigGroups(root);
    for (const auto& configGroup : configGroups) {
        m_configManager->addConfigGroup(configGroup);
    }
    
    // 验证配置
    return m_configManager->validateAllConfigs();
}

void ConfigProcessor::processConfigGroup(std::shared_ptr<ConfigGroup> configGroup) {
    if (!configGroup) {
        return;
    }
    
    // 处理配置组
    m_configManager->addConfigGroup(configGroup);
}

void ConfigProcessor::processNameConfig(std::shared_ptr<ConfigGroup> configGroup) {
    if (!configGroup) {
        return;
    }
    
    // 处理名称配置
    // 这里需要根据具体的实现来处理关键字名称配置
}

void ConfigProcessor::processOriginTypeConfig(std::shared_ptr<ConfigGroup> configGroup) {
    if (!configGroup) {
        return;
    }
    
    // 处理原始类型配置
    // 这里需要根据具体的实现来处理自定义原始嵌入类型配置
}

} // namespace CHTL