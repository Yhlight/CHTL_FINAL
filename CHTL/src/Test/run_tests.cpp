#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>

#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLLoader/CHTLLoader.h"
#include "CHTL/CHTLNode/DocumentNode.h"
#include "CHTL/CHTLContext/CHTLContext.h"

// New test runner for multi-file projects
void run_multi_file_test(
    const std::string& test_name,
    const std::map<std::string, std::string>& file_system,
    const std::string& entry_point,
    const std::string& expected_html
) {
    std::cout << "Running test: " << test_name << " ... ";
    std::string actual_html;
    try {
        CHTL::CHTLContext context;

        // Create a file provider that reads from our map
        CHTL::CHTLLoader::FileProvider file_provider =
            [&file_system](const std::string& path) -> std::string {
            if (file_system.count(path)) {
                return file_system.at(path);
            }
            throw std::runtime_error("File not found in mock filesystem: " + path);
        };

        CHTL::CHTLLoader loader(file_provider);
        auto ast = loader.loadAndParse(entry_point, context);

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

// Keep the old run_test for simplicity, just adapt it to the new runner
void run_test(const std::string& test_name, const std::string& chtl_source, const std::string& expected_html) {
    std::map<std::string, std::string> file_system;
    file_system["test.chtl"] = chtl_source;
    run_multi_file_test(test_name, file_system, "test.chtl", expected_html);
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
void test_insert_after() { std::string src = "[Custom] @Element Box{ div{} } body{ @Custom @Element Box{ insert after div[0]{ p{} } } }"; run_test("Phase 3.D: Insert After", src, "<body><div></div><p></p></body>"); }
void test_insert_before() { std::string src = "[Custom] @Element Box{ div{} } body{ @Custom @Element Box{ insert before div[0]{ p{} } } }"; run_test("Phase 3.D: Insert Before", src, "<body><p></p><div></div></body>"); }
void test_insert_replace() { std::string src = "[Custom] @Element Box{ div{} } body{ @Custom @Element Box{ insert replace div[0]{ p{} } } }"; run_test("Phase 3.D: Insert Replace", src, "<body><p></p></body>"); }
void test_insert_at_top() { std::string src = "[Custom] @Element Box{ div{} } body{ @Custom @Element Box{ insert at top { p{} } } }"; run_test("Phase 3.D: Insert At Top", src, "<body><p></p><div></div></body>"); }
void test_insert_at_bottom() { std::string src = "[Custom] @Element Box{ div{} } body{ @Custom @Element Box{ insert at bottom { p{} } } }"; run_test("Phase 3.D: Insert At Bottom", src, "<body><div></div><p></p></body>"); }
void test_namespaced_template() { run_test("Phase 4.A: Namespaced Template Usage", "[Namespace] MyUI { [Template] @Element Button { button{} } } body{ @Element MyUI::Button; }", "<body><button></button></body>"); }

void test_multi_file_import() {
    std::map<std::string, std::string> fs;
    fs["/project/main.chtl"] = "[Import] @Chtl from \"./lib.chtl\" as MyLib; body{ @Element MyLib::Card; }";
    fs["/project/lib.chtl"] = "[Namespace] MyLib { [Template] @Element Card { div{} } }";
    run_multi_file_test("Phase 4.B: Multi-file Import", fs, "/project/main.chtl", "<body><div></div></body>");
}

void test_variable_template() {
    run_test("Phase 5.A: Variable Template Usage", "[Template] @Var MyTheme { primary: 'red'; } div { style { color: MyTheme(primary); } }", "<div style=\"color: red;\"></div>");
}

void test_origin_blocks() {
    run_test("Phase 5.B: Immediate Origin Block", "body { [Origin] @Html { <p>raw html</p> } }", "<body> <p>raw html</p> </body>");
    std::string named_origin_src = "[Origin] @Html MyRawBlock { <span>Raw Span</span> } body{ [Origin] @MyRawBlock; }";
    run_test("Phase 5.B: Named Origin Block Usage", named_origin_src, "<body> <span>Raw Span</span> </body>");
}

void test_advanced_conditionals() {
    std::string src = "div { id: box; style { width: 100px; } } div { style { height: #box.width > 50px ? 80px : 30px; } }";
    run_test("Phase 5.C: Advanced Conditional Expression", src, "<div id=\"box\" style=\"width: 100px;\"></div><div style=\"height: 80px;\"></div>");
}

void test_text_node() { run_test("Phase 1: Text Node", "div{ text{ \"Hello World\" } }", "<div>Hello World</div>"); }
void test_text_node_unquoted() { run_test("Phase 1: Unquoted Text Node", "div{ text{ Hello World } }", "<div>Hello World</div>"); }
void test_text_attribute() { run_test("Phase 1: Text Attribute", "div{ text: \"Hello Again\"; }", "<div>Hello Again</div>"); }

void test_comments() {
    run_test("Phase 6: Single-line Comment", "div{ \n// this is a comment\n }", "<div></div>");
    run_test("Phase 6: End-of-line Comment", "div{} // another comment", "<div></div>");
    run_test("Phase 6: Multi-line Comment", "div{ /* comment \n goes here */ }", "<div></div>");
    run_test("Phase 6: Generator Comment", "div{ -- this becomes an html comment }", "<div><!-- this becomes an html comment --></div>");
}


int main() {
    std::cout << "--- Running CHTL Full Test Suite ---" << std::endl;
    test_simple_element();
    test_text_node();
    test_text_node_unquoted();
    test_text_attribute();
    test_attributes();
    test_inline_style_generation();
    test_conditional_true();
    test_nested_element_template();
    test_style_template_override_local_wins();
    test_custom_element_delete();
    test_custom_style_delete();
    test_insert_after();
    test_insert_before();
    test_insert_replace();
    test_insert_at_top();
    test_insert_at_bottom();
    test_namespaced_template();
    test_multi_file_import();
    test_variable_template();
    test_origin_blocks();
    test_advanced_conditionals();
    test_comments();
    std::cout << "------------------------------------" << std::endl;
    return 0;
}
