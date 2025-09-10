#include "chtl/config_system.h"
#include <sstream>
#include <algorithm>
#include <regex>

namespace chtl {
namespace config_system {

// ConfigParser 实现
ConfigParser::ConfigParser(const std::string& input) : input_(input), position_(0) {}

std::vector<std::shared_ptr<ConfigGroup>> ConfigParser::parseConfigurations() {
    std::vector<std::shared_ptr<ConfigGroup>> configs;
    
    while (position_ < input_.length()) {
        skipWhitespace();
        if (position_ >= input_.length()) break;
        
        if (input_.substr(position_, 13) == "[Configuration]") {
            position_ += 13;
            skipWhitespace();
            
            auto config = parseConfigGroup();
            if (config) {
                configs.push_back(config);
            }
        } else {
            advance();
        }
    }
    
    return configs;
}

std::vector<std::shared_ptr<NameConfig>> ConfigParser::parseNameConfigs() {
    std::vector<std::shared_ptr<NameConfig>> configs;
    
    while (position_ < input_.length()) {
        skipWhitespace();
        if (position_ >= input_.length()) break;
        
        if (input_.substr(position_, 5) == "[Name]") {
            position_ += 5;
            skipWhitespace();
            
            auto config = parseNameConfig();
            if (config) {
                configs.push_back(config);
            }
        } else {
            advance();
        }
    }
    
    return configs;
}

std::vector<std::shared_ptr<NamedConfigGroup>> ConfigParser::parseNamedConfigGroups() {
    std::vector<std::shared_ptr<NamedConfigGroup>> configs;
    
    while (position_ < input_.length()) {
        skipWhitespace();
        if (position_ >= input_.length()) break;
        
        if (input_.substr(position_, 6) == "[Named]") {
            position_ += 6;
            skipWhitespace();
            
            auto config = parseNamedConfigGroup();
            if (config) {
                configs.push_back(config);
            }
        } else {
            advance();
        }
    }
    
    return configs;
}

std::vector<std::shared_ptr<OriginTypeConfig>> ConfigParser::parseOriginTypeConfigs() {
    std::vector<std::shared_ptr<OriginTypeConfig>> configs;
    
    while (position_ < input_.length()) {
        skipWhitespace();
        if (position_ >= input_.length()) break;
        
        if (input_.substr(position_, 10) == "[OriginType]") {
            position_ += 10;
            skipWhitespace();
            
            auto config = parseOriginTypeConfig();
            if (config) {
                configs.push_back(config);
            }
        } else {
            advance();
        }
    }
    
    return configs;
}

void ConfigParser::skipWhitespace() {
    while (position_ < input_.length() && std::isspace(input_[position_])) {
        position_++;
    }
}

char ConfigParser::currentChar() const {
    if (position_ >= input_.length()) return '\0';
    return input_[position_];
}

char ConfigParser::peekChar() const {
    if (position_ + 1 >= input_.length()) return '\0';
    return input_[position_ + 1];
}

void ConfigParser::advance() {
    if (position_ < input_.length()) {
        position_++;
    }
}

std::shared_ptr<ConfigGroup> ConfigParser::parseConfigGroup() {
    auto config = std::make_shared<ConfigGroup>();
    
    skipWhitespace();
    if (currentChar() == '{') {
        advance();
        skipWhitespace();
        
        // 解析配置组名称
        config->name = parseIdentifier();
        skipWhitespace();
        
        // 解析配置项
        while (position_ < input_.length() && currentChar() != '}') {
            skipWhitespace();
            if (currentChar() == '}') break;
            
            config->items.push_back(parseConfigItem());
            skipWhitespace();
        }
        
        if (currentChar() == '}') advance();
    }
    
    return config;
}

std::shared_ptr<NameConfig> ConfigParser::parseNameConfig() {
    auto config = std::make_shared<NameConfig>();
    
    skipWhitespace();
    if (currentChar() == '{') {
        advance();
        skipWhitespace();
        
        // 解析原始名称
        config->originalName = parseIdentifier();
        skipWhitespace();
        
        if (currentChar() == ':') {
            advance();
            skipWhitespace();
            config->newName = parseString();
        }
        
        skipWhitespace();
        if (currentChar() == '}') advance();
    }
    
    return config;
}

std::shared_ptr<NamedConfigGroup> ConfigParser::parseNamedConfigGroup() {
    auto config = std::make_shared<NamedConfigGroup>();
    
    skipWhitespace();
    if (currentChar() == '{') {
        advance();
        skipWhitespace();
        
        // 解析命名配置组名称
        config->name = parseIdentifier();
        skipWhitespace();
        
        // 解析配置项
        while (position_ < input_.length() && currentChar() != '}') {
            skipWhitespace();
            if (currentChar() == '}') break;
            
            config->items.push_back(parseConfigItem());
            skipWhitespace();
        }
        
        if (currentChar() == '}') advance();
    }
    
    return config;
}

std::shared_ptr<OriginTypeConfig> ConfigParser::parseOriginTypeConfig() {
    auto config = std::make_shared<OriginTypeConfig>();
    
    skipWhitespace();
    if (currentChar() == '{') {
        advance();
        skipWhitespace();
        
        // 解析类型名称
        config->typeName = parseIdentifier();
        skipWhitespace();
        
        // 解析描述
        if (currentChar() == ':') {
            advance();
            skipWhitespace();
            config->description = parseString();
        }
        
        // 解析支持的扩展名
        if (input_.substr(position_, 10) == "extensions") {
            position_ += 10;
            skipWhitespace();
            if (currentChar() == ':') {
                advance();
                skipWhitespace();
                config->supportedExtensions = parseArray();
            }
        }
        
        // 解析处理器
        if (input_.substr(position_, 9) == "processor") {
            position_ += 9;
            skipWhitespace();
            if (currentChar() == ':') {
                advance();
                skipWhitespace();
                config->processor = parseString();
            }
        }
        
        skipWhitespace();
        if (currentChar() == '}') advance();
    }
    
    return config;
}

ConfigItem ConfigParser::parseConfigItem() {
    ConfigItem item;
    
    // 解析配置项名称
    item.name = parseIdentifier();
    skipWhitespace();
    
    if (currentChar() == ':') {
        advance();
        skipWhitespace();
        item.value = parseConfigValue();
    }
    
    skipWhitespace();
    return item;
}

ConfigValue ConfigParser::parseConfigValue() {
    ConfigValue value;
    
    if (currentChar() == '"') {
        value = ConfigValue(parseString());
    } else if (std::isdigit(currentChar()) || currentChar() == '-') {
        value = ConfigValue(std::stod(parseNumber()));
    } else if (input_.substr(position_, 4) == "true") {
        value = ConfigValue(true);
        position_ += 4;
    } else if (input_.substr(position_, 5) == "false") {
        value = ConfigValue(false);
        position_ += 5;
    } else if (currentChar() == '[') {
        value.type = ConfigValueType::ARRAY;
        value.arrayValue = parseArray();
    } else if (currentChar() == '{') {
        value.type = ConfigValueType::OBJECT;
        value.objectValue = parseObject();
    } else {
        value = ConfigValue(parseString());
    }
    
    return value;
}

std::string ConfigParser::parseString() {
    std::string result;
    
    if (currentChar() == '"') {
        advance();
        while (position_ < input_.length() && currentChar() != '"') {
            result += currentChar();
            advance();
        }
        if (currentChar() == '"') advance();
    } else if (currentChar() == '\'') {
        advance();
        while (position_ < input_.length() && currentChar() != '\'') {
            result += currentChar();
            advance();
        }
        if (currentChar() == '\'') advance();
    } else {
        // 解析无引号字符串
        while (position_ < input_.length() && 
               !std::isspace(currentChar()) && 
               currentChar() != ',' && 
               currentChar() != '}' && 
               currentChar() != ';') {
            result += currentChar();
            advance();
        }
    }
    
    return result;
}

std::string ConfigParser::parseIdentifier() {
    std::string result;
    
    while (position_ < input_.length() && 
           (std::isalnum(currentChar()) || currentChar() == '_' || currentChar() == '-')) {
        result += currentChar();
        advance();
    }
    
    return result;
}

std::string ConfigParser::parseNumber() {
    std::string result;
    
    if (currentChar() == '-') {
        result += currentChar();
        advance();
    }
    
    while (position_ < input_.length() && 
           (std::isdigit(currentChar()) || currentChar() == '.')) {
        result += currentChar();
        advance();
    }
    
    return result;
}

bool ConfigParser::parseBoolean() {
    if (input_.substr(position_, 4) == "true") {
        position_ += 4;
        return true;
    } else if (input_.substr(position_, 5) == "false") {
        position_ += 5;
        return false;
    }
    return false;
}

std::vector<std::string> ConfigParser::parseArray() {
    std::vector<std::string> result;
    
    if (currentChar() == '[') {
        advance();
        skipWhitespace();
        
        while (position_ < input_.length() && currentChar() != ']') {
            skipWhitespace();
            if (currentChar() == ']') break;
            
            result.push_back(parseString());
            skipWhitespace();
            
            if (currentChar() == ',') {
                advance();
                skipWhitespace();
            }
        }
        
        if (currentChar() == ']') advance();
    }
    
    return result;
}

std::map<std::string, std::string> ConfigParser::parseObject() {
    std::map<std::string, std::string> result;
    
    if (currentChar() == '{') {
        advance();
        skipWhitespace();
        
        while (position_ < input_.length() && currentChar() != '}') {
            skipWhitespace();
            if (currentChar() == '}') break;
            
            std::string key = parseIdentifier();
            skipWhitespace();
            
            if (currentChar() == ':') {
                advance();
                skipWhitespace();
                std::string value = parseString();
                result[key] = value;
            }
            
            skipWhitespace();
            
            if (currentChar() == ',') {
                advance();
                skipWhitespace();
            }
        }
        
        if (currentChar() == '}') advance();
    }
    
    return result;
}

bool ConfigParser::isKeyword(const std::string& word) const {
    static const std::vector<std::string> keywords = {
        "Configuration", "Name", "Named", "OriginType", "extensions", "processor"
    };
    
    return std::find(keywords.begin(), keywords.end(), word) != keywords.end();
}

bool ConfigParser::isConfigKeyword(const std::string& word) const {
    static const std::vector<std::string> configKeywords = {
        "Configuration", "Name", "Named", "OriginType", "extensions", "processor"
    };
    
    return std::find(configKeywords.begin(), configKeywords.end(), word) != configKeywords.end();
}

// ConfigCompiler 实现
ConfigCompiler::ConfigCompiler() {}

std::string ConfigCompiler::compile(const std::vector<std::shared_ptr<ConfigGroup>>& configs) {
    std::string result = "// Generated Configuration\n\n";
    
    for (const auto& config : configs) {
        result += compileConfigGroup(*config) + "\n";
    }
    
    return result;
}

std::string ConfigCompiler::compileConfigGroup(const ConfigGroup& config) {
    std::string result = "// Configuration Group: " + config.name + "\n";
    result += "// Description: " + config.description + "\n";
    result += "// Active: " + std::string(config.isActive ? "true" : "false") + "\n\n";
    
    for (const auto& item : config.items) {
        result += "// " + item.description + "\n";
        result += item.name + ": " + item.value.stringValue + "\n";
    }
    
    return result;
}

std::string ConfigCompiler::compileNameConfig(const NameConfig& config) {
    std::string result = "// Name Configuration\n";
    result += "// Original: " + config.originalName + "\n";
    result += "// New: " + config.newName + "\n";
    result += "// Description: " + config.description + "\n\n";
    
    return result;
}

std::string ConfigCompiler::compileNamedConfigGroup(const NamedConfigGroup& config) {
    std::string result = "// Named Configuration Group: " + config.name + "\n";
    result += "// Description: " + config.description + "\n";
    result += "// Active: " + std::string(config.isActive ? "true" : "false") + "\n\n";
    
    for (const auto& item : config.items) {
        result += "// " + item.description + "\n";
        result += item.name + ": " + item.value.stringValue + "\n";
    }
    
    return result;
}

std::string ConfigCompiler::compileOriginTypeConfig(const OriginTypeConfig& config) {
    std::string result = "// Origin Type Configuration: " + config.typeName + "\n";
    result += "// Description: " + config.description + "\n";
    result += "// Processor: " + config.processor + "\n";
    result += "// Supported Extensions: ";
    
    for (const auto& ext : config.supportedExtensions) {
        result += ext + " ";
    }
    
    result += "\n\n";
    return result;
}

void ConfigCompiler::addConfig(std::shared_ptr<ConfigGroup> config) {
    configs_[config->name] = config;
}

void ConfigCompiler::addNameConfig(std::shared_ptr<NameConfig> config) {
    nameConfigs_[config->originalName] = config;
}

void ConfigCompiler::addNamedConfigGroup(std::shared_ptr<NamedConfigGroup> config) {
    namedConfigGroups_[config->name] = config;
}

void ConfigCompiler::addOriginTypeConfig(std::shared_ptr<OriginTypeConfig> config) {
    originTypeConfigs_[config->typeName] = config;
}

std::shared_ptr<ConfigGroup> ConfigCompiler::getConfig(const std::string& name) {
    auto it = configs_.find(name);
    return (it != configs_.end()) ? it->second : nullptr;
}

std::shared_ptr<NameConfig> ConfigCompiler::getNameConfig(const std::string& name) {
    auto it = nameConfigs_.find(name);
    return (it != nameConfigs_.end()) ? it->second : nullptr;
}

std::shared_ptr<NamedConfigGroup> ConfigCompiler::getNamedConfigGroup(const std::string& name) {
    auto it = namedConfigGroups_.find(name);
    return (it != namedConfigGroups_.end()) ? it->second : nullptr;
}

std::shared_ptr<OriginTypeConfig> ConfigCompiler::getOriginTypeConfig(const std::string& name) {
    auto it = originTypeConfigs_.find(name);
    return (it != originTypeConfigs_.end()) ? it->second : nullptr;
}

std::string ConfigCompiler::generateConfigCode(const ConfigGroup& config) {
    return "// Config code for " + config.name;
}

std::string ConfigCompiler::generateNameConfigCode(const NameConfig& config) {
    return "// Name config code for " + config.originalName;
}

std::string ConfigCompiler::generateNamedConfigGroupCode(const NamedConfigGroup& config) {
    return "// Named config group code for " + config.name;
}

std::string ConfigCompiler::generateOriginTypeConfigCode(const OriginTypeConfig& config) {
    return "// Origin type config code for " + config.typeName;
}

// ConfigManager 实现
ConfigManager::ConfigManager() {}

void ConfigManager::addConfig(std::shared_ptr<ConfigGroup> config) {
    compiler_.addConfig(config);
}

void ConfigManager::addNameConfig(std::shared_ptr<NameConfig> config) {
    compiler_.addNameConfig(config);
}

void ConfigManager::addNamedConfigGroup(std::shared_ptr<NamedConfigGroup> config) {
    compiler_.addNamedConfigGroup(config);
}

void ConfigManager::addOriginTypeConfig(std::shared_ptr<OriginTypeConfig> config) {
    compiler_.addOriginTypeConfig(config);
}

std::shared_ptr<ConfigGroup> ConfigManager::getConfig(const std::string& name) {
    return compiler_.getConfig(name);
}

std::shared_ptr<NameConfig> ConfigManager::getNameConfig(const std::string& name) {
    return compiler_.getNameConfig(name);
}

std::shared_ptr<NamedConfigGroup> ConfigManager::getNamedConfigGroup(const std::string& name) {
    return compiler_.getNamedConfigGroup(name);
}

std::shared_ptr<OriginTypeConfig> ConfigManager::getOriginTypeConfig(const std::string& name) {
    return compiler_.getOriginTypeConfig(name);
}

std::string ConfigManager::generateCode(const std::vector<std::shared_ptr<ConfigGroup>>& configs) {
    return compiler_.compile(configs);
}

std::string ConfigManager::generateConfigCode() {
    std::string result = "// Configuration Code\n\n";
    
    for (const auto& config : compiler_.configs_) {
        result += compiler_.generateConfigCode(*config.second) + "\n";
    }
    
    return result;
}

std::string ConfigManager::generateNameConfigCode() {
    std::string result = "// Name Configuration Code\n\n";
    
    for (const auto& config : compiler_.nameConfigs_) {
        result += compiler_.generateNameConfigCode(*config.second) + "\n";
    }
    
    return result;
}

std::string ConfigManager::generateNamedConfigGroupCode() {
    std::string result = "// Named Configuration Group Code\n\n";
    
    for (const auto& config : compiler_.namedConfigGroups_) {
        result += compiler_.generateNamedConfigGroupCode(*config.second) + "\n";
    }
    
    return result;
}

std::string ConfigManager::generateOriginTypeConfigCode() {
    std::string result = "// Origin Type Configuration Code\n\n";
    
    for (const auto& config : compiler_.originTypeConfigs_) {
        result += compiler_.generateOriginTypeConfigCode(*config.second) + "\n";
    }
    
    return result;
}

bool ConfigManager::validateConfig(const std::string& name, const std::map<std::string, std::string>& values) {
    auto config = getConfig(name);
    if (!config) return false;
    
    for (const auto& item : config->items) {
        if (item.isRequired) {
            auto it = values.find(item.name);
            if (it == values.end()) return false;
        }
    }
    
    return true;
}

bool ConfigManager::validateNameConfig(const std::string& name, const std::string& value) {
    auto config = getNameConfig(name);
    if (!config) return false;
    
    return !value.empty();
}

bool ConfigManager::validateNamedConfigGroup(const std::string& name, const std::map<std::string, std::string>& values) {
    auto config = getNamedConfigGroup(name);
    if (!config) return false;
    
    for (const auto& item : config->items) {
        if (item.isRequired) {
            auto it = values.find(item.name);
            if (it == values.end()) return false;
        }
    }
    
    return true;
}

bool ConfigManager::validateOriginTypeConfig(const std::string& name, const std::string& value) {
    auto config = getOriginTypeConfig(name);
    if (!config) return false;
    
    return !value.empty();
}

void ConfigManager::clear() {
    compiler_.configs_.clear();
    compiler_.nameConfigs_.clear();
    compiler_.namedConfigGroups_.clear();
    compiler_.originTypeConfigs_.clear();
}

size_t ConfigManager::getConfigCount() const {
    return compiler_.configs_.size() + compiler_.nameConfigs_.size() + 
           compiler_.namedConfigGroups_.size() + compiler_.originTypeConfigs_.size();
}

std::string ConfigManager::processConfigDependencies(const std::string& content) {
    return content;
}

std::string ConfigManager::resolveConfigReferences(const std::string& content) {
    return content;
}

std::string ConfigManager::validateConfigParameters(const std::string& configName, const std::map<std::string, std::string>& parameters) {
    return "";
}

// ConfigValidator 实现
ConfigValidator::ConfigValidator() {}

void ConfigValidator::setConfigs(const std::vector<std::shared_ptr<ConfigGroup>>& configs) {
    configs_ = configs;
}

void ConfigValidator::setNameConfigs(const std::vector<std::shared_ptr<NameConfig>>& configs) {
    nameConfigs_ = configs;
}

void ConfigValidator::setNamedConfigGroups(const std::vector<std::shared_ptr<NamedConfigGroup>>& configs) {
    namedConfigGroups_ = configs;
}

void ConfigValidator::setOriginTypeConfigs(const std::vector<std::shared_ptr<OriginTypeConfig>>& configs) {
    originTypeConfigs_ = configs;
}

bool ConfigValidator::validate(const std::string& configName, const std::map<std::string, std::string>& values) {
    clearErrors();
    clearWarnings();
    
    bool isValid = true;
    
    for (const auto& config : configs_) {
        if (config->name == configName) {
            if (!checkConfig(*config, values)) {
                isValid = false;
            }
        }
    }
    
    return isValid;
}

std::vector<std::string> ConfigValidator::getValidationErrors() {
    return errors_;
}

std::vector<std::string> ConfigValidator::getValidationWarnings() {
    return warnings_;
}

void ConfigValidator::clearErrors() {
    errors_.clear();
}

void ConfigValidator::clearWarnings() {
    warnings_.clear();
}

void ConfigValidator::addError(const std::string& error) {
    errors_.push_back(error);
}

void ConfigValidator::addWarning(const std::string& warning) {
    warnings_.push_back(warning);
}

bool ConfigValidator::checkConfig(const ConfigGroup& config, const std::map<std::string, std::string>& values) {
    for (const auto& item : config.items) {
        if (item.isRequired) {
            auto it = values.find(item.name);
            if (it == values.end()) {
                addError("Required configuration item missing: " + item.name);
                return false;
            }
        }
    }
    
    return true;
}

bool ConfigValidator::checkNameConfig(const NameConfig& config, const std::string& value) {
    if (value.empty()) {
        addError("Name configuration value cannot be empty");
        return false;
    }
    
    return true;
}

bool ConfigValidator::checkNamedConfigGroup(const NamedConfigGroup& config, const std::map<std::string, std::string>& values) {
    for (const auto& item : config.items) {
        if (item.isRequired) {
            auto it = values.find(item.name);
            if (it == values.end()) {
                addError("Required named configuration item missing: " + item.name);
                return false;
            }
        }
    }
    
    return true;
}

bool ConfigValidator::checkOriginTypeConfig(const OriginTypeConfig& config, const std::string& value) {
    if (value.empty()) {
        addError("Origin type configuration value cannot be empty");
        return false;
    }
    
    return true;
}

} // namespace config_system
} // namespace chtl