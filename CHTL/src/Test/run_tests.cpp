#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLNode/DocumentNode.h"

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

// --- Phase 2.A: Style Block Tests ---
void test_inline_style_generation() { run_test("Phase 2.A: Inline Style", "div{ style{ color:red; font-size:16px; } }", "<div style=\"color: red;font-size: 16px;\"></div>"); }
void test_class_selector_generation() { run_test("Phase 2.A: Class Selector", "div{ style{ .box{ width:100px; } } }", "<head><style>.box { width: 100px; }\n</style></head><div class=\"box\"></div>"); }
void test_id_selector_generation() { run_test("Phase 2.A: ID Selector", "div{ style{ #main{ height:100px; } } }", "<head><style>#main { height: 100px; }\n</style></head><div id=\"main\"></div>"); }
void test_pseudo_selector_generation() { run_test("Phase 2.A: Pseudo Selector", "button{ class:btn; style{ &:hover{ background-color:blue; } } }", "<head><style>.btn:hover { background-color: blue; }\n</style></head><button class=\"btn\"></button>"); }
void test_combined_style_features() { run_test("Phase 2.A: Combined Styles", "div{ id:my-div; style{ color:red; .box{ border:1px; } &:hover{ color:blue; } } }", "<head><style>.box { border: 1px; }\n#my-div:hover { color: blue; }\n</style></head><div class=\"box\" id=\"my-div\" style=\"color: red;\"></div>"); }

// --- Phase 2.B: Conditional Expression Tests (with corrected expected output) ---
void test_conditional_true() {
    std::string src = "div{ style{ width:100px; color: width > 50px ? 'green' : 'red'; } }";
    std::string expected = "<div style=\"width: 100px;color: green;\"></div>";
    run_test("Phase 2.B: Conditional (True)", src, expected);
}

void test_conditional_false() {
    std::string src = "div{ style{ width:40px; color: width > 50px ? 'green' : 'red'; } }";
    std::string expected = "<div style=\"width: 40px;color: red;\"></div>";
    run_test("Phase 2.B: Conditional (False)", src, expected);
}

void test_conditional_logical_and() {
    std::string src = "div{ style{ width:60px; height:80px; color: width > 50px && height < 90px ? 'pass' : 'fail'; } }";
    std::string expected = "<div style=\"width: 60px;height: 80px;color: pass;\"></div>";
    run_test("Phase 2.B: Conditional (Logical AND)", src, expected);
}

void test_conditional_logical_or() {
    std::string src = "div{ style{ width:40px; height:80px; color: width > 50px || height < 90px ? 'pass' : 'fail'; } }";
    std::string expected = "<div style=\"width: 40px;height: 80px;color: pass;\"></div>";
    run_test("Phase 2.B: Conditional (Logical OR)", src, expected);
}

int main() {
    std::cout << "--- Running CHTL Full Test Suite ---" << std::endl;
    test_simple_element(); test_element_with_text(); test_nested_elements(); test_attributes();
    test_inline_style_generation(); test_class_selector_generation(); test_id_selector_generation(); test_pseudo_selector_generation(); test_combined_style_features();
    test_conditional_true(); test_conditional_false(); test_conditional_logical_and(); test_conditional_logical_or();
    std::cout << "------------------------------------" << std::endl;
    return 0;
}
