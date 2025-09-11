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
    std::ifstream file("Test/E2ETest/AdvancedStyles.chtl");
    if (!file) {
        std::cerr << "Failed to open test file 'Test/E2ETest/AdvancedStyles.chtl'" << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    std::cout << "--- CHTL Source (AdvancedStyles) ---" << std::endl;
    std::cout << source << std::endl;
    std::cout << "------------------------------------" << std::endl;

    std::string expected_html = R"(
        <style>
            .inner-text {
                font-size: 14px;
            }
            #main-box:hover {
                border: 1px solid black;
            }
        </style>
        <div id="main-box" width="200" style="height:300px;">
            <p class="inner-text">
                Some text content.
            </p>
        </div>
    )";

    try {
        // 1. Lexer
        CHTL::CHTLLexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.Tokenize();

        // 2. Parser
        CHTL::CHTLParser parser(tokens);
        CHTL::NodeList ast = parser.Parse();

        // 3. Generator
        CHTL::CHTLGenerator generator;
        std::string actual_html = generator.Generate(ast);

        std::cout << "\n--- Generated HTML ---" << std::endl;
        std::cout << actual_html << std::endl;
        std::cout << "----------------------" << std::endl;

        std::cout << "\n--- Test Result ---" << std::endl;

        std::string actual_flat = removeAllWhitespace(actual_html);
        std::string expected_flat = removeAllWhitespace(expected_html);

        assert(actual_flat == expected_flat);

        std::cout << "SUCCESS: Generated HTML matches expected HTML for advanced styles." << std::endl;
        return 0;

    } catch (const std::exception& e) {
        std::cerr << "FAILURE: An exception occurred during compilation: " << e.what() << std::endl;
        return 1;
    }
}
