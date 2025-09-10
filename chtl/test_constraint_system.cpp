#include "chtl/constraint_system.h"
#include <iostream>
#include <string>

using namespace chtl::constraint_system;

int main() {
    std::cout << "=== CHTL 约束系统测试 ===" << std::endl;
    
    // 测试约束解析器
    std::string constraintInput = R"(
except button-constraint precise html {
    type == "button",
    disabled != "true",
    class in "btn btn-primary btn-secondary"
}

except input-constraint type html {
    type == "text" || type == "email" || type == "password",
    required == "true" || required == "false",
    placeholder != ""
}

except global-constraint global html {
    id != "",
    class != "",
    style != ""
}
)";
    
    std::cout << "1. 测试约束解析器..." << std::endl;
    ConstraintParser parser(constraintInput);
    auto rules = parser.parse();
    
    std::cout << "解析到 " << rules.size() << " 个约束规则" << std::endl;
    
    for (const auto& rule : rules) {
        std::cout << "  - 规则: " << rule->name 
                  << ", 类型: " << static_cast<int>(rule->type)
                  << ", 目标: " << static_cast<int>(rule->target)
                  << ", 条件数: " << rule->conditions.size() << std::endl;
    }
    
    // 测试约束管理器
    std::cout << "\n2. 测试约束管理器..." << std::endl;
    ConstraintManager manager;
    
    // 创建精确约束
    auto preciseRule = std::make_shared<ConstraintRule>("button-constraint", ConstraintType::PRECISE, ConstraintTarget::HTML_ELEMENT);
    preciseRule->conditions.push_back(ConstraintCondition("type", "button", "=="));
    preciseRule->conditions.push_back(ConstraintCondition("disabled", "true", "!="));
    preciseRule->conditions.push_back(ConstraintCondition("class", "btn btn-primary btn-secondary", "in"));
    
    manager.addRule(preciseRule);
    
    // 创建类型约束
    auto typeRule = std::make_shared<ConstraintRule>("input-constraint", ConstraintType::TYPE, ConstraintTarget::HTML_ELEMENT);
    typeRule->conditions.push_back(ConstraintCondition("type", "text", "=="));
    typeRule->conditions.push_back(ConstraintCondition("required", "true", "=="));
    typeRule->conditions.push_back(ConstraintCondition("placeholder", "", "!="));
    
    manager.addRule(typeRule);
    
    // 创建全局约束
    auto globalRule = std::make_shared<ConstraintRule>("global-constraint", ConstraintType::GLOBAL, ConstraintTarget::HTML_ELEMENT);
    globalRule->conditions.push_back(ConstraintCondition("id", "", "!="));
    globalRule->conditions.push_back(ConstraintCondition("class", "", "!="));
    globalRule->conditions.push_back(ConstraintCondition("style", "", "!="));
    
    manager.addRule(globalRule);
    
    std::cout << "添加了 3 个约束规则到管理器" << std::endl;
    
    // 测试约束验证
    std::cout << "\n3. 测试约束验证..." << std::endl;
    
    // 测试HTML元素验证
    std::map<std::string, std::string> buttonAttributes = {
        {"type", "button"},
        {"disabled", "false"},
        {"class", "btn btn-primary"},
        {"id", "my-button"},
        {"style", "margin: 10px;"}
    };
    
    bool buttonValid = manager.validateHTMLElement("button", buttonAttributes);
    std::cout << "按钮验证: " << (buttonValid ? "通过" : "失败") << std::endl;
    
    // 测试输入元素验证
    std::map<std::string, std::string> inputAttributes = {
        {"type", "text"},
        {"required", "true"},
        {"placeholder", "Enter your name"},
        {"id", "name-input"},
        {"class", "form-control"},
        {"style", "width: 100%;"}
    };
    
    bool inputValid = manager.validateHTMLElement("input", inputAttributes);
    std::cout << "输入框验证: " << (inputValid ? "通过" : "失败") << std::endl;
    
    // 测试无效元素验证
    std::map<std::string, std::string> invalidAttributes = {
        {"type", "button"},
        {"disabled", "true"},  // 违反约束
        {"class", "btn btn-primary"},
        {"id", "my-button"},
        {"style", "margin: 10px;"}
    };
    
    bool invalidValid = manager.validateHTMLElement("button", invalidAttributes);
    std::cout << "无效按钮验证: " << (invalidValid ? "通过" : "失败") << std::endl;
    
    // 测试约束编译器
    std::cout << "\n4. 测试约束编译器..." << std::endl;
    
    std::string generatedCode = manager.generateCode(rules);
    std::cout << "生成的约束代码:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << generatedCode << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    // 测试约束验证器
    std::cout << "\n5. 测试约束验证器..." << std::endl;
    
    ConstraintValidator validator;
    validator.setRules(rules);
    
    // 验证有效目标
    std::map<std::string, std::string> validTarget = {
        {"type", "button"},
        {"disabled", "false"},
        {"class", "btn btn-primary"},
        {"id", "test-button"},
        {"style", "color: red;"}
    };
    
    bool validResult = validator.validate("button", validTarget);
    std::cout << "有效目标验证: " << (validResult ? "通过" : "失败") << std::endl;
    
    // 验证无效目标
    std::map<std::string, std::string> invalidTarget = {
        {"type", "button"},
        {"disabled", "true"},  // 违反约束
        {"class", "btn btn-primary"},
        {"id", "test-button"},
        {"style", "color: red;"}
    };
    
    bool invalidResult = validator.validate("button", invalidTarget);
    std::cout << "无效目标验证: " << (invalidResult ? "通过" : "失败") << std::endl;
    
    // 显示验证错误
    auto errors = validator.getValidationErrors();
    if (!errors.empty()) {
        std::cout << "验证错误:" << std::endl;
        for (const auto& error : errors) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    // 测试约束条件
    std::cout << "\n6. 测试约束条件..." << std::endl;
    
    ConstraintCondition condition1("type", "button", "==");
    ConstraintCondition condition2("disabled", "true", "!=");
    ConstraintCondition condition3("class", "btn btn-primary btn-secondary", "in");
    
    std::cout << "条件1: " << condition1.name << " " << condition1.operator_ << " " << condition1.value << std::endl;
    std::cout << "条件2: " << condition2.name << " " << condition2.operator_ << " " << condition2.value << std::endl;
    std::cout << "条件3: " << condition3.name << " " << condition3.operator_ << " " << condition3.value << std::endl;
    
    // 测试约束作用域
    std::cout << "\n7. 测试约束作用域..." << std::endl;
    
    auto scopedRule = std::make_shared<ConstraintRule>("scoped-constraint", ConstraintType::PRECISE, ConstraintTarget::HTML_ELEMENT);
    scopedRule->scope = "form .input-group";
    scopedRule->conditions.push_back(ConstraintCondition("type", "text", "=="));
    
    manager.addRule(scopedRule);
    
    std::cout << "作用域约束: " << scopedRule->name << " 作用域: " << scopedRule->scope << std::endl;
    
    // 测试约束统计
    std::cout << "\n8. 测试约束统计..." << std::endl;
    
    std::cout << "总约束规则数: " << manager.getRuleCount() << std::endl;
    
    // 测试约束清理
    std::cout << "\n9. 测试约束清理..." << std::endl;
    
    manager.clear();
    std::cout << "清理后约束规则数: " << manager.getRuleCount() << std::endl;
    
    std::cout << "\n=== 约束系统测试完成 ===" << std::endl;
    
    return 0;
}