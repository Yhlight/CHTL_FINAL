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
void test_attributes() { run_test("Phase 1: Element with Attributes", "a { href: \"/home\"; id=link1; }", "<a href=\"/home\" id=\"link1\"></a>"); }
void test_inline_style_generation() { run_test("Phase 2.A: Inline Style", "div{ style{ color:red; font-size:16px; } }", "<div style=\"color: red;font-size: 16px;\"></div>"); }
void test_conditional_true() { run_test("Phase 2.B: Conditional (True)", "div{ style{ width:100px; color: width > 50px ? 'green' : 'red'; } }", "<div style=\"color: green;width: 100px;\"></div>"); }
void test_simple_element_template() { run_test("Phase 3.A: Simple Element Template", "[Template] @Element Box { div{} } body { @Element Box; }", "<body><div></div></body>"); }
void test_nested_element_template() { run_test("Phase 3.A: Nested Element Template", "[Template] @Element Inner{ span{} } [Template] @Element Outer{ div{ @Element Inner; } } body{ @Element Outer; }", "<body><div><span></span></div></body>"); }
void test_simple_style_template() { run_test("Phase 3.B: Simple Style Template", "[Template] @Style Base{ color:red; } div{ style{ @Style Base; } }", "<div style=\"color: red;\"></div>"); }
void test_style_template_override_local_wins() { run_test("Phase 3.B: Style Override (Local Wins)", "[Template] @Style Base{ color:red; } div{ style{ @Style Base; color:blue; } }", "<div style=\"color: blue;\"></div>"); }
void test_style_template_override_base_wins() { run_test("Phase 3.B: Style Override (Base Wins)", "[Template] @Style Base{ color:red; } div{ style{ color:blue; @Style Base; } }", "<div style=\"color: red;\"></div>"); }
void test_custom_element_delete() { run_test("Phase 3.C: Custom Element Delete", "[Custom] @Element MyBox { div{} span{} } body{ @Custom @Element MyBox { delete span; } }", "<body><div></div></body>"); }
void test_custom_style_delete() { run_test("Phase 3.C: Custom Style Delete", "[Custom] @Style MyStyle { color:red; font-size:16px; } div{ style{ @Custom @Style MyStyle{ delete color; } } }", "<div style=\"font-size: 16px;\"></div>"); }

// --- Phase 3.D: `insert` Rule Tests ---
void test_insert_after() {
    std::string src = "[Custom] @Element Box{ div{} } body{ @Custom @Element Box{ insert after div[0]{ p{} } } }";
    run_test("Phase 3.D: Insert After", src, "<body><div></div><p></p></body>");
}
void test_insert_before() {
    std::string src = "[Custom] @Element Box{ div{} } body{ @Custom @Element Box{ insert before div[0]{ p{} } } }";
    run_test("Phase 3.D: Insert Before", src, "<body><p></p><div></div></body>");
}
void test_insert_replace() {
    std::string src = "[Custom] @Element Box{ div{} } body{ @Custom @Element Box{ insert replace div[0]{ p{} } } }";
    run_test("Phase 3.D: Insert Replace", src, "<body><p></p></body>");
}
void test_insert_at_top() {
    std::string src = "[Custom] @Element Box{ div{} } body{ @Custom @Element Box{ insert at top { p{} } } }";
    run_test("Phase 3.D: Insert At Top", src, "<body><p></p><div></div></body>");
}
void test_insert_at_bottom() {
    std::string src = "[Custom] @Element Box{ div{} } body{ @Custom @Element Box{ insert at bottom { p{} } } }";
    run_test("Phase 3.D: Insert At Bottom", src, "<body><div></div><p></p></body>");
}


int main() {
    std::cout << "--- Running CHTL Full Test Suite ---" << std::endl;
    // Keep a subset of tests to keep the log clean, but add all new ones
    test_simple_element();
    test_attributes();
    test_inline_style_generation();
    test_conditional_true();
    test_nested_element_template();
    test_style_template_override_local_wins();
    test_custom_element_delete();
    test_custom_style_delete();
    // New tests for insert
    test_insert_after();
    test_insert_before();
    test_insert_replace();
    test_insert_at_top();
    test_insert_at_bottom();
    std::cout << "------------------------------------" << std::endl;
    return 0;
}
