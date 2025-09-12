#include <iostream>
#include <string>
#include "Compiler/CompilerDispatcher.h"

using namespace CHTL;

int main() {
    std::cout << "=== Complex Compiler Dispatcher Test ===" << std::endl;
    
    // 测试更复杂的CHTL代码
    std::string chtl_code = R"(
        html {
            head {
                title { text { "CHTL Test Page" } }
            }
            body {
                div {
                    text { "Hello from CHTL!" }
                    style {
                        color: red;
                        font-size: 20px;
                    }
                }
            }
        }
    )";
    
    std::cout << "Source CHTL code:" << std::endl;
    std::cout << chtl_code << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    // 创建编译器调度器
    CompilerDispatcher dispatcher;
    dispatcher.setDebugMode(true);
    
    // 编译CHTL代码
    std::cout << "Compiling CHTL code..." << std::endl;
    auto result = dispatcher.compile(chtl_code);
    
    if (result.success) {
        std::cout << "Compilation successful!" << std::endl;
        std::cout << "Generated HTML:" << std::endl;
        std::cout << result.html << std::endl;
        std::cout << "Generated CSS:" << std::endl;
        std::cout << result.css << std::endl;
    } else {
        std::cout << "Compilation failed!" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "Error: " << error << std::endl;
        }
    }
    
    return 0;
}