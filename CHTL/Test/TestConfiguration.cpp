#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <memory>

#include "CHTL/CHTLNode/ConfigurationNode.h"
#include "CHTL/CHTLManage/ConfigurationManager.h"

using namespace CHTL;

void testConfigurationNode() {
    std::cout << "Testing ConfigurationNode..." << std::endl;
    
    // 测试关键字配置
    auto keywordConfig = std::make_shared<ConfigurationNode>(ConfigurationType::Keyword, "MyKeywords");
    keywordConfig->keywords = {"customKeyword1", "customKeyword2"};
    keywordConfig->options["enabled"] = "true";
    
    std::cout << "Keyword Configuration:" << std::endl;
    std::cout << keywordConfig->toString() << std::endl;
    
    // 测试原始类型配置
    auto originTypeConfig = std::make_shared<ConfigurationNode>(ConfigurationType::OriginType, "MyOriginTypes");
    originTypeConfig->originTypes = {"@CustomType1", "@CustomType2"};
    originTypeConfig->options["priority"] = "high";
    
    std::cout << "Origin Type Configuration:" << std::endl;
    std::cout << originTypeConfig->toString() << std::endl;
    
    // 测试命名配置组
    auto namedConfig = std::make_shared<ConfigurationNode>(ConfigurationType::Named, "MyConfig");
    namedConfig->options["indent_size"] = "2";
    namedConfig->options["use_tabs"] = "false";
    namedConfig->options["line_ending"] = "crlf";
    
    std::cout << "Named Configuration:" << std::endl;
    std::cout << namedConfig->toString() << std::endl;
    
    // 测试导入配置组
    auto importConfig = std::make_shared<ConfigurationNode>(ConfigurationType::Import, "ImportedConfig");
    importConfig->importPath = "./configs/myconfig.chtl";
    
    std::cout << "Import Configuration:" << std::endl;
    std::cout << importConfig->toString() << std::endl;
    
    std::cout << "ConfigurationNode tests passed!" << std::endl << std::endl;
}

void testConfigurationManager() {
    std::cout << "Testing ConfigurationManager..." << std::endl;
    
    ConfigurationManager manager;
    
    // 测试添加配置
    std::map<std::string, ConfigOption> customConfig;
    customConfig["indent_size"] = {ConfigOptionType::Number, "4", {}};
    customConfig["use_tabs"] = {ConfigOptionType::Boolean, "true", {}};
    customConfig["line_ending"] = {ConfigOptionType::String, "lf", {}};
    
    manager.addConfiguration("custom", customConfig);
    assert(manager.hasConfiguration("custom"));
    
    // 测试获取选项
    ConfigOption indentOption = manager.getOption("custom", "indent_size");
    assert(indentOption.type == ConfigOptionType::Number);
    assert(indentOption.value == "4");
    
    // 测试自定义关键字
    manager.addCustomKeyword("myKeyword");
    manager.addCustomKeyword("anotherKeyword");
    assert(manager.isCustomKeyword("myKeyword"));
    assert(manager.isCustomKeyword("anotherKeyword"));
    assert(!manager.isCustomKeyword("notAKeyword"));
    
    // 测试自定义原始类型
    manager.addCustomOriginType("@MyType");
    manager.addCustomOriginType("@AnotherType");
    assert(manager.isCustomOriginType("@MyType"));
    assert(manager.isCustomOriginType("@AnotherType"));
    assert(!manager.isCustomOriginType("@NotAType"));
    
    // 测试导入配置
    manager.importConfiguration("imported", "./configs/imported.chtl");
    
    // 测试获取所有配置
    auto allConfigs = manager.getAllConfigurations();
    assert(allConfigs.find("default") != allConfigs.end());
    assert(allConfigs.find("custom") != allConfigs.end());
    
    auto customKeywords = manager.getCustomKeywords();
    assert(customKeywords.size() == 2);
    
    auto customOriginTypes = manager.getCustomOriginTypes();
    assert(customOriginTypes.size() == 2);
    
    std::cout << "ConfigurationManager tests passed!" << std::endl << std::endl;
}

void testConfigurationIntegration() {
    std::cout << "Testing Configuration Integration..." << std::endl;
    
    // 模拟解析配置组
    ConfigurationManager manager;
    
    // 模拟 [Configuration] MyProject
    std::map<std::string, ConfigOption> projectConfig;
    projectConfig["project_name"] = {ConfigOptionType::String, "MyCHTLProject", {}};
    projectConfig["version"] = {ConfigOptionType::String, "1.0.0", {}};
    projectConfig["features"] = {ConfigOptionType::List, "", {"templates", "customs", "imports"}};
    
    manager.addConfiguration("MyProject", projectConfig);
    
    // 模拟关键字自定义
    manager.addCustomKeyword("myCustomElement");
    manager.addCustomKeyword("myCustomStyle");
    
    // 模拟原始类型自定义
    manager.addCustomOriginType("@MyCustomType");
    manager.addCustomOriginType("@AnotherCustomType");
    
    // 验证配置
    assert(manager.hasConfiguration("MyProject"));
    assert(manager.isCustomKeyword("myCustomElement"));
    assert(manager.isCustomOriginType("@MyCustomType"));
    
    std::cout << "Configuration Integration tests passed!" << std::endl << std::endl;
}

int main() {
    std::cout << "=== CHTL Configuration System Tests ===" << std::endl << std::endl;
    
    testConfigurationNode();
    testConfigurationManager();
    testConfigurationIntegration();
    
    std::cout << "All configuration system tests passed!" << std::endl;
    return 0;
}