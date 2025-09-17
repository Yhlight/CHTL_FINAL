#include "Lexer.h"
#include "Parser.h"
#include "Generator.h"

#include <iostream>
#include <cassert>

void runElementTemplateTest() {
    std::cout << "Running Element Template Test..." << std::endl;

    std::string input = R"(
        [Template] @Element MyCard {
            div {
                class: "card";
                h1 { text { "Title" } }
                p { text { "Content" } }
            }
        }

        body {
            @Element MyCard;
        }
    )";

    std::string expected_html = R"(<body><div class="card"><h1>Title</h1><p>Content</p></div></body>)";

    CHTL::Lexer lexer(input);
    CHTL::Parser parser(lexer);
    auto program = parser.ParseProgram();

    const auto& errors = parser.Errors();
    if (!errors.empty()) {
        std::cerr << "Element Template Test Failed! Parser has " << errors.size() << " errors:" << std::endl;
        for (const auto& msg : errors) {
            std::cerr << "\t" << msg << std::endl;
        }
        assert(false);
    }
    assert(program != nullptr);

    CHTL::Generator generator(*program);
    std::string actual_html = generator.Generate();

    if (actual_html != expected_html) {
        std::cerr << "Element Template Test Failed!" << std::endl;
        std::cerr << "Expected: " << expected_html << std::endl;
        std::cerr << "Got:      " << actual_html << std::endl;
        assert(false);
    }

    std::cout << "Element Template Test Passed!" << std::endl;
}

int main() {
    runElementTemplateTest();
    return 0;
}
