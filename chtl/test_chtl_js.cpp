#include <iostream>
#include <string>
#include "chtl_js/chtl_js_compiler.h"

int main() {
    std::cout << "=== CHTL JS编译器测试 ===" << std::endl;
    
    // 测试CHTL JS代码
    std::string chtl_js_code = R"(
// CHTL JS测试代码
fileloader {
    load: "./module1.cjjs",
    load: "./module2.cjjs",
    load: "./module3.js"
}

listen {
    event: "click";
    selector: {{button}};
    handler: function(e) {
        console.log("Button clicked!");
    }
}

animate {
    target: {{.box}};
    duration: 1000;
    easing: ease-in-out;
    loop: 1;
}

vir test = iNeverAway {
    Void<A>: function(x, y) {
        return x + y;
    },
    Void<B>: function(x, y) {
        return x * y;
    }
};

util a > b -> change print("a > b") -> then print("a is greater");

const result = {{selector}}->textContent();
)";

    chtl::chtl_js::CHTLJSCompileOptions options;
    options.enable_selector_auto_add = true;
    options.enable_arrow_syntax = true;
    options.enable_util_expressions = true;
    options.enable_vir_objects = true;
    options.enable_ineveraway = true;
    options.debug_mode = true;
    
    chtl::chtl_js::CHTLJSCompiler compiler(options);
    
    std::cout << "原始CHTL JS代码长度: " << chtl_js_code.length() << " 字符" << std::endl;
    std::cout << "\n原始代码:" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    std::cout << chtl_js_code << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    // 编译CHTL JS代码
    auto result = compiler.compile(chtl_js_code);
    
    if (result.success) {
        std::cout << "\n✓ 编译成功！" << std::endl;
        std::cout << "\n生成的JavaScript代码:" << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        std::cout << result.js_code << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        
        std::cout << "\n编译统计:" << std::endl;
        std::cout << "- 选择器数量: " << result.selectors.size() << std::endl;
        std::cout << "- 依赖数量: " << result.dependencies.size() << std::endl;
        std::cout << "- 错误数量: " << result.errors.size() << std::endl;
        std::cout << "- 警告数量: " << result.warnings.size() << std::endl;
        
        if (!result.selectors.empty()) {
            std::cout << "\n检测到的选择器:" << std::endl;
            for (const auto& selector : result.selectors) {
                std::cout << "  - " << selector << std::endl;
            }
        }
        
        if (!result.dependencies.empty()) {
            std::cout << "\n检测到的依赖:" << std::endl;
            for (const auto& dep : result.dependencies) {
                std::cout << "  - " << dep << std::endl;
            }
        }
        
    } else {
        std::cout << "\n✗ 编译失败！" << std::endl;
        std::cout << "\n错误信息:" << std::endl;
        for (const auto& error : result.errors) {
            std::cout << "  - " << error << std::endl;
        }
    }
    
    return 0;
}