#include "Lexer.h"
#include "Parser.h"
#include "Generator.h"

#include <iostream>
#include <cassert>

void runAttributeTest() {
    std::cout << "Running Attribute Test..." << std::endl;

    std::string input = R"(
        div {
            id: "main";
            class = box; // unquoted literal value

            p {
                text { "Content" }
            }
        }
    )";

    std::string expected_html = R"(<div id="main" class="box"><p>Content</p></div>)";

    CHTL::Lexer lexer(input);
    CHTL::Parser parser(lexer);
    auto program = parser.ParseProgram();

    const auto& errors = parser.Errors();
    if (!errors.empty()) {
        std::cerr << "Attribute Test Failed! Parser has " << errors.size() << " errors:" << std::endl;
        for (const auto& msg : errors) {
            std::cerr << "\t" << msg << std::endl;
        }
        assert(false);
    }
    assert(program != nullptr);

    CHTL::Generator generator(*program);
    std::string actual_html = generator.Generate();

    if (actual_html != expected_html) {
        std::cerr << "Attribute Test Failed!" << std::endl;
        std::cerr << "Expected: " << expected_html << std::endl;
        std::cerr << "Got:      " << actual_html << std::endl;
        assert(false);
    }

    std::cout << "Attribute Test Passed!" << std::endl;
}

int main() {
    runAttributeTest();
    return 0;
}
