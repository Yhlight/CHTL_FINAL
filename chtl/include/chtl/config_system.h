#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <set>
#include <variant>

namespace chtl {
namespace config_system {

// 配置类型
enum class ConfigType {
    CONFIGURATION,  // 配置组
    NAME,           // Name配置
    NAMED_CONFIG,   // 命名配置组
    ORIGIN_TYPE     // OriginType
};

// 配置值类型
enum class ConfigValueType {
    STRING,         // 字符串
    NUMBER,         // 数字
    BOOLEAN,        // 布尔值
    ARRAY,          // 数组
    OBJECT          // 对象
};

// 配置值
struct ConfigValue {
    ConfigValueType type;
    std::string stringValue;
    double numberValue;
    bool booleanValue;
    std::vector<std::string> arrayValue;
    std::map<std::string, std::string> objectValue;
    
    ConfigValue() : type(ConfigValueType::STRING), numberValue(0), booleanValue(false) {}
    ConfigValue(const std::string& value) : type(ConfigValueType::STRING), stringValue(value), numberValue(0), booleanValue(false) {}
    ConfigValue(double value) : type(ConfigValueType::NUMBER), numberValue(value), booleanValue(false) {}
    ConfigValue(bool value) : type(ConfigValueType::BOOLEAN), numberValue(0), booleanValue(value) {}
};

// 配置项
struct ConfigItem {
    std::string name;
    ConfigValue value;
    std::string description;
    bool isRequired;
    
    ConfigItem(const std::string& n = "", const ConfigValue& v = ConfigValue(), const std::string& desc = "", bool req = false)
        : name(n), value(v), description(desc), isRequired(req) {}
};

// 配置组
struct ConfigGroup {
    std::string name;
    std::vector<ConfigItem> items;
    std::string description;
    bool isActive;
    
    ConfigGroup(const std::string& n = "") : name(n), isActive(true) {}
};

// Name配置
struct NameConfig {
    std::string originalName;
    std::string newName;
    std::string description;
    
    NameConfig(const std::string& orig = "", const std::string& newName = "", const std::string& desc = "")
        : originalName(orig), newName(newName), description(desc) {}
};

// 命名配置组
struct NamedConfigGroup {
    std::string name;
    std::vector<ConfigItem> items;
    std::string description;
    bool isActive;
    
    NamedConfigGroup(const std::string& n = "") : name(n), isActive(true) {}
};

// OriginType配置
struct OriginTypeConfig {
    std::string typeName;
    std::string description;
    std::vector<std::string> supportedExtensions;
    std::string processor;
    
    OriginTypeConfig(const std::string& type = "", const std::string& desc = "")
        : typeName(type), description(desc) {}
};

// 配置解析器
class ConfigParser {
public:
    ConfigParser(const std::string& input);
    
    std::vector<std::shared_ptr<ConfigGroup>> parseConfigurations();
    std::vector<std::shared_ptr<NameConfig>> parseNameConfigs();
    std::vector<std::shared_ptr<NamedConfigGroup>> parseNamedConfigGroups();
    std::vector<std::shared_ptr<OriginTypeConfig>> parseOriginTypeConfigs();
    
private:
    std::string input_;
    size_t position_;
    
    void skipWhitespace();
    char currentChar() const;
    char peekChar() const;
    void advance();
    
    std::shared_ptr<ConfigGroup> parseConfigGroup();
    std::shared_ptr<NameConfig> parseNameConfig();
    std::shared_ptr<NamedConfigGroup> parseNamedConfigGroup();
    std::shared_ptr<OriginTypeConfig> parseOriginTypeConfig();
    
    ConfigItem parseConfigItem();
    ConfigValue parseConfigValue();
    std::string parseString();
    std::string parseIdentifier();
    std::string parseNumber();
    bool parseBoolean();
    std::vector<std::string> parseArray();
    std::map<std::string, std::string> parseObject();
    
    bool isKeyword(const std::string& word) const;
    bool isConfigKeyword(const std::string& word) const;
};

// 配置编译器
class ConfigCompiler {
public:
    ConfigCompiler();
    
    std::string compile(const std::vector<std::shared_ptr<ConfigGroup>>& configs);
    std::string compileConfigGroup(const ConfigGroup& config);
    std::string compileNameConfig(const NameConfig& config);
    std::string compileNamedConfigGroup(const NamedConfigGroup& config);
    std::string compileOriginTypeConfig(const OriginTypeConfig& config);
    
