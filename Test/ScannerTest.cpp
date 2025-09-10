#include "../Scanner/UnifiedScanner.h"
#include <iostream>
#include <vector>

std::string blockTypeToString(CodeBlockType type) {
    switch (type) {
        case CodeBlockType::CHTL: return "CHTL";
        case CodeBlockType::CSS: return "CSS";
        case CodeBlockType::JS: return "JS";
        case CodeBlockType::CHTL_JS: return "CHTL_JS";
        default: return "UNKNOWN";
    }
}

int main() {
    std::string source = R"(
        html {
            head {
                // This is some CHTL
            }
            body {
                div {
                    id = "my-div";
                    -- This is a CHTL comment
                }
            }
        }

        style {
            body {
                background-color: #eee;
            }
        }

        script {
            console.log("Hello from a script block!");
            function greet() {
                return "Hello";
            }
        }

        footer {
            // More CHTL
        }

        script {
            {{.my-button}}->listen {
                click: () => { console.log("Button clicked!"); }
            }
        }
    )";

    std::cout << "--- Starting Unified Scanner Test ---" << std::endl;
    UnifiedScanner scanner(source);
    std::vector<CodeBlock> blocks = scanner.scan();

    std::cout << "--- Scanned Blocks ---" << std::endl;
    for (const auto& block : blocks) {
        std::cout << "--- Block Type: " << blockTypeToString(block.type) << " ---" << std::endl;
        std::cout << block.content << std::endl;
    }

    std::cout << "--- Scanner Test Finished ---" << std::endl;

    return 0;
}
