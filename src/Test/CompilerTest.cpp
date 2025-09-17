#include "../CHTL/Lexer/Lexer.h"
#include "../CHTL/Parser/Parser.h"
#include "../CHTL/Generator/Generator.h"
#include "AstPrinter.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <regex>

// Helper to remove whitespace for easier comparison
std::string removeWhitespace(const std::string& str) {
    std::string out = str;
    out.erase(std::remove(out.begin(), out.end(), ' '), out.end());
    out.erase(std::remove(out.begin(), out.end(), '\n'), out.end());
    return out;
}

void testTemplateInheritance() {
    std::cout << "--- Testing Template Inheritance ---\n";
    std::string source = R"(
        [Template] @Style BaseStyle
        {
            color: blue;
            font-weight: bold;
        }

        [Template] @Style FullStyle
        {
            @Style BaseStyle;
            font-size: 20px;
            color: red; // This should override the color from BaseStyle
        }

        div {
            style {
                @Style FullStyle;
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

    // Verification
    // The final style should have properties from both, with 'color' overridden.
    // CSS cascading means the last 'color' property (red) will be applied.
    std::string expected_style_substr1 = "color:blue;";
    std::string expected_style_substr2 = "font-weight:bold;";
    std::string expected_style_substr3 = "font-size:20px;";
    std::string expected_style_substr4 = "color:red;";

    std::string clean_html = removeWhitespace(html);

    assert(clean_html.find(expected_style_substr1) != std::string::npos);
    assert(clean_html.find(expected_style_substr2) != std::string::npos);
    assert(clean_html.find(expected_style_substr3) != std::string::npos);
    assert(clean_html.find(expected_style_substr4) != std::string::npos);

    // Check that the override happened correctly (red appears after blue)
    assert(clean_html.find("color:blue;") < clean_html.find("color:red;"));


    std::cout << "Template inheritance test passed.\n";
    std::cout << "--------------------------------\n\n";
}


int main() {
    testTemplateInheritance();
    return 0;
}
