#include "chtl/template_system.h"
#include <iostream>
#include <string>

using namespace chtl::template_system;

int main() {
    std::cout << "=== CHTL 模板系统测试 ===" << std::endl;
    
    // 测试模板解析器
    std::string templateInput = R"(
[Template] @Style {
    button {
        background-color: #007bff;
        color: white;
        padding: 10px 20px;
        border: none;
        border-radius: 4px;
    }
}

[Template] @Element {
    card {
        div {
            class: "card";
            style: "margin: 10px;";
        }
    }
}

[Template] @Var {
    colors {
        primary: "#007bff";
        secondary: "#6c757d";
        success: "#28a745";
    }
}
)";
    
    std::cout << "1. 测试模板解析器..." << std::endl;
    TemplateParser parser(templateInput);
    auto instances = parser.parse();
    
    std::cout << "解析到 " << instances.size() << " 个模板实例" << std::endl;
    
    for (const auto& instance : instances) {
        std::cout << "  - 模板: " << instance->templateName 
                  << ", 实例: " << instance->instanceName 
                  << ", 类型: " << static_cast<int>(instance->type) << std::endl;
    }
    
    // 测试模板管理器
    std::cout << "\n2. 测试模板管理器..." << std::endl;
    TemplateManager manager;
    
    // 创建样式组模板
    auto styleTemplate = std::make_shared<StyleGroupTemplate>("button");
    styleTemplate->properties.push_back("background-color: #007bff;");
    styleTemplate->properties.push_back("color: white;");
    styleTemplate->properties.push_back("padding: 10px 20px;");
    
    manager.addTemplate("button", styleTemplate);
    
    // 创建元素模板
    auto elementTemplate = std::make_shared<ElementTemplate>("card");
    elementTemplate->elementType = "div";
    elementTemplate->attributes.push_back("class=\"card\"");
    elementTemplate->attributes.push_back("style=\"margin: 10px;\"");
    
    manager.addTemplate("card", elementTemplate);
    
    // 创建变量组模板
    auto varTemplate = std::make_shared<VariableGroupTemplate>("colors");
    varTemplate->variables["primary"] = "#007bff";
    varTemplate->variables["secondary"] = "#6c757d";
    varTemplate->variables["success"] = "#28a745";
    
    manager.addTemplate("colors", varTemplate);
    
    std::cout << "添加了 3 个模板到管理器" << std::endl;
    
    // 测试模板实例化
    std::cout << "\n3. 测试模板实例化..." << std::endl;
    
    // 创建样式组实例
    auto styleInstance = std::make_shared<TemplateInstance>("button", "my-button", TemplateType::STYLE_GROUP);
    styleInstance->parameters["color"] = "#ff0000";
    
    // 创建元素实例
    auto elementInstance = std::make_shared<TemplateInstance>("card", "my-card", TemplateType::ELEMENT);
    elementInstance->parameters["title"] = "My Card";
    
    // 创建变量组实例
    auto varInstance = std::make_shared<TemplateInstance>("colors", "my-colors", TemplateType::VARIABLE_GROUP);
    varInstance->parameters["primary"] = "#00ff00";
    
    std::vector<std::shared_ptr<TemplateInstance>> testInstances = {
        styleInstance, elementInstance, varInstance
    };
    
    // 生成代码
    std::string generatedCode = manager.generateCode(testInstances);
    
    std::cout << "生成的代码:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << generatedCode << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    // 测试模板继承
    std::cout << "\n4. 测试模板继承..." << std::endl;
    
    auto baseStyleTemplate = std::make_shared<StyleGroupTemplate>("base-button");
    baseStyleTemplate->properties.push_back("border: none;");
    baseStyleTemplate->properties.push_back("border-radius: 4px;");
    
    auto derivedStyleTemplate = std::make_shared<StyleGroupTemplate>("primary-button");
    derivedStyleTemplate->parentTemplate = "base-button";
    derivedStyleTemplate->inheritanceType = InheritanceType::EXPLICIT;
    derivedStyleTemplate->properties.push_back("background-color: #007bff;");
    derivedStyleTemplate->properties.push_back("color: white;");
    
    manager.addTemplate("base-button", baseStyleTemplate);
    manager.addTemplate("primary-button", derivedStyleTemplate);
    
    auto inheritedInstance = std::make_shared<TemplateInstance>("primary-button", "inherited-button", TemplateType::STYLE_GROUP);
    std::vector<std::shared_ptr<TemplateInstance>> inheritedInstances = {inheritedInstance};
    
    std::string inheritedCode = manager.generateCode(inheritedInstances);
    
    std::cout << "继承模板生成的代码:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << inheritedCode << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    std::cout << "\n=== 模板系统测试完成 ===" << std::endl;
    
    return 0;
}