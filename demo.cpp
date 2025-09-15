#include <iostream>
#include <string>
#include "CHTL/CHTLJS/CHTLJS.h"
#include "Scanner/UnifiedScanner.h"

int main() {
    std::cout << "=== CHTL项目演示程序 ===" << std::endl;
    std::cout << "版本: 1.0.0" << std::endl;
    std::cout << "作者: CHTL Team" << std::endl;
    std::cout << "许可证: MIT" << std::endl;
    std::cout << std::endl;
    
    // 测试CHTL JS编译器
    std::cout << "1. 测试CHTL JS编译器" << std::endl;
    std::cout << "========================" << std::endl;
    
    CHTLJS::CHTLJS chtljs;
    chtljs.setDebugMode(true);
    
    std::string chtljsCode = R"(
Vir test = Listen {
    click: () => {
        console.log("Hello from CHTL JS!");
        {{box}}->textContent = "Clicked!";
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
        opacity: 0,
        transform: scale(0.8)
    },
    end: {
        opacity: 1,
        transform: scale(1.0)
    }
};

const router = Router {
    url: "/home",
    page: {{homePage}},
    mode: "history"
};
)";
    
    std::cout << "输入代码:" << std::endl;
    std::cout << chtljsCode << std::endl;
    std::cout << std::endl;
    
    std::string jsResult = chtljs.compile(chtljsCode);
    if (chtljs.hasError()) {
        std::cerr << "编译错误: " << chtljs.getLastError() << std::endl;
    } else {
        std::cout << "编译结果:" << std::endl;
        std::cout << jsResult << std::endl;
    }
    
    std::cout << std::endl;
    
    // 测试统一扫描器
    std::cout << "2. 测试统一扫描器" << std::endl;
    std::cout << "==================" << std::endl;
    
    CHTL::UnifiedScanner scanner;
    scanner.setDebugMode(true);
    
    std::string mixedCode = R"(
html {
    head {
        title {
            text: "CHTL Demo";
        }
    }
    
    body {
        div {
            id: main;
            class: container;
            
            style {
                .container {
                    width: 100%;
                    max-width: 1200px;
                    margin: 0 auto;
                    padding: 20px;
                }
                
                .box {
                    width: 200px;
                    height: 200px;
                    background-color: #f0f0f0;
                    border: 1px solid #ccc;
                    border-radius: 8px;
                    display: flex;
                    align-items: center;
                    justify-content: center;
                    cursor: pointer;
                    transition: all 0.3s ease;
                }
                
                .box:hover {
                    background-color: #e0e0e0;
                    transform: scale(1.05);
                }
            }
            
            div {
                id: box;
                class: box;
                text: "Click me!";
                
                script {
                    {{box}}->Listen {
                        click: () => {
                            console.log("Box clicked!");
                            {{box}}->textContent = "Clicked!";
                        }
                    };
                }
            }
        }
    }
}
)";
    
    std::cout << "输入代码:" << std::endl;
    std::cout << mixedCode << std::endl;
    std::cout << std::endl;
    
    auto fragments = scanner.scan(mixedCode);
    std::cout << "扫描结果:" << std::endl;
    scanner.printFragments();
    scanner.printStatistics();
    
    std::cout << std::endl;
    
    // 显示项目状态
    std::cout << "3. 项目实现状态" << std::endl;
    std::cout << "================" << std::endl;
    std::cout << "✅ CHTL JS核心编译器 - 已完成" << std::endl;
    std::cout << "✅ CHTL核心编译器 - 已完成" << std::endl;
    std::cout << "✅ 统一扫描器 - 已完成" << std::endl;
    std::cout << "🚧 CMOD模块系统 - 进行中" << std::endl;
    std::cout << "⏳ CJMOD模块系统 - 待实现" << std::endl;
    std::cout << "⏳ CJMOD API - 待实现" << std::endl;
    std::cout << "⏳ CLI工具 - 待实现" << std::endl;
    std::cout << "⏳ VSCode扩展 - 待实现" << std::endl;
    std::cout << "⏳ 珂朵莉模块 - 待实现" << std::endl;
    std::cout << "⏳ 由比滨结衣模块 - 待实现" << std::endl;
    
    std::cout << std::endl;
    std::cout << "=== 演示完成 ===" << std::endl;
    
    return 0;
}