#include "Lexer.h"
#include "Parser.h"
#include "Generator.h"

#include <iostream>
#include <cassert>

void runGeneratorTest() {
    std::cout << "Running Generator Test..." << std::endl;

    std::string input = R"(
        html {
            body {
                text { "Hello World" }
            }
        }
    )";

    std::string expected_html = "<html><body>Hello World</body></html>";

    CHTL::Lexer lexer(input);
    CHTL::Parser parser(lexer);
    auto program = parser.ParseProgram();

    // Ensure parser produced a valid AST
    assert(parser.Errors().empty());
    assert(program != nullptr);

    CHTL::Generator generator(*program);
    std::string actual_html = generator.Generate();

    if (actual_html != expected_html) {
        std::cerr << "Generator Test Failed!" << std::endl;
        std::cerr << "Expected: " << expected_html << std::endl;
        std::cerr << "Got:      " << actual_html << std::endl;
        assert(false);
    }

    std::cout << "Generator Test Passed!" << std::endl;
}

int main() {
    runGeneratorTest();
    return 0;
}
