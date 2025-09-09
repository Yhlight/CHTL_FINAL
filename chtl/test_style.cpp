#include <iostream>
#include "style/style_processor.h"

using namespace chtl::style;

int main() {
    std::cout << "Testing StyleProcessor..." << std::endl;
    
    try {
        StyleProcessor processor;
        
        // 设置上下文
        processor.set_context_attribute("width", "100px");
        processor.set_context_attribute("height", "200px");
        processor.push_element("div");
        processor.push_element("container");
        
        // 创建样式规则
        StyleRule rule(".container");
        rule.attributes.push_back(StyleAttribute("width", "100%"));
        rule.attributes.push_back(StyleAttribute("height", "@height"));  // 引用属性
        rule.attributes.push_back(StyleAttribute("background-color", "blue"));
        
        processor.add_rule(rule);
        
        // 测试表达式求值
        std::string result = ExpressionEvaluator::add("10", "20");
        std::cout << "10 + 20 = " << result << std::endl;
        
        std::string ref_result = ExpressionEvaluator::resolve_attribute_reference("@width", processor.get_context());
        std::cout << "Reference @width = " << ref_result << std::endl;
        
        // 生成CSS
        std::string css = processor.generate_css();
        std::cout << "Generated CSS:\n" << css << std::endl;
        
        // 打印调试信息
        processor.print_rules();
        processor.print_context();
        
        std::cout << "StyleProcessor test completed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}