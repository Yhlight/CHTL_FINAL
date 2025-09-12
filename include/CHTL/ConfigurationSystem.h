#pragma once

#include "CHTL/CHTLNode.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <unordered_set>

namespace CHTL {

/**
 * 配置项类型
 */
enum class ConfigurationType {
    BOOLEAN,
    INTEGER,
    STRING,
    ARRAY,
    OBJECT
};

/**
 * 配置项值
 */
class ConfigurationValue {
public:
    ConfigurationValue();
    ConfigurationValue(bool value);
    ConfigurationValue(int value);
    ConfigurationValue(const std::string& value);
    ConfigurationValue(const std::vector<std::string>& value);
    
    ConfigurationType getType() const;
    bool getBoolean() const;
    int getInteger() const;
    const std::string& getString() const;
    const std::vector<std::string>& getArray() const;
    
    void setBoolean(bool value);
    void setInteger(int value);
    void setString(const std::string& value);
    void setArray(const std::vector<std::string>& value);
    
    std::string toString() const;
    
private:
    ConfigurationType type_;
    bool bool_value_;
    int int_value_;
    std::string string_value_;
    std::vector<std::string> array_value_;
};

/**
 * Name配置项
 */
struct NameConfiguration {
    std::string CUSTOM_STYLE;
    std::string CUSTOM_ELEMENT;
    std::string CUSTOM_VAR;
    std::string TEMPLATE_STYLE;
    std::string TEMPLATE_ELEMENT;
    std::string TEMPLATE_VAR;
    std::string ORIGIN_HTML;
    std::string ORIGIN_STYLE;
    std::string ORIGIN_JAVASCRIPT;
    std::string IMPORT_HTML;
    std::string IMPORT_STYLE;
    std::string IMPORT_JAVASCRIPT;
    std::string IMPORT_CHTL;
    std::string IMPORT_CJMOD;
    std::string KEYWORD_INHERIT;
    std::string KEYWORD_DELETE;
    std::string KEYWORD_INSERT;
    std::string KEYWORD_AFTER;
    std::string KEYWORD_BEFORE;
    std::string KEYWORD_REPLACE;
    std::string KEYWORD_ATTOP;
    std::string KEYWORD_ATBOTTOM;
    std::string KEYWORD_FROM;
    std::string KEYWORD_AS;
    std::string KEYWORD_EXCEPT;
    std::string KEYWORD_USE;
    std::string KEYWORD_HTML5;
    std::string KEYWORD_TEXT;
    std::string KEYWORD_STYLE;
    std::string KEYWORD_SCRIPT;
    std::string KEYWORD_CUSTOM;
    std::string KEYWORD_TEMPLATE;
    std::string KEYWORD_ORIGIN;
    std::string KEYWORD_IMPORT;
    std::string KEYWORD_NAMESPACE;
    std::string CONFIGURATION_CONFIG;
    std::string IMPORT_CONFIG;
    
    // 组选项
    std::vector<std::string> CUSTOM_STYLE_GROUP;
    int OPTION_COUNT;
    
    NameConfiguration();
    void setDefaultValues();
    bool hasGroupOption(const std::string& key) const;
    std::vector<std::string> getGroupOption(const std::string& key) const;
    void setGroupOption(const std::string& key, const std::vector<std::string>& values);
};

/**
 * 配置组
 */
class ConfigurationGroup {
public:
    ConfigurationGroup(const std::string& name = "");
    ~ConfigurationGroup();
    
    // 基本信息
    const std::string& getName() const;
    void setName(const std::string& name);
    bool isNamed() const;
    
    // 配置项管理
    void setConfiguration(const std::string& key, const ConfigurationValue& value);
    ConfigurationValue getConfiguration(const std::string& key) const;
    bool hasConfiguration(const std::string& key) const;
    void removeConfiguration(const std::string& key);
    
    // Name配置
    void setNameConfiguration(const NameConfiguration& nameConfig);
    const NameConfiguration& getNameConfiguration() const;
    NameConfiguration& getNameConfiguration();
    bool hasNameConfiguration() const;
    
    // 配置项列表
    std::vector<std::string> getConfigurationKeys() const;
    std::map<std::string, ConfigurationValue> getAllConfigurations() const;
    
    // 验证
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
    // 清理
    void clear();
    
private:
    std::string name_;
    std::map<std::string, ConfigurationValue> configurations_;
    NameConfiguration name_configuration_;
    bool has_name_configuration_;
    
    // 验证辅助函数
    bool validateNameConfiguration() const;
    bool validateGroupOptions() const;
};

/**
 * 配置管理器
 */
class ConfigurationManager {
public:
    ConfigurationManager();
    ~ConfigurationManager();
    
    // 配置组管理
    void addConfigurationGroup(std::shared_ptr<ConfigurationGroup> group);
    void removeConfigurationGroup(const std::string& name);
    std::shared_ptr<ConfigurationGroup> getConfigurationGroup(const std::string& name) const;
    std::shared_ptr<ConfigurationGroup> getDefaultConfigurationGroup() const;
    
    // 配置组查询
    std::vector<std::string> getConfigurationGroupNames() const;
    std::vector<std::shared_ptr<ConfigurationGroup>> getAllConfigurationGroups() const;
    
    // 配置解析
    bool parseConfigurationFromString(const std::string& content);
    bool parseConfigurationFromFile(const std::string& filePath);
    
    // 配置生成
    std::string generateConfigurationString(const std::string& groupName = "") const;
    bool saveConfigurationToFile(const std::string& filePath, const std::string& groupName = "") const;
    
    // 配置应用
    void applyConfiguration(const std::string& groupName);
    void applyDefaultConfiguration();
    
