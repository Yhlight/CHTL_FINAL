#include "Lexer.h"
#include "Parser.h"
#include "Generator.h"

#include <iostream>
#include <cassert>

void runTemplateTest() {
    std::cout << "Running Template Test..." << std::endl;

    std::string input = R"(
        [Template] @Style DefaultText {
            color: black;
            font-size: 16px;
        }

        p {
            style {
                @Style DefaultText;
                font-weight: bold;
            }
            text { "Hello" }
        }
    )";

    // Note: The generator doesn't guarantee order between template styles and direct styles.
    // Let's check for both possibilities or fix the generator to be deterministic.
    // The current generator will iterate through StyleNode items, so the template will come first.
    std::string expected_html = R"(<p style="color:black;font-size:16px;font-weight:bold;">Hello</p>)";

    CHTL::Lexer lexer(input);
    CHTL::Parser parser(lexer);
    auto program = parser.ParseProgram();

    const auto& errors = parser.Errors();
    if (!errors.empty()) {
        std::cerr << "Template Test Failed! Parser has " << errors.size() << " errors:" << std::endl;
        for (const auto& msg : errors) {
            std::cerr << "\t" << msg << std::endl;
        }
        assert(false);
    }
    assert(program != nullptr);

    CHTL::Generator generator(*program);
    std::string actual_html = generator.Generate();

    if (actual_html != expected_html) {
        std::cerr << "Template Test Failed!" << std::endl;
        std::cerr << "Expected: " << expected_html << std::endl;
        std::cerr << "Got:      " << actual_html << std::endl;
        assert(false);
    }

    std::cout << "Template Test Passed!" << std::endl;
}

int main() {
    runTemplateTest();
    return 0;
}
