#include "Lexer.h"
#include "Parser.h"

#include <iostream>
#include <cassert>

void runErrorTest() {
    std::cout << "Running Error Test..." << std::endl;

    // This input is missing a semicolon after the attribute.
    std::string input = R"(
        div {
            id: "main"
        }
    )";

    CHTL::Lexer lexer(input);
    CHTL::Parser parser(lexer);
    auto program = parser.ParseProgram();

    const auto& errors = parser.Errors();

    if (errors.empty()) {
        std::cerr << "Error Test Failed! Expected parser errors for malformed input, but got none." << std::endl;
        assert(false);
    } else {
        std::cout << "Error Test Passed! Parser correctly detected " << errors.size() << " error(s)." << std::endl;
        // For debugging, print the errors found
        for(const auto& err : errors) {
            std::cout << "  - Found expected error: " << err << std::endl;
        }
    }
}

int main() {
    runErrorTest();
    return 0;
}
