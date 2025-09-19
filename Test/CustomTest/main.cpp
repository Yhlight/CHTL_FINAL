#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <string>
#include <algorithm>
#include <cassert>

// Helper function to remove whitespace for easier comparison
std::string removeWhitespace(std::string str) {
    str.erase(std::remove(str.begin(), str.end(), ' '), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\t'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\n'), str.end());
    str.erase(std::remove(str.begin(), str.end(), '\r'), str.end());
    return str;
}


int main() {
    std::string entry_point = "Test/CustomTest/main.chtl";
    std::string source = R"(
        [Namespace] my_ui {
            [Template] @Element Button {
                button {
                    class: "btn";
                    text: "Click Me";
                }
            }

            body {
                @Element Button;
            }
        }
    )";

    CHTL::FileSystem::writeFile(entry_point, source);

    std::cout << "--- Running End-to-End Namespace Test ---" << std::endl;
    std::cout << "--- Input CHTL ---\n" << source << "\n------------------\n" << std::endl;

    std::string expected_html = R"(
        <body>
            <button class="btn">Click Me</button>
        </body>
    )";

    try {
        CHTL::CHTLLexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();
        CHTL::CHTLParser parser(source, tokens, entry_point);
        auto ast = parser.parse();
        CHTL::CHTLGenerator generator(parser.getTemplateDefinitions());
        CHTL::CompilationResult result = generator.generate(ast.get());

        std::cout << "--- Generated HTML ---\n" << result.html << "\n----------------------\n" << std::endl;

        std::string clean_generated = removeWhitespace(result.html);
        std::string clean_expected = removeWhitespace(expected_html);

        if (clean_generated == clean_expected) {
            std::cout << ">>> PASS: End-to-End namespace test passed!" << std::endl;
        } else {
            std::cerr << ">>> FAIL: End-to-End namespace test failed!" << std::endl;
            std::cerr << "  Expected: " << clean_expected << std::endl;
            std::cerr << "  Got:      " << clean_generated << std::endl;
        }
        assert(clean_generated == clean_expected);

    } catch (const std::exception& e) {
        std::cerr << "Caught an exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
