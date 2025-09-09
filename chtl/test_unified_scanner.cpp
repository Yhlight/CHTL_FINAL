#include <iostream>
#include <string>
#include "scanner/unified_scanner.h"

using namespace chtl;

int main() {
    // 测试CHTL代码，包含style和script块
    std::string test_code = R"(
html
{
    head
    {
        title
        {
            text
            {
                Unified Scanner Test
            }
        }
    }
    body
    {
        div
        {
            class: "container";
            
            style
            {
                .container
                {
                    width: 100%;
                    height: 100vh;
                    background-color: #f0f0f0;
                }
            }
            
            script
            {
                fileloader
                {
                    console.log("This is CHTL JS");
                }
                
                function regularJS() {
                    console.log("This is regular JavaScript");
                }
            }
        }
    }
}
)";

    try {
        std::cout << "=== Testing Unified Scanner ===" << std::endl;
        
        scanner::UnifiedScanner scanner(test_code);
        
        // 扫描代码片段
        std::cout << "\n=== Scanning Fragments ===" << std::endl;
        scanner.print_fragments();
        
        // 处理为CHTL编译器
        std::cout << "\n=== Processing for CHTL Compiler ===" << std::endl;
        std::string chtl_processed = scanner.process_for_chtl_compiler();
        std::cout << "CHTL Processed Content:" << std::endl;
        std::cout << chtl_processed << std::endl;
        
        // 处理为CHTL JS编译器
        std::cout << "\n=== Processing for CHTL JS Compiler ===" << std::endl;
        std::string chtl_js_processed = scanner.process_for_chtl_js_compiler();
        std::cout << "CHTL JS Processed Content:" << std::endl;
        std::cout << chtl_js_processed << std::endl;
        
        // 处理为JS编译器
        std::cout << "\n=== Processing for JS Compiler ===" << std::endl;
        std::string js_processed = scanner.process_for_js_compiler();
        std::cout << "JS Processed Content:" << std::endl;
        std::cout << js_processed << std::endl;
        
        // 处理为CSS编译器
        std::cout << "\n=== Processing for CSS Compiler ===" << std::endl;
        std::string css_processed = scanner.process_for_css_compiler();
        std::cout << "CSS Processed Content:" << std::endl;
        std::cout << css_processed << std::endl;
        
        // 显示占位符信息
        std::cout << "\n=== Placeholder Information ===" << std::endl;
        scanner.print_placeholders();
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}