    // 配置查询
    ConfigurationValue getConfigurationValue(const std::string& key, const std::string& groupName = "") const;
    bool hasConfigurationValue(const std::string& key, const std::string& groupName = "") const;
    
    // Name配置查询
    std::string getNameKeyword(const std::string& key, const std::string& groupName = "") const;
    std::vector<std::string> getNameGroupOption(const std::string& key, const std::string& groupName = "") const;
    bool hasNameKeyword(const std::string& key, const std::string& groupName = "") const;
    bool hasNameGroupOption(const std::string& key, const std::string& groupName = "") const;
    
    // 清理
    void clear();
    void clearConfigurationGroup(const std::string& name);
    
private:
    std::map<std::string, std::shared_ptr<ConfigurationGroup>> configuration_groups_;
    std::string default_group_name_;
    std::shared_ptr<ConfigurationGroup> active_group_;
    
    // 解析辅助函数
    bool parseConfigurationBlock(const std::string& content, const std::string& groupName = "");
    bool parseNameBlock(const std::string& content, ConfigurationGroup& group);
    bool parseConfigurationItem(const std::string& line, ConfigurationGroup& group);
    bool parseGroupOption(const std::string& line, ConfigurationGroup& group);
    
    // 生成辅助函数
    std::string generateConfigurationBlock(const ConfigurationGroup& group) const;
    std::string generateNameBlock(const NameConfiguration& nameConfig) const;
    std::string generateConfigurationItem(const std::string& key, const ConfigurationValue& value) const;
    std::string generateGroupOption(const std::string& key, const std::vector<std::string>& values) const;
    
    // 验证辅助函数
    bool validateConfigurationGroup(const ConfigurationGroup& group) const;
    bool validateNameConfiguration(const NameConfiguration& nameConfig) const;
};

/**
 * 配置节点
 */
class ConfigurationNode : public CHTLNode {
public:
    ConfigurationNode(const std::string& name = "");
    ~ConfigurationNode();
    
    // 基本信息
    const std::string& getConfigurationName() const;
    void setConfigurationName(const std::string& name);
    
    // 配置项管理
    void addConfiguration(const std::string& key, const ConfigurationValue& value);
    ConfigurationValue getConfiguration(const std::string& key) const;
    bool hasConfiguration(const std::string& key) const;
    void removeConfiguration(const std::string& key);
    
    // Name配置
    void setNameConfiguration(const NameConfiguration& nameConfig);
    const NameConfiguration& getNameConfiguration() const;
    NameConfiguration& getNameConfiguration();
    bool hasNameConfiguration() const;
    
    // 节点功能
    std::string generateHTML() const override;
    std::string generateConfigurationString() const;
    
    // 验证
    bool validate() const;
    std::vector<std::string> getValidationErrors() const;
    
private:
    std::string configuration_name_;
    std::map<std::string, ConfigurationValue> configurations_;
    NameConfiguration name_configuration_;
    bool has_name_configuration_;
    
    // 生成辅助函数
    std::string generateConfigurationBlock() const;
    std::string generateNameBlock() const;
    std::string generateConfigurationItem(const std::string& key, const ConfigurationValue& value) const;
    std::string generateGroupOption(const std::string& key, const std::vector<std::string>& values) const;
};

/**
 * 配置解析器
 */
class ConfigurationParser {
public:
    ConfigurationParser();
    ~ConfigurationParser();
    
    // 解析配置
    std::shared_ptr<ConfigurationNode> parseConfiguration(const std::string& content);
    std::vector<std::shared_ptr<ConfigurationNode>> parseConfigurations(const std::string& content);
    
    // 解析辅助函数
    bool parseConfigurationBlock(const std::string& content, ConfigurationNode& node);
    bool parseNameBlock(const std::string& content, ConfigurationNode& node);
    bool parseConfigurationItem(const std::string& line, ConfigurationNode& node);
    bool parseGroupOption(const std::string& line, ConfigurationNode& node);
    
    // 配置项解析
    ConfigurationValue parseConfigurationValue(const std::string& value);
    std::vector<std::string> parseArrayValue(const std::string& value);
    
    // 验证
    bool validateConfiguration(const ConfigurationNode& node) const;
    std::vector<std::string> getValidationErrors(const ConfigurationNode& node) const;
    
private:
    // 解析辅助函数
    std::string trim(const std::string& str) const;
    std::vector<std::string> split(const std::string& str, char delimiter) const;
    bool isBoolean(const std::string& str) const;
    bool isInteger(const std::string& str) const;
    bool isArray(const std::string& str) const;
    std::string unquote(const std::string& str) const;
};

/**
 * 配置应用器
 */
class ConfigurationApplicator {
public:
    ConfigurationApplicator();
    ~ConfigurationApplicator();
    
    // 配置应用
    void applyConfiguration(const ConfigurationGroup& group);
    void applyNameConfiguration(const NameConfiguration& nameConfig);
    
    // 关键字查询
    std::string getKeyword(const std::string& key) const;
    std::vector<std::string> getGroupOption(const std::string& key) const;
    bool hasKeyword(const std::string& key) const;
    bool hasGroupOption(const std::string& key) const;
    
    // 配置查询
    ConfigurationValue getConfiguration(const std::string& key) const;
    bool hasConfiguration(const std::string& key) const;
    
    // 清理
    void clear();
    void clearKeywords();
    void clearConfigurations();
    
private:
    std::map<std::string, std::string> keywords_;
    std::map<std::string, std::vector<std::string>> group_options_;
    std::map<std::string, ConfigurationValue> configurations_;
    
    // 应用辅助函数
    void applyKeyword(const std::string& key, const std::string& value);
    void applyGroupOption(const std::string& key, const std::vector<std::string>& values);
    void applyConfiguration(const std::string& key, const ConfigurationValue& value);
};

} // namespace CHTL