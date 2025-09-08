#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLNode/DocumentNode.h"

int main() {
    std::string source = R"(
        html {
            head {}
            body {
                div {
                    id: my-div; // Manually set ID
                    style {
                        // This should become an inline style
                        color: red;

                        // This should be added to the class attribute and global styles
                        .box {
                            border: 1px solid black;
                        }

                        // This should use the element's context and go to global styles
                        &:hover {
                            border-color: blue;
                        }
                    }
                    text { "Styled Box" }
                }
            }
        }
    )";

    std::cout << "--- CHTL CSS Generation Test ---" << std::endl;
    std::cout << "\n--- Source Code ---\n" << source << std::endl;

    try {
        // 1. Lexing
        CHTL::Lexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.tokenize();

        // 2. Parsing
        CHTL::Parser parser(tokens);
        std::unique_ptr<CHTL::DocumentNode> ast = parser.parse();
        std::cout << "\n--- Lexing and Parsing Successful ---" << std::endl;

        // 3. Generating
        CHTL::Generator generator;
        std::string html = generator.generate(ast.get());
        std::cout << "--- Code Generation Successful ---" << std::endl;

        // 4. Final Output
        std::cout << "\n--- Generated HTML ---\n" << html << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "\n--- COMPILATION FAILED ---\n" << e.what() << std::endl;
        return 1;
    }

    std::cout << "\n--------------------------------" << std::endl;

    return 0;
}
