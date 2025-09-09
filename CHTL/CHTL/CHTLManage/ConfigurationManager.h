#pragma once
#include <string>
#include <vector>
#include <map>
#include <memory>

namespace CHTL {

enum class ConfigOptionType {
    String,
    Number,
    Boolean,
    List
};

struct ConfigOption {
    ConfigOptionType type;
    std::string value;
    std::vector<std::string> listValues; // For list type options
};

class ConfigurationManager {
public:
    ConfigurationManager();
    
    // 配置组管理
    void addConfiguration(const std::string& name, const std::map<std::string, ConfigOption>& options);
    void removeConfiguration(const std::string& name);
    bool hasConfiguration(const std::string& name) const;
    
    // 选项管理
    void setOption(const std::string& configName, const std::string& optionName, const ConfigOption& option);
    ConfigOption getOption(const std::string& configName, const std::string& optionName) const;
    bool hasOption(const std::string& configName, const std::string& optionName) const;
    
    // 关键字自定义
    void addCustomKeyword(const std::string& keyword);
    void removeCustomKeyword(const std::string& keyword);
    bool isCustomKeyword(const std::string& keyword) const;
    
    // 原始类型自定义
    void addCustomOriginType(const std::string& originType);
    void removeCustomOriginType(const std::string& originType);
    bool isCustomOriginType(const std::string& originType) const;
    
    // 导入配置组
    void importConfiguration(const std::string& configName, const std::string& importPath);
    
    // 获取所有配置
    std::map<std::string, std::map<std::string, ConfigOption>> getAllConfigurations() const;
    std::vector<std::string> getCustomKeywords() const;
    std::vector<std::string> getCustomOriginTypes() const;

private:
    std::map<std::string, std::map<std::string, ConfigOption>> configurations;
    std::vector<std::string> customKeywords;
    std::vector<std::string> customOriginTypes;
    std::map<std::string, std::string> importedConfigurations; // configName -> importPath
};

} // namespace CHTL