    void addConfig(std::shared_ptr<ConfigGroup> config);
    void addNameConfig(std::shared_ptr<NameConfig> config);
    void addNamedConfigGroup(std::shared_ptr<NamedConfigGroup> config);
    void addOriginTypeConfig(std::shared_ptr<OriginTypeConfig> config);
    
    std::shared_ptr<ConfigGroup> getConfig(const std::string& name);
    std::shared_ptr<NameConfig> getNameConfig(const std::string& name);
    std::shared_ptr<NamedConfigGroup> getNamedConfigGroup(const std::string& name);
    std::shared_ptr<OriginTypeConfig> getOriginTypeConfig(const std::string& name);
    
public:
    std::map<std::string, std::shared_ptr<ConfigGroup>> configs_;
    std::map<std::string, std::shared_ptr<NameConfig>> nameConfigs_;
    std::map<std::string, std::shared_ptr<NamedConfigGroup>> namedConfigGroups_;
    std::map<std::string, std::shared_ptr<OriginTypeConfig>> originTypeConfigs_;
    
    std::string generateConfigCode(const ConfigGroup& config);
    std::string generateNameConfigCode(const NameConfig& config);
    std::string generateNamedConfigGroupCode(const NamedConfigGroup& config);
    std::string generateOriginTypeConfigCode(const OriginTypeConfig& config);
};

// 配置管理器
class ConfigManager {
public:
    ConfigManager();
    
    void addConfig(std::shared_ptr<ConfigGroup> config);
    void addNameConfig(std::shared_ptr<NameConfig> config);
    void addNamedConfigGroup(std::shared_ptr<NamedConfigGroup> config);
    void addOriginTypeConfig(std::shared_ptr<OriginTypeConfig> config);
    
    std::shared_ptr<ConfigGroup> getConfig(const std::string& name);
    std::shared_ptr<NameConfig> getNameConfig(const std::string& name);
    std::shared_ptr<NamedConfigGroup> getNamedConfigGroup(const std::string& name);
    std::shared_ptr<OriginTypeConfig> getOriginTypeConfig(const std::string& name);
    
    std::string generateCode(const std::vector<std::shared_ptr<ConfigGroup>>& configs);
    std::string generateConfigCode();
    std::string generateNameConfigCode();
    std::string generateNamedConfigGroupCode();
    std::string generateOriginTypeConfigCode();
    
    bool validateConfig(const std::string& name, const std::map<std::string, std::string>& values);
    bool validateNameConfig(const std::string& name, const std::string& value);
    bool validateNamedConfigGroup(const std::string& name, const std::map<std::string, std::string>& values);
    bool validateOriginTypeConfig(const std::string& name, const std::string& value);
    
    void clear();
    size_t getConfigCount() const;
    
private:
    ConfigCompiler compiler_;
    
    std::string processConfigDependencies(const std::string& content);
    std::string resolveConfigReferences(const std::string& content);
    std::string validateConfigParameters(const std::string& configName, const std::map<std::string, std::string>& parameters);
};

// 配置验证器
class ConfigValidator {
public:
    ConfigValidator();
    
    void setConfigs(const std::vector<std::shared_ptr<ConfigGroup>>& configs);
    void setNameConfigs(const std::vector<std::shared_ptr<NameConfig>>& configs);
    void setNamedConfigGroups(const std::vector<std::shared_ptr<NamedConfigGroup>>& configs);
    void setOriginTypeConfigs(const std::vector<std::shared_ptr<OriginTypeConfig>>& configs);
    
    bool validate(const std::string& configName, const std::map<std::string, std::string>& values);
    std::vector<std::string> getValidationErrors();
    std::vector<std::string> getValidationWarnings();
    
    void clearErrors();
    void clearWarnings();
    
private:
    std::vector<std::shared_ptr<ConfigGroup>> configs_;
    std::vector<std::shared_ptr<NameConfig>> nameConfigs_;
    std::vector<std::shared_ptr<NamedConfigGroup>> namedConfigGroups_;
    std::vector<std::shared_ptr<OriginTypeConfig>> originTypeConfigs_;
    std::vector<std::string> errors_;
    std::vector<std::string> warnings_;
    
    void addError(const std::string& error);
    void addWarning(const std::string& warning);
    bool checkConfig(const ConfigGroup& config, const std::map<std::string, std::string>& values);
    bool checkNameConfig(const NameConfig& config, const std::string& value);
    bool checkNamedConfigGroup(const NamedConfigGroup& config, const std::map<std::string, std::string>& values);
    bool checkOriginTypeConfig(const OriginTypeConfig& config, const std::string& value);
};

} // namespace config_system
} // namespace chtl