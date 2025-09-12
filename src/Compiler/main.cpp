#include "Compiler.h"
#include "CHTLParser.h" // For the ParseError
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main() {
    std::string source = R"(
        html {
            head {
                title { text: "My Site"; }
            }
            body {
                div {
                    style {
                        .box {
                            color: blue;
                            font-size: 14px;
                        }
                    }
                }
            }
        }
    )";

    std::cout << "--- CHTL Source ---" << std::endl;
    std::cout << source << std::endl << std::endl;

    std::string html_output;
    try {
        html_output = compile_source(source);
    } catch (const std::exception& e) {
        std::cerr << "Compilation failed: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "--- Generated HTML ---" << std::endl;
    std::cout << html_output;
    std::cout << "----------------------" << std::endl;

    return 0;
}
