#include <iostream>
#include <string>
#include "chtl/style_system.h"

using namespace chtl::style;

int main() {
    std::cout << "=== CHTL样式系统测试 ===" << std::endl;
    
    // 创建样式上下文
    StyleContext context;
    
    // 添加一些变量
    context.variables["primary-color"] = std::make_shared<StyleValue>(StyleValueType::LITERAL, "#007bff");
    context.variables["secondary-color"] = std::make_shared<StyleValue>(StyleValueType::LITERAL, "#6c757d");
    context.variables["font-size"] = std::make_shared<StyleValue>(StyleValueType::LITERAL, "16px");
    
    // 创建样式管理器
    StyleManager manager;
    manager.setContext(context);
    
    // 测试表达式解析
    std::cout << "\n=== 表达式解析测试 ===" << std::endl;
    
    std::vector<std::string> expressions = {
        "primary-color",
        "font-size + 2px",
        "primary-color == secondary-color",
        "font-size > 14px ? 'large' : 'small'",
        "calc(100% - 20px)",
        "rgb(255, 0, 0)",
        "var(--custom-color)"
    };
    
    for (const auto& expr : expressions) {
        std::cout << "表达式: " << expr << std::endl;
        
        StyleExpressionParser parser(expr, context);
        auto value = parser.parse();
        
        if (value) {
            std::cout << "  类型: " << static_cast<int>(value->type) << std::endl;
            std::cout << "  值: " << value->value << std::endl;
        } else {
            std::cout << "  解析失败" << std::endl;
        }
        std::cout << std::endl;
    }
    
    // 测试选择器解析
    std::cout << "\n=== 选择器解析测试 ===" << std::endl;
    
    std::vector<std::string> selectors = {
        "button",
        ".btn-primary",
        "#main-content",
        "div.container",
        "button:hover",
        "div > p",
        "div + p",
        "div ~ p",
        "[data-toggle]",
        "div[class*='col-']"
    };
    
    for (const auto& selector : selectors) {
        std::cout << "选择器: " << selector << std::endl;
        
        StyleSelectorParser parser(selector, context);
        auto result = parser.parse();
        
        std::cout << "  解析结果: " << result << std::endl;
        std::cout << std::endl;
    }
    
    // 测试样式组
    std::cout << "\n=== 样式组测试 ===" << std::endl;
    
    StyleGroup buttonGroup("btn");
    buttonGroup.properties.push_back(StyleProperty("background-color", 
        std::make_shared<StyleValue>(StyleValueType::LITERAL, "primary-color")));
    buttonGroup.properties.push_back(StyleProperty("color", 
        std::make_shared<StyleValue>(StyleValueType::LITERAL, "white")));
    buttonGroup.properties.push_back(StyleProperty("padding", 
        std::make_shared<StyleValue>(StyleValueType::LITERAL, "10px 20px")));
    buttonGroup.properties.push_back(StyleProperty("border-radius", 
        std::make_shared<StyleValue>(StyleValueType::LITERAL, "4px")));
    
    manager.addStyleGroup("btn", buttonGroup);
    
    // 测试样式规则
    std::cout << "\n=== 样式规则测试 ===" << std::endl;
    
    StyleRule buttonRule("button");
    buttonRule.properties.push_back(StyleProperty("font-family", 
        std::make_shared<StyleValue>(StyleValueType::LITERAL, "Arial, sans-serif")));
    buttonRule.properties.push_back(StyleProperty("font-size", 
        std::make_shared<StyleValue>(StyleValueType::LITERAL, "font-size")));
    buttonRule.properties.push_back(StyleProperty("cursor", 
        std::make_shared<StyleValue>(StyleValueType::LITERAL, "pointer")));
    
    manager.addStyleRule("button", buttonRule);
    
    // 测试样式属性
    std::cout << "\n=== 样式属性测试 ===" << std::endl;
    
    manager.addStyleProperty("button:hover", StyleProperty("background-color", 
        std::make_shared<StyleValue>(StyleValueType::LITERAL, "darkblue")));
    manager.addStyleProperty("button:active", StyleProperty("transform", 
        std::make_shared<StyleValue>(StyleValueType::LITERAL, "scale(0.95)")));
    
    // 生成CSS
    std::cout << "\n=== 生成的CSS ===" << std::endl;
    
    std::string css = manager.generateCSS();
    std::cout << css << std::endl;
    
    // 测试特定选择器的CSS
    std::cout << "\n=== 特定选择器CSS ===" << std::endl;
    
    std::string buttonCSS = manager.generateCSSForSelector("button");
    std::cout << "button选择器CSS:" << std::endl;
    std::cout << buttonCSS << std::endl;
    
    std::cout << "\n=== 样式系统测试完成 ===" << std::endl;
    
    return 0;
}