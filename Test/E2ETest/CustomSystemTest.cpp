#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>
#include <algorithm>
#include <cctype>

#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"

// Function to remove all whitespace for robust comparison
std::string removeAllWhitespace(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}

int main() {
    std::ifstream file("Test/E2ETest/CustomSystem.chtl");
    if (!file) {
        std::cerr << "Failed to open test file 'Test/E2ETest/CustomSystem.chtl'" << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    std::cout << "--- CHTL Source (CustomSystem) ---" << std::endl;
    std::cout << source << std::endl;
    std::cout << "------------------------------------" << std::endl;

    std::string expected_html = R"(
        <div style="border:1pxsolidblack;padding:10px;background-color:green;color:white;">
            Button 1
        </div>
        <div style="padding:10px;background-color:blue;color:white;">
            Button 2 (No Border)
        </div>
        <p color="red">
            This text should be red.
        </p>
    )";

    try {
        CHTL::CHTLLexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.Tokenize();
        CHTL::CHTLParser parser(tokens);
        CHTL::NodeList ast = parser.Parse();
        CHTL::CHTLGenerator generator;
        std::string actual_html = generator.Generate(ast);

        std::cout << "\n--- Generated HTML ---" << std::endl;
        std::cout << actual_html << std::endl;
        std::cout << "----------------------" << std::endl;

        std::cout << "\n--- Test Result ---" << std::endl;

        std::string actual_flat = removeAllWhitespace(actual_html);
        std::string expected_flat = removeAllWhitespace(expected_html);

        assert(actual_flat == expected_flat);

        std::cout << "SUCCESS: Generated HTML matches expected HTML for valueless properties." << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "FAILURE: An exception occurred during compilation: " << e.what() << std::endl;
        return 1;
    }
}
