#include "chtl/config_system.h"
#include <iostream>
#include <string>

using namespace chtl::config_system;

int main() {
    std::cout << "=== CHTL 配置系统测试 ===" << std::endl;
    
    // 测试配置解析器
    std::string configInput = R"(
[Configuration] {
    debug-mode {
        DEBUG_MODE: true,
        LOG_LEVEL: "debug",
        VERBOSE: true
    }
}

[Name] {
    button: "btn",
    input: "inp",
    container: "cont"
}

[Named] {
    production {
        DEBUG_MODE: false,
        LOG_LEVEL: "error",
        VERBOSE: false
    }
}

[OriginType] {
    Vue {
        description: "Vue.js component support",
        extensions: ["vue", "js", "ts"],
        processor: "vue-processor"
    }
}
)";
    
    std::cout << "1. 测试配置解析器..." << std::endl;
    ConfigParser parser(configInput);
    
    auto configs = parser.parseConfigurations();
    auto nameConfigs = parser.parseNameConfigs();
    auto namedConfigs = parser.parseNamedConfigGroups();
    auto originTypeConfigs = parser.parseOriginTypeConfigs();
    
    std::cout << "解析到 " << configs.size() << " 个配置组" << std::endl;
    std::cout << "解析到 " << nameConfigs.size() << " 个Name配置" << std::endl;
    std::cout << "解析到 " << namedConfigs.size() << " 个命名配置组" << std::endl;
    std::cout << "解析到 " << originTypeConfigs.size() << " 个OriginType配置" << std::endl;
    
    // 测试配置管理器
    std::cout << "\n2. 测试配置管理器..." << std::endl;
    ConfigManager manager;
    
    // 创建配置组
    auto configGroup = std::make_shared<ConfigGroup>("debug-mode");
    configGroup->description = "Debug mode configuration";
    configGroup->items.push_back(ConfigItem("DEBUG_MODE", ConfigValue(true), "Enable debug mode", true));
    configGroup->items.push_back(ConfigItem("LOG_LEVEL", ConfigValue("debug"), "Log level", true));
    configGroup->items.push_back(ConfigItem("VERBOSE", ConfigValue(true), "Enable verbose output", false));
    
    manager.addConfig(configGroup);
    
    // 创建Name配置
    auto nameConfig = std::make_shared<NameConfig>("button", "btn", "Button element name mapping");
    manager.addNameConfig(nameConfig);
    
    // 创建命名配置组
    auto namedConfigGroup = std::make_shared<NamedConfigGroup>("production");
    namedConfigGroup->description = "Production environment configuration";
    namedConfigGroup->items.push_back(ConfigItem("DEBUG_MODE", ConfigValue(false), "Disable debug mode", true));
    namedConfigGroup->items.push_back(ConfigItem("LOG_LEVEL", ConfigValue("error"), "Error level logging", true));
    namedConfigGroup->items.push_back(ConfigItem("VERBOSE", ConfigValue(false), "Disable verbose output", false));
    
    manager.addNamedConfigGroup(namedConfigGroup);
    
    // 创建OriginType配置
    auto originTypeConfig = std::make_shared<OriginTypeConfig>("Vue", "Vue.js component support");
    originTypeConfig->supportedExtensions = {"vue", "js", "ts"};
    originTypeConfig->processor = "vue-processor";
    
    manager.addOriginTypeConfig(originTypeConfig);
    
    std::cout << "添加了 4 个配置到管理器" << std::endl;
    
    // 测试配置生成
    std::cout << "\n3. 测试配置生成..." << std::endl;
    
    std::string configCode = manager.generateConfigCode();
    std::cout << "配置代码:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << configCode << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    std::string nameConfigCode = manager.generateNameConfigCode();
    std::cout << "Name配置代码:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << nameConfigCode << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    std::string namedConfigCode = manager.generateNamedConfigGroupCode();
    std::cout << "命名配置组代码:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << namedConfigCode << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    std::string originTypeCode = manager.generateOriginTypeConfigCode();
    std::cout << "OriginType配置代码:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << originTypeCode << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    // 测试配置验证
    std::cout << "\n4. 测试配置验证..." << std::endl;
    
    // 验证有效配置
    std::map<std::string, std::string> validConfig = {
        {"DEBUG_MODE", "true"},
        {"LOG_LEVEL", "debug"},
        {"VERBOSE", "true"}
    };
    
    bool validResult = manager.validateConfig("debug-mode", validConfig);
    std::cout << "有效配置验证: " << (validResult ? "通过" : "失败") << std::endl;
    
    // 验证无效配置
    std::map<std::string, std::string> invalidConfig = {
        {"DEBUG_MODE", "true"},
        {"LOG_LEVEL", "debug"}
        // 缺少必需的VERBOSE配置
    };
    
    bool invalidResult = manager.validateConfig("debug-mode", invalidConfig);
    std::cout << "无效配置验证: " << (invalidResult ? "通过" : "失败") << std::endl;
    
    // 测试Name配置验证
    bool nameValid = manager.validateNameConfig("button", "btn");
    std::cout << "Name配置验证: " << (nameValid ? "通过" : "失败") << std::endl;
    
    // 测试命名配置组验证
    std::map<std::string, std::string> namedConfig = {
        {"DEBUG_MODE", "false"},
        {"LOG_LEVEL", "error"},
        {"VERBOSE", "false"}
    };
    
    bool namedValid = manager.validateNamedConfigGroup("production", namedConfig);
    std::cout << "命名配置组验证: " << (namedValid ? "通过" : "失败") << std::endl;
    
    // 测试OriginType配置验证
    bool originTypeValid = manager.validateOriginTypeConfig("Vue", "vue-processor");
    std::cout << "OriginType配置验证: " << (originTypeValid ? "通过" : "失败") << std::endl;
    
    // 测试配置查找
    std::cout << "\n5. 测试配置查找..." << std::endl;
    
    auto foundConfig = manager.getConfig("debug-mode");
    auto foundNameConfig = manager.getNameConfig("button");
    auto foundNamedConfig = manager.getNamedConfigGroup("production");
    auto foundOriginTypeConfig = manager.getOriginTypeConfig("Vue");
    
    std::cout << "找到配置组: " << (foundConfig ? "是" : "否") << std::endl;
    std::cout << "找到Name配置: " << (foundNameConfig ? "是" : "否") << std::endl;
    std::cout << "找到命名配置组: " << (foundNamedConfig ? "是" : "否") << std::endl;
    std::cout << "找到OriginType配置: " << (foundOriginTypeConfig ? "是" : "否") << std::endl;
    
    // 测试配置验证器
    std::cout << "\n6. 测试配置验证器..." << std::endl;
    
    ConfigValidator validator;
    validator.setConfigs(configs);
    validator.setNameConfigs(nameConfigs);
    validator.setNamedConfigGroups(namedConfigs);
    validator.setOriginTypeConfigs(originTypeConfigs);
    
    // 验证配置
    bool validatorResult = validator.validate("debug-mode", validConfig);
    std::cout << "验证器结果: " << (validatorResult ? "通过" : "失败") << std::endl;
    
    // 显示验证错误
    auto errors = validator.getValidationErrors();
    if (!errors.empty()) {
        std::cout << "验证错误:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    // 显示验证警告
    auto warnings = validator.getValidationWarnings();
    if (!warnings.empty()) {
        std::cout << "验证警告:" << std::endl;
        for (const auto& warning : warnings) {
            std::cout << "  - " << warning << std::endl;
        }
    }
    
    // 测试配置统计
    std::cout << "\n7. 测试配置统计..." << std::endl;
    
    std::cout << "总配置数: " << manager.getConfigCount() << std::endl;
    
    // 测试配置清理
    std::cout << "\n8. 测试配置清理..." << std::endl;
    
    manager.clear();
    std::cout << "清理后配置数: " << manager.getConfigCount() << std::endl;
    
    // 测试配置值类型
    std::cout << "\n9. 测试配置值类型..." << std::endl;
    
    ConfigValue stringValue("hello");
    ConfigValue numberValue(42.5);
    ConfigValue booleanValue(true);
    
    std::cout << "字符串值: " << stringValue.stringValue << std::endl;
    std::cout << "数字值: " << numberValue.numberValue << std::endl;
    std::cout << "布尔值: " << (booleanValue.booleanValue ? "true" : "false") << std::endl;
    
    // 测试配置项
    std::cout << "\n10. 测试配置项..." << std::endl;
    
    ConfigItem item("test", ConfigValue("value"), "Test configuration item", true);
    std::cout << "配置项: " << item.name << " = " << item.value.stringValue 
              << " (必需: " << (item.isRequired ? "是" : "否") << ")" << std::endl;
    
    std::cout << "\n=== 配置系统测试完成 ===" << std::endl;
    
    return 0;
}