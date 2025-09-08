#include <iostream>
#include <string>
#include <vector>
#include <cassert>
#include <stdexcept>

// Corrected include paths, relative to the `src` directory
// which is specified in CMakeLists.txt
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLNode/DocumentNode.h"

// A simple helper to run a test case and report the result.
void run_test(const std::string& test_name, const std::string& chtl_source, const std::string& expected_html) {
    std::cout << "Running test: " << test_name << " ... ";
    std::string actual_html;
    try {
        // 1. Lex
        CHTL::Lexer lexer(chtl_source);
        auto tokens = lexer.tokenize();

        // 2. Parse
        CHTL::Parser parser(tokens);
        auto ast = parser.parse();

        // 3. Generate
        CHTL::Generator generator;
        actual_html = generator.generate(ast.get());

    } catch (const std::exception& e) {
        std::cerr << "\n  [FAIL] Test crashed with exception: " << e.what() << std::endl;
        return;
    }

    // 4. Assert
    if (actual_html == expected_html) {
        std::cout << "[PASS]" << std::endl;
    } else {
        std::cout << "\n  [FAIL]\n";
        std::cout << "    Expected: " << expected_html << "\n";
        std::cout << "    Actual  : " << actual_html << std::endl;
    }
}

void test_simple_element() {
    run_test("Simple Element", "div{}", "<div></div>");
}

void test_element_with_text() {
    run_test("Element with Text", "p { text { \"hello\" } }", "<p>hello</p>");
}

void test_nested_elements() {
    run_test("Nested Elements", "div{ p{ span{} } }", "<div><p><span></span></p></div>");
}

void test_attributes() {
    run_test("Element with Attributes", "a { href: \"/home\"; id=link1; }", "<a href=\"/home\" id=\"link1\"></a>");
}

void test_full_document() {
    std::string source = R"(
        html {
            body {
                h1 { text { "Title" } }
            }
        }
    )";
    run_test("Full Document", source, "<html><body><h1>Title</h1></body></html>");
}


int main() {
    std::cout << "--- Running CHTL Initial Test Suite ---" << std::endl;
    test_simple_element();
    test_element_with_text();
    test_nested_elements();
    test_attributes();
    test_full_document();
    std::cout << "---------------------------------------" << std::endl;
    return 0;
}
