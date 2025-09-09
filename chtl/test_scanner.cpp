#include <iostream>
#include "scanner/unified_scanner.h"

using namespace chtl::scanner;

int main() {
    std::cout << "Testing UnifiedScanner..." << std::endl;
    
    std::string source = R"(
        html {
            head {
                title: "My CHTL Page";
            }
            body {
                div {
                    class: "container";
                    text {
                        "Hello, CHTL!"
                    }
                    style {
                        .container {
                            width: 100%;
                            height: 100vh;
                            background-color: blue;
                        }
                    }
                }
            }
        }
    )";
    
    try {
        UnifiedScanner scanner(source);
        
        std::cout << "Source length: " << source.length() << " characters" << std::endl;
        
        // 扫描代码片段
        auto fragments = scanner.scan();
        std::cout << "Found " << fragments.size() << " fragments" << std::endl;
        
        for (size_t i = 0; i < fragments.size(); ++i) {
            const auto& fragment = fragments[i];
            std::cout << "Fragment " << i << ": " << static_cast<int>(fragment.type) 
                      << " at " << fragment.start_pos << "-" << fragment.end_pos << std::endl;
        }
        
        // 打印占位符信息
        scanner.print_placeholders();
        
        std::cout << "UnifiedScanner test completed successfully!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}