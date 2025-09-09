#include "../../CHTL/CHTLLexer/Lexer.h"
#include "../../CHTL/CHTLParser/Parser.h"
#include "../../CHTL/CHTLGenerator/Generator.h"
#include <iostream>
#include <vector>

void runPipelineTest(const std::string& source) {
    std::cout << "--- CHTL Source ---" << std::endl;
    std::cout << source << std::endl;
    std::cout << "--------------------" << std::endl;

    try {
        // 1. Lexer
        CHTL::Lexer lexer(source);
        auto tokens = lexer.tokenize();

        // 2. Parser
        CHTL::Parser parser(tokens);
        CHTL::DocumentNode ast = parser.parse();

        // 3. Generator
        CHTL::Generator generator;
        std::string html_output = generator.generate(ast);

        std::cout << "--- Generated HTML ---" << std::endl;
        std::cout << html_output << std::endl;
        std::cout << "----------------------" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }
    std::cout << "\n\n";
}

int main() {
    std::string sample = R"(
        div {
            id: "main";
            class = container;

            h1 {
                text { "Welcome to CHTL" }
            }

            p {
                style {
                    color: red;
                    font-size: 16px;
                }
                text { "This text should be red." }
            }
        }
    )";

    runPipelineTest(sample);

    return 0;
}
