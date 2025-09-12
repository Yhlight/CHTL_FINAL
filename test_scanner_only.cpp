#include <iostream>
#include <string>
#include "Scanner/UnifiedScanner.h"

using namespace CHTL;

int main() {
    std::cout << "=== Test Scanner Only ===" << std::endl;
    
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
    
    // 创建统一扫描器
    UnifiedScanner scanner;
    
    try {
        std::cout << "Calling scanner.scan()..." << std::endl;
        auto fragments = scanner.scan(chtl_code);
        std::cout << "Scanner completed!" << std::endl;
        
        std::cout << "Found " << fragments.size() << " fragments:" << std::endl;
        for (size_t i = 0; i < fragments.size(); i++) {
            const auto& fragment = fragments[i];
            std::cout << "  " << i << ": type=" << static_cast<int>(fragment.type) 
                      << ", content='" << fragment.content << "'" << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cout << "Exception: " << e.what() << std::endl;
    }
    
    std::cout << std::string(50, '=') << std::endl;
    std::cout << "Test Scanner Only Complete!" << std::endl;
    
    return 0;
}