#include "CHTL/CHTLJS/CHTLJS.h"
#include <iostream>
#include <string>

int main() {
    try {
        // 创建CHTL JS实例
        CHTLJS::CHTLJS chtljs;
        
        // 设置调试模式
        chtljs.setDebugMode(true);
        chtljs.setMinifyOutput(false);
        
        // 测试代码
        std::string testCode = R"(
Vir test = Listen {
    click: () => {
        console.log("Button clicked!");
    },
    mouseenter: () => {
        console.log("Mouse entered!");
    }
};

const anim = Animate {
    target: {{box}},
    duration: 1000,
    easing: ease-in-out,
    begin: {
        opacity: 0
    },
    end: {
        opacity: 1
    }
};

const router = Router {
    url: "/home",
    page: {{homePage}}
};
)";
        
        std::cout << "=== CHTL JS Test ===" << std::endl;
        std::cout << "Input code:" << std::endl;
        std::cout << testCode << std::endl;
        std::cout << std::endl;
        
        // 编译代码
        std::string result = chtljs.compile(testCode);
        
        if (chtljs.hasError()) {
            std::cerr << "Error: " << chtljs.getLastError() << std::endl;
            return 1;
        }
        
        std::cout << "Compiled JavaScript:" << std::endl;
        std::cout << result << std::endl;
        
        // 打印上下文信息
        chtljs.printContext();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Exception: " << e.what() << std::endl;
        return 1;
    }
}