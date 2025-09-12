#include <iostream>
#include "src/CHTL/StyleSystem.h"

int main() {
    std::cout << "Testing CHTL Style System..." << std::endl;
    
    CHTL::StyleSystem styleSystem;
    
    // 测试属性运算
    std::map<std::string, std::string> context;
    context["width"] = "100px";
    context["height"] = "200px";
    
    std::string result1 = styleSystem.processPropertyCalculation("100px + 50px", context);
    std::cout << "Property Calculation (100px + 50px): " << result1 << std::endl;
    
    std::string result2 = styleSystem.processPropertyCalculation("200px * 2", context);
    std::cout << "Property Calculation (200px * 2): " << result2 << std::endl;
    
    // 测试条件表达式
    std::string result3 = styleSystem.processConditionalExpression("width > 50px ? \"red\" : \"blue\"", context);
    std::cout << "Conditional Expression: " << result3 << std::endl;
    
    // 测试属性引用
    context["box.width"] = "100px";
    std::string result4 = styleSystem.processPropertyReference("box.width", context);
    std::cout << "Property Reference (box.width): " << result4 << std::endl;
    
    // 测试响应式值
    context["boxClass"] = "responsive-box";
    std::string result5 = styleSystem.processReactiveValue("$boxClass$", context);
    std::cout << "Reactive Value ($boxClass$): " << result5 << std::endl;
    
    // 测试全局样式
    styleSystem.addGlobalStyle(".test-class { color: red; }");
    styleSystem.addGlobalStyle("#test-id { background: blue; }");
    
    std::string globalStyles = styleSystem.getGlobalStyles();
    std::cout << "Global Styles:" << std::endl;
    std::cout << globalStyles << std::endl;
    
    std::cout << "Style System test completed successfully!" << std::endl;
    
    return 0;
}