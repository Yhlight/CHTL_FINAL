#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLNode/DocumentNode.h"
#include "CHTL/CHTLContext/CHTLContext.h"

void run_test(const std::string& test_name, const std::string& chtl_source, const std::string& expected_html) {
    std::cout << "Running test: " << test_name << " ... ";
    std::string actual_html;
    try {
        CHTL::Lexer lexer(chtl_source);
        auto tokens = lexer.tokenize();
        CHTL::CHTLContext context;
        CHTL::Parser parser(tokens, context);
        auto ast = parser.parse();
        CHTL::Generator generator;
        actual_html = generator.generate(ast.get(), context);
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

// --- Test Cases ---
void test_simple_element() { run_test("Phase 1: Simple Element", "div{}", "<div></div>"); }
void test_element_with_text() { run_test("Phase 1: Element with Text", "p { text { \"hello\" } }", "<p>hello</p>"); }
void test_nested_elements() { run_test("Phase 1: Nested Elements", "div{ p{ span{} } }", "<div><p><span></span></p></div>"); }
void test_attributes() { run_test("Phase 1: Element with Attributes", "a { href: \"/home\"; id=link1; }", "<a href=\"/home\" id=\"link1\"></a>"); }
void test_inline_style_generation() { run_test("Phase 2.A: Inline Style", "div{ style{ color:red; font-size:16px; } }", "<div style=\"color: red;font-size: 16px;\"></div>"); }
void test_class_selector_generation() { run_test("Phase 2.A: Class Selector", "div{ style{ .box{ width:100px; } } }", "<head><style>.box { width: 100px; }\n</style></head><div class=\"box\"></div>"); }
void test_id_selector_generation() { run_test("Phase 2.A: ID Selector", "div{ style{ #main{ height:100px; } } }", "<head><style>#main { height: 100px; }\n</style></head><div id=\"main\"></div>"); }
void test_pseudo_selector_generation() { run_test("Phase 2.A: Pseudo Selector", "button{ class:btn; style{ &:hover{ background-color:blue; } } }", "<head><style>.btn:hover { background-color: blue; }\n</style></head><button class=\"btn\"></button>"); }
void test_combined_style_features() { run_test("Phase 2.A: Combined Styles", "div{ id:my-div; style{ color:red; .box{ border:1px; } &:hover{ color:blue; } } }", "<head><style>.box { border: 1px; }\n#my-div:hover { color: blue; }\n</style></head><div class=\"box\" id=\"my-div\" style=\"color: red;\"></div>"); }
void test_conditional_true() { run_test("Phase 2.B: Conditional (True)", "div{ style{ width:100px; color: width > 50px ? 'green' : 'red'; } }", "<div style=\"color: green;width: 100px;\"></div>"); }
void test_conditional_false() { run_test("Phase 2.B: Conditional (False)", "div{ style{ width:40px; color: width > 50px ? 'green' : 'red'; } }", "<div style=\"color: red;width: 40px;\"></div>"); }
void test_conditional_logical_and() { run_test("Phase 2.B: Conditional (Logical AND)", "div{ style{ width:60px; height:80px; color: width > 50px && height < 90px ? 'pass' : 'fail'; } }", "<div style=\"color: pass;height: 80px;width: 60px;\"></div>"); }
void test_conditional_logical_or() { run_test("Phase 2.B: Conditional (Logical OR)", "div{ style{ width:40px; height:80px; color: width > 50px || height < 90px ? 'pass' : 'fail'; } }", "<div style=\"color: pass;height: 80px;width: 40px;\"></div>"); }
void test_simple_element_template() { run_test("Phase 3.A: Simple Element Template", "[Template] @Element Box { div{} } body { @Element Box; }", "<body><div></div></body>"); }
void test_nested_element_template() { run_test("Phase 3.A: Nested Element Template", "[Template] @Element Inner{ span{} } [Template] @Element Outer{ div{ @Element Inner; } } body{ @Element Outer; }", "<body><div><span></span></div></body>"); }

// --- Phase 3.B: @Style Template Tests ---
void test_simple_style_template() {
    std::string src = "[Template] @Style Base{ color:red; } div{ style{ @Style Base; } }";
    run_test("Phase 3.B: Simple Style Template", src, "<div style=\"color: red;\"></div>");
}
void test_style_template_override_local_wins() {
    std::string src = "[Template] @Style Base{ color:red; } div{ style{ @Style Base; color:blue; } }";
    run_test("Phase 3.B: Style Override (Local Wins)", src, "<div style=\"color: blue;\"></div>");
}
void test_style_template_override_base_wins() {
    std::string src = "[Template] @Style Base{ color:red; } div{ style{ color:blue; @Style Base; } }";
    run_test("Phase 3.B: Style Override (Base Wins)", src, "<div style=\"color: red;\"></div>");
}
void test_style_template_multilevel_inheritance() {
    std::string src = "[Template] @Style A{ color:red; } [Template] @Style B{ @Style A; font-size:16px; } div{ style{ @Style B; } }";
    run_test("Phase 3.B: Multilevel Style Inheritance", src, "<div style=\"color: red;font-size: 16px;\"></div>");
}


int main() {
    std::cout << "--- Running CHTL Full Test Suite ---" << std::endl;
    test_simple_element(); test_element_with_text(); test_nested_elements(); test_attributes();
    test_inline_style_generation(); test_class_selector_generation(); test_id_selector_generation(); test_pseudo_selector_generation(); test_combined_style_features();
    test_conditional_true(); test_conditional_false(); test_conditional_logical_and(); test_conditional_logical_or();
    test_simple_element_template(); test_nested_element_template();
    test_simple_style_template(); test_style_template_override_local_wins(); test_style_template_override_base_wins(); test_style_template_multilevel_inheritance();
    std::cout << "------------------------------------" << std::endl;
    return 0;
}
