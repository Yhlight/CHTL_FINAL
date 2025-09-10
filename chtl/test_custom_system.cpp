#include "chtl/custom_system.h"
#include <iostream>
#include <string>

using namespace chtl::custom_system;

int main() {
    std::cout << "=== CHTL 自定义系统测试 ===" << std::endl;
    
    // 测试自定义解析器
    std::string customInput = R"(
[Custom] @Style {
    primary-button {
        from base-button
        delete (color, font-size)
        remove inherit
        background-color: #007bff;
        border-radius: 8px;
    }
}

[Custom] @Element {
    enhanced-card {
        from base-card
        add ("<div class='header'></div>", "<div class='footer'></div>")
        insert ("<div class='content'></div>")
        delete ("<div class='old-content'></div>")
        remove inherit
    }
}

[Custom] @Var {
    extended-colors {
        from base-colors
        add (accent: "#ff6b6b", warning: "#ffa726")
        modify (primary: "#0056b3", secondary: "#5a6268")
        delete (old-color, deprecated-color)
        remove inherit
    }
}
)";
    
    std::cout << "1. 测试自定义解析器..." << std::endl;
    CustomParser parser(customInput);
    auto instances = parser.parse();
    
    std::cout << "解析到 " << instances.size() << " 个自定义实例" << std::endl;
    
    for (const auto& instance : instances) {
        std::cout << "  - 自定义: " << instance->customName 
                  << ", 实例: " << instance->instanceName 
                  << ", 类型: " << static_cast<int>(instance->type) << std::endl;
    }
    
    // 测试自定义管理器
    std::cout << "\n2. 测试自定义管理器..." << std::endl;
    CustomManager manager;
    
    // 创建样式组自定义
    auto styleCustom = std::make_shared<StyleGroupCustom>("primary-button");
    styleCustom->baseTemplate = "base-button";
    styleCustom->deletedProperties.insert("color");
    styleCustom->deletedProperties.insert("font-size");
    styleCustom->deletedInheritance.insert("inherit");
    styleCustom->properties.push_back("background-color: #007bff;");
    styleCustom->properties.push_back("border-radius: 8px;");
    
    manager.addCustom("primary-button", styleCustom);
    
    // 创建元素自定义
    auto elementCustom = std::make_shared<ElementCustom>("enhanced-card");
    elementCustom->baseTemplate = "base-card";
    elementCustom->addedElements.push_back("<div class='header'></div>");
    elementCustom->addedElements.push_back("<div class='footer'></div>");
    elementCustom->insertedElements.push_back("<div class='content'></div>");
    elementCustom->deletedElements.insert("<div class='old-content'></div>");
    elementCustom->deletedInheritance.insert("inherit");
    
    manager.addCustom("enhanced-card", elementCustom);
    
    // 创建变量组自定义
    auto varCustom = std::make_shared<VariableGroupCustom>("extended-colors");
    varCustom->baseTemplate = "base-colors";
    varCustom->addedVariables["accent"] = "#ff6b6b";
    varCustom->addedVariables["warning"] = "#ffa726";
    varCustom->modifiedVariables["primary"] = "#0056b3";
    varCustom->modifiedVariables["secondary"] = "#5a6268";
    varCustom->deletedVariables.insert("old-color");
    varCustom->deletedVariables.insert("deprecated-color");
    varCustom->deletedInheritance.insert("inherit");
    
    manager.addCustom("extended-colors", varCustom);
    
    std::cout << "添加了 3 个自定义到管理器" << std::endl;
    
    // 测试自定义实例化
    std::cout << "\n3. 测试自定义实例化..." << std::endl;
    
    // 创建样式组自定义实例
    auto styleInstance = std::make_shared<CustomInstance>("primary-button", "my-primary-button", CustomType::STYLE_GROUP);
    styleInstance->parameters["color"] = "#ffffff";
    
    // 创建元素自定义实例
    auto elementInstance = std::make_shared<CustomInstance>("enhanced-card", "my-enhanced-card", CustomType::ELEMENT);
    elementInstance->parameters["title"] = "My Card";
    
    // 创建变量组自定义实例
    auto varInstance = std::make_shared<CustomInstance>("extended-colors", "my-extended-colors", CustomType::VARIABLE_GROUP);
    varInstance->parameters["accent"] = "#ff0000";
    
    std::vector<std::shared_ptr<CustomInstance>> testInstances = {
        styleInstance, elementInstance, varInstance
    };
    
    // 生成代码
    std::string generatedCode = manager.generateCode(testInstances);
    
    std::cout << "生成的代码:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << generatedCode << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    // 测试无值样式组
    std::cout << "\n4. 测试无值样式组..." << std::endl;
    
    auto unvaluedStyleCustom = std::make_shared<StyleGroupCustom>("unvalued-style");
    unvaluedStyleCustom->unvalued = true;
    unvaluedStyleCustom->properties.push_back("/* 无值样式组内容 */");
    
    manager.addCustom("unvalued-style", unvaluedStyleCustom);
    
    auto unvaluedInstance = std::make_shared<CustomInstance>("unvalued-style", "my-unvalued-style", CustomType::STYLE_GROUP);
    std::vector<std::shared_ptr<CustomInstance>> unvaluedInstances = {unvaluedInstance};
    
    std::string unvaluedCode = manager.generateCode(unvaluedInstances);
    
    std::cout << "无值样式组生成的代码:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << unvaluedCode << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    // 测试特例化
    std::cout << "\n5. 测试特例化..." << std::endl;
    
    auto specializedStyleCustom = std::make_shared<StyleGroupCustom>("specialized-button");
    specializedStyleCustom->baseTemplate = "base-button";
    specializedStyleCustom->deletedProperties.insert("padding");
    specializedStyleCustom->deletedInheritance.insert("inherit");
    specializedStyleCustom->properties.push_back("padding: 15px 30px;");
    specializedStyleCustom->properties.push_back("font-weight: bold;");
    
    manager.addCustom("specialized-button", specializedStyleCustom);
    
    auto specializedInstance = std::make_shared<CustomInstance>("specialized-button", "my-specialized-button", CustomType::STYLE_GROUP);
    std::vector<std::shared_ptr<CustomInstance>> specializedInstances = {specializedInstance};
    
    std::string specializedCode = manager.generateCode(specializedInstances);
    
    std::cout << "特例化生成的代码:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << specializedCode << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    std::cout << "\n=== 自定义系统测试完成 ===" << std::endl;
    
    return 0;
}