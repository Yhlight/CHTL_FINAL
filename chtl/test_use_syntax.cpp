#include "chtl/use_syntax.h"
#include <iostream>
#include <string>

using namespace chtl::use_syntax;

int main() {
    std::cout << "=== CHTL Use语法测试 ===" << std::endl;
    
    // 测试use解析器
    std::string useInput = R"(
use html5
use xhtml-strict
use config(debug-mode: true, log-level: "debug")
use production(optimize: true, minify: true)
)";
    
    std::cout << "1. 测试use解析器..." << std::endl;
    UseParser parser(useInput);
    auto statements = parser.parse();
    
    std::cout << "解析到 " << statements.size() << " 个use语句" << std::endl;
    
    for (const auto& statement : statements) {
        std::cout << "  - 类型: " << static_cast<int>(statement->type)
                  << ", 值: " << statement->value
                  << ", 参数数: " << statement->parameters.size() << std::endl;
    }
    
    // 测试use管理器
    std::cout << "\n2. 测试use管理器..." << std::endl;
    UseManager manager;
    
    // 创建HTML5类型use语句
    auto html5Statement = std::make_shared<UseStatement>(UseType::HTML5_TYPE, "html5");
    manager.addUseStatement(html5Statement);
    
    // 创建XHTML类型use语句
    auto xhtmlStatement = std::make_shared<UseStatement>(UseType::HTML5_TYPE, "xhtml-strict");
    manager.addUseStatement(xhtmlStatement);
    
    // 创建配置组use语句
    auto configStatement = std::make_shared<UseStatement>(UseType::CONFIG_GROUP, "debug-mode");
    configStatement->parameters["log-level"] = "debug";
    configStatement->parameters["verbose"] = "true";
    manager.addUseStatement(configStatement);
    
    std::cout << "添加了 3 个use语句到管理器" << std::endl;
    
    // 测试HTML5类型支持
    std::cout << "\n3. 测试HTML5类型支持..." << std::endl;
    
    auto supportedTypes = HTML5TypeSupport::getSupportedTypes();
    std::cout << "支持的HTML5类型: ";
    for (const auto& type : supportedTypes) {
        std::cout << type << " ";
    }
    std::cout << std::endl;
    
    std::cout << "html5类型支持: " << (HTML5TypeSupport::isSupportedType("html5") ? "是" : "否") << std::endl;
    std::cout << "html5类型描述: " << HTML5TypeSupport::getTypeDescription("html5") << std::endl;
    
    auto features = HTML5TypeSupport::getTypeFeatures("html5");
    std::cout << "html5类型特性: ";
    for (const auto& feature : features) {
        std::cout << feature << " ";
    }
    std::cout << std::endl;
    
    // 测试HTML5文档类型生成
    std::cout << "\n4. 测试HTML5文档类型生成..." << std::endl;
    
    std::string html5Doctype = HTML5TypeSupport::generateHTML5Doctype("html5");
    std::cout << "HTML5文档类型: " << html5Doctype << std::endl;
    
    std::string xhtmlDoctype = HTML5TypeSupport::generateXHTMLDoctype("xhtml-strict");
    std::cout << "XHTML文档类型: " << xhtmlDoctype << std::endl;
    
    std::string metaTags = HTML5TypeSupport::generateMetaTags("html5");
    std::cout << "HTML5元标签:" << std::endl;
    std::cout << metaTags << std::endl;
    
    // 测试配置组支持
    std::cout << "\n5. 测试配置组支持..." << std::endl;
    
    auto availableGroups = ConfigGroupSupport::getAvailableConfigGroups();
    std::cout << "可用的配置组: ";
    for (const auto& group : availableGroups) {
        std::cout << group << " ";
    }
    std::cout << std::endl;
    
    std::cout << "debug-mode配置组可用: " << (ConfigGroupSupport::isAvailableConfigGroup("debug-mode") ? "是" : "否") << std::endl;
    std::cout << "debug-mode配置组描述: " << ConfigGroupSupport::getConfigGroupDescription("debug-mode") << std::endl;
    
    auto parameters = ConfigGroupSupport::getConfigGroupParameters("debug-mode");
    std::cout << "debug-mode配置组参数: ";
    for (const auto& param : parameters) {
        std::cout << param << " ";
    }
    std::cout << std::endl;
    
    // 测试配置组代码生成
    std::cout << "\n6. 测试配置组代码生成..." << std::endl;
    
    std::map<std::string, std::string> configParams = {
        {"DEBUG_MODE", "true"},
        {"LOG_LEVEL", "debug"},
        {"VERBOSE", "true"}
    };
    
    std::string configCode = ConfigGroupSupport::generateConfigGroupCode("debug-mode", configParams);
    std::cout << "配置组代码:" << std::endl;
    std::cout << configCode << std::endl;
    
    std::string configImport = ConfigGroupSupport::generateConfigGroupImport("debug-mode");
    std::cout << "配置组导入: " << configImport << std::endl;
    
    std::string configUsage = ConfigGroupSupport::generateConfigGroupUsage("debug-mode", configParams);
    std::cout << "配置组使用: " << configUsage << std::endl;
    
    // 测试use编译器
    std::cout << "\n7. 测试use编译器..." << std::endl;
    
    std::string generatedCode = manager.generateCode(statements);
    std::cout << "生成的use代码:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << generatedCode << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    // 测试use验证
    std::cout << "\n8. 测试use验证..." << std::endl;
    
    bool html5Valid = manager.validateHTML5Type("html5");
    std::cout << "HTML5类型验证: " << (html5Valid ? "通过" : "失败") << std::endl;
    
    bool xhtmlValid = manager.validateHTML5Type("xhtml-strict");
    std::cout << "XHTML类型验证: " << (xhtmlValid ? "通过" : "失败") << std::endl;
    
    bool invalidValid = manager.validateHTML5Type("invalid-type");
    std::cout << "无效类型验证: " << (invalidValid ? "通过" : "失败") << std::endl;
    
    // 测试配置组验证
    ConfigGroupUse configGroupUse("debug-mode");
    configGroupUse.parameters["log-level"] = "debug";
    
    bool configValid = manager.validateConfigGroupUse(configGroupUse);
    std::cout << "配置组验证: " << (configValid ? "通过" : "失败") << std::endl;
    
    // 测试use验证器
    std::cout << "\n9. 测试use验证器..." << std::endl;
    
    UseValidator validator;
    validator.setUseStatements(statements);
    
    // 验证有效use语句
    bool validResult = validator.validate(*html5Statement);
    std::cout << "有效use语句验证: " << (validResult ? "通过" : "失败") << std::endl;
    
    // 验证无效use语句
    auto invalidStatement = std::make_shared<UseStatement>(UseType::HTML5_TYPE, "invalid-type");
    bool invalidResult = validator.validate(*invalidStatement);
    std::cout << "无效use语句验证: " << (invalidResult ? "通过" : "失败") << std::endl;
    
    // 显示验证错误
    auto errors = validator.getValidationErrors();
    if (!errors.empty()) {
        std::cout << "验证错误:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    // 测试HTML5类型设置
    std::cout << "\n10. 测试HTML5类型设置..." << std::endl;
    
    manager.setHTML5Type(HTML5Type::HTML5_STRICT);
    HTML5Type currentType = manager.getHTML5Type();
    std::cout << "当前HTML5类型: " << static_cast<int>(currentType) << std::endl;
    
    // 测试配置组使用
    std::cout << "\n11. 测试配置组使用..." << std::endl;
    
    ConfigGroupUse debugConfig("debug-mode");
    debugConfig.parameters["log-level"] = "debug";
    debugConfig.parameters["verbose"] = "true";
    
    manager.addConfigGroupUse(debugConfig);
    
    ConfigGroupUse productionConfig("production");
    productionConfig.parameters["optimize"] = "true";
    productionConfig.parameters["minify"] = "true";
    
    manager.addConfigGroupUse(productionConfig);
    
    auto configGroupUses = manager.getConfigGroupUses();
    std::cout << "配置组使用数: " << configGroupUses.size() << std::endl;
    
    for (const auto& config : configGroupUses) {
        std::cout << "  - " << config.groupName << " (参数数: " << config.parameters.size() << ")" << std::endl;
    }
    
    // 测试use统计
    std::cout << "\n12. 测试use统计..." << std::endl;
    
    std::cout << "总use语句数: " << manager.getUseStatementCount() << std::endl;
    
    // 测试use清理
    std::cout << "\n13. 测试use清理..." << std::endl;
    
    manager.clear();
    std::cout << "清理后use语句数: " << manager.getUseStatementCount() << std::endl;
    
    std::cout << "\n=== Use语法测试完成 ===" << std::endl;
    
    return 0;
}