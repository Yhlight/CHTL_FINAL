#include "../../Scanner/UnifiedScanner.h"
#include <iostream>
#include <cassert>
#include <string>
#include <map>

// Helper function to print test results
void run_test(void (*test_function)(), const std::string& test_name) {
    std::cout << "Running test: " << test_name << "..." << std::endl;
    test_function();
    std::cout << test_name << " PASSED." << std::endl << std::endl;
}

void test_simple_js_block() {
    UnifiedScanner scanner;
    std::string input = "script { const a = 1; }";
    std::string result = scanner.scan(input);
    const auto& map = scanner.getPlaceholderMap();
    assert(map.size() == 1);
    assert(map.at("_JS_CODE_PLACEHOLDER_0_") == "const a = 1;");
    assert(result == "script { _JS_CODE_PLACEHOLDER_0_ }");
}

void test_simple_chtljs_selector() {
    UnifiedScanner scanner;
    std::string input = "script { {{box}} }";
    std::string result = scanner.scan(input);
    assert(scanner.getPlaceholderMap().empty());
    assert(result == "script { {{box}} }");
}

void test_mixed_js_and_chtljs() {
    UnifiedScanner scanner;
    std::string input = "script { const a = {{box}}; }";
    std::string result = scanner.scan(input);
    const auto& map = scanner.getPlaceholderMap();
    assert(map.size() == 2);
    assert(map.at("_JS_CODE_PLACEHOLDER_0_") == "const a = ");
    assert(map.at("_JS_CODE_PLACEHOLDER_1_") == ";");
    assert(result == "script { _JS_CODE_PLACEHOLDER_0_{{box}}_JS_CODE_PLACEHOLDER_1_ }");
}

void test_chtljs_block() {
    UnifiedScanner scanner;
    std::string input = "script { Listen { click: 1 } }";
    std::string result = scanner.scan(input);
    const auto& map = scanner.getPlaceholderMap();
    assert(map.size() == 1);
    assert(map.at("_JS_CODE_PLACEHOLDER_0_") == "click: 1");
    assert(result == "script { Listen { _JS_CODE_PLACEHOLDER_0_ } }");
}

void test_nested_mixed_content() {
    UnifiedScanner scanner;
    std::string input = "script { function myFunc() { Listen { action: {{btn}} } } }";
    std::string result = scanner.scan(input);
    const auto& map = scanner.getPlaceholderMap();

    assert(map.size() == 3);
    assert(map.at("_JS_CODE_PLACEHOLDER_0_") == "function myFunc() {");
    assert(map.at("_JS_CODE_PLACEHOLDER_1_") == "action: ");
    assert(map.at("_JS_CODE_PLACEHOLDER_2_") == "}");
    assert(result == "script {  _JS_CODE_PLACEHOLDER_0_ Listen {  _JS_CODE_PLACEHOLDER_1_{{btn}}  } _JS_CODE_PLACEHOLDER_2_  }");
}

void test_style_origin_block() {
    UnifiedScanner scanner;
    std::string input = "style { color: red; [Origin] @Html { <div></div> } font-size: 16px; }";
    std::string result = scanner.scan(input);
    const auto& map = scanner.getPlaceholderMap();
    assert(map.size() == 3);
    assert(map.count("_CHTL_CODE_PLACEHOLDER_0_"));
    assert(map.count("_CHTL_CODE_PLACEHOLDER_1_"));
    assert(map.count("_CHTL_CODE_PLACEHOLDER_2_"));
    assert(map.at("_CHTL_CODE_PLACEHOLDER_1_").find("[Origin]") != std::string::npos);
}


int main() {
    run_test(test_simple_js_block, "test_simple_js_block");
    run_test(test_simple_chtljs_selector, "test_simple_chtljs_selector");
    run_test(test_mixed_js_and_chtljs, "test_mixed_js_and_chtljs");
    run_test(test_chtljs_block, "test_chtljs_block");
    run_test(test_nested_mixed_content, "test_nested_mixed_content");
    run_test(test_style_origin_block, "test_style_origin_block");

    std::cout << "All scanner tests passed!" << std::endl;
    return 0;
}
