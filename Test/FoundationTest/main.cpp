#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

// A simple utility to remove all whitespace from a string for comparison
std::string removeWhitespace(const std::string& str) {
    std::string out;
    out.reserve(str.length());
    for (char c : str) {
        if (!isspace(c)) {
            out += c;
        }
    }
    return out;
}

struct TestCase {
    std::string name;
    std::string input;
    std::string expected_html;
};

bool runTest(const TestCase& test) {
    std::cout << "--- Running Test: " << test.name << " ---" << std::endl;
    bool success = true;
    std::string generated_html;

    try {
        CHTL::CHTLLexer lexer(test.input);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();
        CHTL::CHTLParser parser(test.input, tokens);
        std::unique_ptr<CHTL::BaseNode> ast = parser.parse();

        if (ast) {
            CHTL::CHTLGenerator generator(parser.getTemplateDefinitions());
            CHTL::CompilationResult result = generator.generate(ast.get());
            generated_html = result.html;
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "!!! TEST FAILED with exception: " << e.what() << std::endl;
        success = false;
    }

    std::string clean_generated = removeWhitespace(generated_html);
    std::string clean_expected = removeWhitespace(test.expected_html);

    if (success && clean_generated != clean_expected) {
        std::cerr << "!!! TEST FAILED: Output mismatch!" << std::endl;
        std::cerr << "Expected: " << test.expected_html << std::endl;
        std::cerr << "Got:      " << generated_html << std::endl;
        success = false;
    }

    if (success) {
        std::cout << ">>> TEST PASSED <<<" << std::endl;
    }

    std::cout << "--------------------------------------" << std::endl << std::endl;
    return success;
}

int main() {
    std::vector<TestCase> tests = {
        {
            "Basic Comment Test",
            R"(
                // To be ignored
                html {
                    # Preserved comment
                }
            )",
            "<html><!-- Preserved comment --></html>"
        },
        {
            "Attribute-style Text (Quoted)",
            R"(p { text: "Hello World"; })",
            "<p>Hello World</p>"
        },
        {
            "Attribute-style Text (Unquoted)",
            R"(p { text: HelloWorld; })",
            "<p>HelloWorld</p>"
        },
        {
            "Block-style Text (Quoted)",
            R"(div { text { "Hello Again" } })",
            "<div>Hello Again</div>"
        },
        {
            "Block-style Text (Unquoted Multi-word)",
            R"(
                div {
                    text { The quick brown fox }
                }
            )",
            "<div>The quick brown fox</div>"
        },
        {
            "Attribute with Hyphenated Value",
            R"(div { class: main-content; })",
            R"(<div class="main-content"></div>)"
        },
        {
            "Empty Text Block",
            R"(div { text {} })",
            R"(<div></div>)"
        },
        {
            "Sequential Siblings (Comment then Element)",
            R"(
                body {
                    # This is a comment
                    p { text: "This should be parsed"; }
                }
            )",
            R"(
                <body>
                    <!-- This is a comment -->
                    <p>This should be parsed</p>
                </body>
            )"
        }
    };

    int passed_count = 0;
    for (const auto& test : tests) {
        if (runTest(test)) {
            passed_count++;
        }
    }

    std::cout << "======================================" << std::endl;
    std::cout << "Foundation Test Summary: " << passed_count << " / " << tests.size() << " passed." << std::endl;
    std::cout << "======================================" << std::endl;

    return (passed_count == tests.size()) ? 0 : 1;
}
