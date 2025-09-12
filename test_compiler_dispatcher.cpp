#include <iostream>
#include <string>
#include "Compiler/CompilerDispatcher.h"
#include "Compiler/CodeMerger.h"

using namespace CHTL;

int main() {
    std::cout << "=== CHTL Compiler Dispatcher Test ===" << std::endl;
    
    // 测试CHTL代码
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
    dispatcher.setOutputOptions(false, false, false, true);
    dispatcher.setDebugMode(true);
    
    // 编译CHTL代码
    std::cout << "1. Compiling CHTL code..." << std::endl;
    auto result = dispatcher.compile(chtl_code);
    
    if (result.success) {
        std::cout << "Compilation successful!" << std::endl;
        std::cout << "Generated HTML:" << std::endl;
        std::cout << result.html << std::endl;
        std::cout << "Generated CSS:" << std::endl;
        std::cout << result.css << std::endl;
        std::cout << "Generated JavaScript:" << std::endl;
        std::cout << result.javascript << std::endl;
    } else {
        std::cout << "Compilation failed!" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "Error: " << error << std::endl;
        }
    }
    
    std::cout << std::string(50, '=') << std::endl;
    
    // 测试CHTL JS代码
    std::string chtl_js_code = R"(
        listen {
            click: function() { console.log("Clicked!"); }
        }
        
        animate {
            duration: 1000,
            easing: "ease-in-out"
        }
    )";
    
    std::cout << "Source CHTL JS code:" << std::endl;
    std::cout << chtl_js_code << std::endl;
    std::cout << std::string(50, '=') << std::endl;
    
    // 编译CHTL JS代码
    std::cout << "2. Compiling CHTL JS code..." << std::endl;
    auto js_result = dispatcher.compile(chtl_js_code);
    
    if (js_result.success) {
        std::cout << "CHTL JS compilation successful!" << std::endl;
        std::cout << "Generated JavaScript:" << std::endl;
        std::cout << js_result.javascript << std::endl;
    } else {
        std::cout << "CHTL JS compilation failed!" << std::endl;
        for (const auto& error : js_result.errors) {
            std::cout << "Error: " << error << std::endl;
        }
    }
    
    std::cout << std::string(50, '=') << std::endl;
    
    // 测试代码合并器
    std::cout << "3. Testing Code Merger..." << std::endl;
    CodeMerger merger;
    merger.setMinify(false);
    merger.setAddComments(true);
    
    std::vector<CodeFragment> fragments = {
        CodeFragment("HTML", result.html, 1, "test.chtl", 1),
        CodeFragment("CSS", result.css, 1, "test.chtl", 1),
        CodeFragment("JS", js_result.javascript, 1, "test.chtl", 1)
    };
    
    auto merge_result = merger.merge(fragments);
    
    std::cout << "Merged HTML:" << std::endl;
    std::cout << merge_result.html << std::endl;
    std::cout << "Merged CSS:" << std::endl;
    std::cout << merge_result.css << std::endl;
    std::cout << "Merged JavaScript:" << std::endl;
    std::cout << merge_result.javascript << std::endl;
    
    // 生成完整的HTML文档
    std::cout << "4. Generating complete HTML document..." << std::endl;
    std::string complete_html = merger.generateCompleteHTML(merge_result.html, merge_result.css, merge_result.javascript);
    std::cout << "Complete HTML document:" << std::endl;
    std::cout << complete_html << std::endl;
    
    // 显示编译统计信息
    std::cout << "5. Compilation statistics:" << std::endl;
    auto stats = dispatcher.getCompileStats();
    for (const auto& stat : stats) {
        std::cout << "  " << stat.first << ": " << stat.second << std::endl;
    }
    
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "CHTL Compiler Dispatcher Test Complete!" << std::endl;
    
    return 0;
}