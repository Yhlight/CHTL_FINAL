#include "../CHTL/Lexer/Lexer.h"
#include "../CHTL/Parser/Parser.h"
#include "../CHTL/Generator/Generator.h"
#include "AstPrinter.h"
#include <iostream>
#include <vector>
#include <cassert>

void testTemplateExpansion() {
    std::cout << "--- Testing Template Expansion ---\n";
    std::string source = R"(
        [Template] @Style DefaultText
        {
            color: black;
        }

        div {
            style {
                @Style DefaultText;
            }
        }
    )";

    CHTL::Lexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.getAllTokens();
    CHTL::Parser parser(tokens);
    std::shared_ptr<CHTL::RootNode> ast = parser.parse();
    CHTL::Generator generator(ast);
    generator.generate();
    std::string html = generator.getHtml();
    std::cout << "Generated HTML:\n" << html << std::endl;

    std::string expected_style = "style=\"color: black;\"";
    assert(html.find(expected_style) != std::string::npos);
    std::cout << "Template expansion test passed.\n";
    std::cout << "--------------------------------\n\n";
}

void testNonExistentTemplate() {
    std::cout << "--- Testing Non-Existent Template Usage ---\n";
    std::string source = R"(
        div {
            style {
                @Style NonExistentTemplate;
            }
        }
    )";

    CHTL::Lexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.getAllTokens();
    CHTL::Parser parser(tokens);
    std::shared_ptr<CHTL::RootNode> ast = parser.parse();
    CHTL::Generator generator(ast);
    generator.generate();
    std::string html = generator.getHtml();

    std::cout << "Generated HTML:\n" << html << std::endl;

    // The generator should ignore the missing template, resulting in no style attribute.
    assert(html.find("style=") == std::string::npos);
    std::cout << "Non-existent template test passed.\n";
    std::cout << "--------------------------------\n\n";
}


int main() {
    // Clear the registry before each run to ensure tests are isolated.
    // (This would be better with a proper test framework like gtest,
    // but for now, we can't re-run the executable, so it's fine).
    testTemplateExpansion();
    testNonExistentTemplate();
    return 0;
}
