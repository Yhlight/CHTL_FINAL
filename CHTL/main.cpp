#include "CHTLParser/Parser.h"
#include "CHTLGenerator/Generator.h"
#include <iostream>
int main() {
    std::string code = R"(
div {
    style {
        color: red;
        .nested {
            font-weight: bold;
        }
        #main-id {
            border: "1px solid black";
        }
    }
    p { text { "This text is red." } }
    div {
        class: nested;
        text { "This text is red and bold." }
    }
}
)";
    std::cout << "--- CHTL E2E Test: Global + Inline CSS ---\n";
    CHTL::Lexer lexer(code);
    CHTL::Parser parser(lexer);
    auto root = parser.parse_root();
    CHTL::Generator generator;
    std::string html_output = generator.generate(root.get());
    std::cout << html_output;
    return 0;
}
