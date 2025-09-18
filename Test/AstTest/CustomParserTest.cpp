#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include <iostream>
#include <cassert>

void testCustomStyleParsing() {
    std::cout << "--- Running Test: testCustomStyleParsing ---" << std::endl;

    std::string source = R"(
        [Custom] @Style MyTestStyle {
            color: red;
            font-size, margin;
            background-color: blue;
        }

        div {}
    )";

    try {
        // 1. Lexing
        CHTL::CHTLLexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();

        // 2. Parsing
        CHTL::CHTLParser parser(source, tokens);
        parser.parse(); // This will parse the declarations

        // 3. Verification
        const auto& custom_defs = parser.getCustomDefinitions();

        assert(custom_defs.count("MyTestStyle") == 1);
        std::cout << "Assertion Passed: Custom definition 'MyTestStyle' found." << std::endl;

        const auto& def = custom_defs.at("MyTestStyle");
        assert(def.type == CHTL::CustomType::STYLE);
        std::cout << "Assertion Passed: Custom definition type is STYLE." << std::endl;

        assert(def.style_properties.size() == 2);
        std::cout << "Assertion Passed: Correct number of valued properties (2)." << std::endl;

        assert(def.valueless_style_properties.size() == 2);
        std::cout << "Assertion Passed: Correct number of valueless properties (2)." << std::endl;

        assert(def.style_properties[0].key == "color");
        assert(def.style_properties[1].key == "background-color");
        std::cout << "Assertion Passed: Valued properties have correct keys." << std::endl;

        assert(def.valueless_style_properties[0] == "font-size");
        assert(def.valueless_style_properties[1] == "margin");
        std::cout << "Assertion Passed: Valueless properties have correct names." << std::endl;

        std::cout << "--- Test Passed ---" << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Test Failed with exception: " << e.what() << std::endl;
    } catch (...) {
        std::cerr << "Test Failed with unknown exception." << std::endl;
    }
}

int main() {
    testCustomStyleParsing();
    return 0;
}
