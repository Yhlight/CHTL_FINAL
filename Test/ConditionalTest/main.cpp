#include "../../CHTL/CHTLCompiler.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include <iostream>
#include <string>
#include <sstream>
#include <algorithm>

// Helper function to remove all whitespace from a string
std::string removeWhitespace(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}

int main() {
    std::string entry_file = "Test/ConditionalTest/test.chtl";
    std::cout << "--- Running Conditional Expression Test: " << entry_file << " ---\n" << std::endl;

    try {
        CHTL::CHTLCompiler compiler;
        std::unique_ptr<CHTL::BaseNode> ast = compiler.compile(entry_file);

        CHTL::CHTLGenerator generator(compiler.getTemplateDefinitions());
        CHTL::CompilationResult result = generator.generate(ast.get());

        // The expected style properties based on the evaluation of the conditional expressions
        std::string expected_style =
            "width:200px;"
            "height:100px;"
            "background-color:red;"
            "border-color:black;"
            "padding:20px;"
            "margin:15px;"
            "outline:2pxsolidred;"  // Note: unquoted strings get concatenated
            "font-size:18px;";

        // Extract the style attribute from the generated HTML
        std::string generated_html = result.html;
        std::string style_key = "style=\"";
        size_t start_pos = generated_html.find(style_key);
        if (start_pos == std::string::npos) {
            std::cerr << "TEST FAILED: Could not find style attribute in generated HTML." << std::endl;
            return 1;
        }
        start_pos += style_key.length();
        size_t end_pos = generated_html.find("\"", start_pos);
        if (end_pos == std::string::npos) {
             std::cerr << "TEST FAILED: Could not find closing quote for style attribute." << std::endl;
            return 1;
        }

        std::string generated_style = generated_html.substr(start_pos, end_pos - start_pos);

        // Normalize both strings by removing whitespace to make comparison robust
        std::string normalized_generated = removeWhitespace(generated_style);
        std::string normalized_expected = removeWhitespace(expected_style);

        std::cout << "Generated Style: " << generated_style << std::endl;
        std::cout << "Expected Style (Normalized): " << normalized_expected << std::endl;

        if (normalized_generated == normalized_expected) {
            std::cout << "\n--- Conditional Expression Test PASSED ---" << std::endl;
        } else {
            std::cerr << "\n--- Conditional Expression Test FAILED ---\n" << std::endl;
            std::cerr << "Normalized Generated: " << normalized_generated << std::endl;
            std::cerr << "Normalized Expected:  " << normalized_expected << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Caught an exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
