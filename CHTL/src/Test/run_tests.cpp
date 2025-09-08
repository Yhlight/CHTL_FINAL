#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLNode/DocumentNode.h"

// A simple helper to run a test case and report the result.
void run_test(const std::string& test_name, const std::string& chtl_source, const std::string& expected_html) {
    std::cout << "Running test: " << test_name << " ... ";
    std::string actual_html;
    try {
        CHTL::Lexer lexer(chtl_source);
        auto tokens = lexer.tokenize();
        CHTL::Parser parser(tokens);
        auto ast = parser.parse();
        CHTL::Generator generator;
        actual_html = generator.generate(ast.get());
    } catch (const std::exception& e) {
        std::cout << "\n  [FAIL] Test crashed with exception: " << e.what() << std::endl;
        return;
    }

    if (actual_html == expected_html) {
        std::cout << "[PASS]" << std::endl;
    } else {
        std::cout << "\n  [FAIL]\n";
        std::cout << "    Expected: " << expected_html << "\n";
        std::cout << "    Actual  : " << actual_html << std::endl;
    }
}

// --- Phase 1 Tests ---
void test_simple_element() { run_test("Phase 1: Simple Element", "div{}", "<div></div>"); }
void test_element_with_text() { run_test("Phase 1: Element with Text", "p { text { \"hello\" } }", "<p>hello</p>"); }
void test_nested_elements() { run_test("Phase 1: Nested Elements", "div{ p{ span{} } }", "<div><p><span></span></p></div>"); }
void test_attributes() { run_test("Phase 1: Element with Attributes", "a { href: \"/home\"; id=link1; }", "<a href=\"/home\" id=\"link1\"></a>"); }

// --- Phase 2: Style Block Tests ---
void test_inline_style_generation() {
    std::string src = "div{ style{ color:red; font-size:16px; } }";
    std::string expected = "<div><div style=\"color: red;font-size: 16px;\"></div></div>"; // The generator creates an outer div, which is a slight bug to fix later. Let's test the current behavior.
    // The test reveals a bug! The generator wraps the output. Let's fix the test to expect the current (buggy) output, and I'll fix the bug later.
    // The source is `div{...}`, so the output should be a single div.
    // Let's re-examine the generator logic. It iterates through document children.
    // The source `div{}` should produce `<div></div>`. Okay, my expected output was wrong.
    expected = "<div style=\"color: red;font-size: 16px;\"></div>";
    run_test("Phase 2: Inline Style Generation", src, expected);
}

void test_class_selector_generation() {
    std::string src = "div{ style{ .box{ width:100px; } } }";
    std::string expected = "<head><style>.box { width: 100px; }\n</style></head><div class=\"box\"></div>";
    run_test("Phase 2: Class Selector", src, expected);
}

void test_id_selector_generation() {
    std::string src = "div{ style{ #main{ height:100px; } } }";
    std::string expected = "<head><style>#main { height: 100px; }\n</style></head><div id=\"main\"></div>";
    run_test("Phase 2: ID Selector", src, expected);
}

void test_pseudo_selector_generation() {
    std::string src = "button{ class:btn; style{ &:hover{ background-color:blue; } } }";
    std::string expected = "<head><style>.btn:hover { background-color: blue; }\n</style></head><button class=\"btn\"></button>";
    run_test("Phase 2: Pseudo Selector", src, expected);
}

void test_combined_style_features() {
    std::string src = "div{ id:my-div; style{ color:red; .box{ border:1px; } &:hover{ color:blue; } } }";
    std::string expected = "<head><style>.box { border: 1px; }\n#my-div:hover { color: blue; }\n</style></head><div class=\"box\" id=\"my-div\" style=\"color: red;\"></div>";
    run_test("Phase 2: Combined Style Features", src, expected);
}


int main() {
    std::cout << "--- Running CHTL Full Test Suite ---" << std::endl;
    // Phase 1
    test_simple_element();
    test_element_with_text();
    test_nested_elements();
    test_attributes();
    // Phase 2
    test_inline_style_generation();
    test_class_selector_generation();
    test_id_selector_generation();
    test_pseudo_selector_generation();
    test_combined_style_features();
    std::cout << "------------------------------------" << std::endl;
    return 0;
}
