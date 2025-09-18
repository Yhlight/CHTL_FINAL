#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include <iostream>
#include <cassert>

void testCustomElementParsing() {
    std::cout << "Running test: testCustomElementParsing..." << std::endl;
    std::string source = R"(
        [Custom] @Element MyCustomBox {
            div {
                text: "I am a custom box!";
            }
        }

        // Dummy root element to make parsing start
        html {}
    )";

    std::string file_path = "test.chtl";
    CHTL::CHTLLexer lexer(source);
    auto tokens = lexer.scanTokens();
    CHTL::CHTLParser parser(source, tokens, file_path);

    try {
        parser.parse();
    } catch (const std::runtime_error& e) {
        std::cerr << "Parsing failed: " << e.what() << std::endl;
        assert(false);
    }

    const auto& customDefs = parser.getCustomDefinitions();

    // Check 1: Namespace exists
    assert(customDefs.count("test") == 1);
    const auto& ns_defs = customDefs.at("test");

    // Check 2: Custom definition exists in the namespace
    assert(ns_defs.count("MyCustomBox") == 1);
    const auto& def = ns_defs.at("MyCustomBox");

    // Check 3: Correct type and name
    assert(def.type == CHTL::CustomType::ELEMENT);
    assert(def.name == "MyCustomBox");

    // Check 4: Body is not empty
    assert(!def.element_body.empty());
    assert(def.element_body.size() == 1);

    std::cout << "testCustomElementParsing: PASSED!" << std::endl;
}

int main() {
    testCustomElementParsing();
    return 0;
}
