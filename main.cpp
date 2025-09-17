#include <iostream>
#include <string>
#include <vector>
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLNode/Node.h"

int main() {
    std::string input = R"(
        div {
            style {
                width: 100px;
                height: 200px;
                color: red;
            }
            span {
                text: "Hello";
            }
        }
    )";

    CHTL::Lexer lexer(input);
    CHTL::Parser parser(lexer);

    auto program = parser.parseProgram();

    if (!parser.getErrors().empty()) {
        std::cerr << "--- Parser Errors ---" << std::endl;
        for (const auto& error : parser.getErrors()) {
            std::cerr << error << std::endl;
        }
        return 1;
    }

    if (!program) {
        std::cerr << "Parsing failed, returned null program." << std::endl;
        return 1;
    }

    std::cout << "--- Abstract Syntax Tree ---" << std::endl;
    std::cout << program->toString(0);
    std::cout << "--------------------------" << std::endl;


    CHTL::Generator generator;
    std::string html_output = generator.generate(program.get());

    std::cout << "--- Generated HTML ---" << std::endl;
    std::cout << html_output << std::endl;

    return 0;
}
