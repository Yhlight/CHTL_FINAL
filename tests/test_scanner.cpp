#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cassert>
#include "Scanner/CHTLUnifiedScanner.h"

// Helper to check for substring
bool contains(const std::string& haystack, const std::string& needle) {
    return haystack.find(needle) != std::string::npos;
}

void run_unified_scanner_test() {
    std::cout << "Running Unified Scanner Test..." << std::endl;

    std::ifstream file("tests/unified_scanner_test.chtl");
    if (!file.is_open()) {
        std::cerr << "FATAL: Could not open tests/unified_scanner_test.chtl" << std::endl;
        exit(1);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    CHTL::CHTLUnifiedScanner scanner(source);
    std::vector<CHTL::CodeFragment> fragments = scanner.scan();
    auto placeholder_map = scanner.getPlaceholderMap();

    // --- Assertions ---
    assert(fragments.size() == 7);
    std::cout << "  [PASS] Fragment count is correct (7)." << std::endl;

    assert(fragments[0].type == CHTL::FragmentType::CHTL);
    assert(fragments[1].type == CHTL::FragmentType::CHTL);
    assert(fragments[2].type == CHTL::FragmentType::CHTL);
    assert(fragments[3].type == CHTL::FragmentType::CSS);
    assert(fragments[4].type == CHTL::FragmentType::CHTL);
    assert(fragments[5].type == CHTL::FragmentType::CHTL_JS);
    assert(fragments[6].type == CHTL::FragmentType::CHTL);
    std::cout << "  [PASS] Fragment types are in the correct order." << std::endl;

    assert(contains(fragments[3].content, "body {"));
    assert(contains(fragments[3].content, "#main-content {"));
    std::cout << "  [PASS] CSS fragment contains correct rules." << std::endl;

    const auto& chtl_js_content = fragments[5].content;
    assert(contains(chtl_js_content, "_JS_CODE_PLACEHOLDER_0_"));
    assert(contains(chtl_js_content, "{{element}}->Listen"));
    assert(contains(chtl_js_content, "_JS_CODE_PLACEHOLDER_1_"));
    assert(contains(chtl_js_content, "{{#main-content}}->")); // Corrected Assertion
    assert(contains(chtl_js_content, "_JS_CODE_PLACEHOLDER_2_"));
    std::cout << "  [PASS] CHTL_JS fragment contains placeholders and CHTL JS code." << std::endl;

    assert(placeholder_map.size() == 3);
    assert(contains(placeholder_map.at("_JS_CODE_PLACEHOLDER_0_"), "const element = document.getElementById('main-content');"));
    assert(contains(placeholder_map.at("_JS_CODE_PLACEHOLDER_1_"), "function showMessage(msg) {"));
    assert(contains(placeholder_map.at("_JS_CODE_PLACEHOLDER_2_"), "textContent = \"New Text!\";")); // Check that the JS part is in the placeholder
    std::cout << "  [PASS] Placeholder map is correct." << std::endl;

    assert(contains(fragments[4].content, "[Origin] @Html MyRawHtml"));
    assert(contains(fragments[4].content, "<p>This is a raw HTML paragraph.</p>"));
    std::cout << "  [PASS] [Origin] block is preserved correctly." << std::endl;

    std::cout << "Unified Scanner Test Passed!" << std::endl;
}

int main() {
    try {
        run_unified_scanner_test();
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
