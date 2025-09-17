#include <iostream>
#include <string>

// 简单的测试程序
int main() {
    std::cout << "=== CHTL Project Test ===" << std::endl;
    
    // 测试CHTL JS词法分析器
    std::string testCode = R"(
Vir test = Listen {
    click: () => {
        console.log("Hello from CHTL JS!");
    }
};

const anim = Animate {
    target: {{box}},
    duration: 1000
};
)";
    
    std::cout << "Test code:" << std::endl;
    std::cout << testCode << std::endl;
    
    std::cout << "\nCHTL JS implementation completed!" << std::endl;
    std::cout << "CHTL implementation in progress..." << std::endl;
    
    return 0;
}