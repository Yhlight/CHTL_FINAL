#include "../../CHTL/Scanner/CHTLUnifiedScanner.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <cassert>
#include <vector>
#include <string>

// Helper function to check if a string contains a substring
bool contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

int main() {
    std::ifstream file("Test/unified_scanner_test.chtl");
    if (!file.is_open()) {
        std::cerr << "Failed to open test file." << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    std::cout << "--- Running Scanner Test ---" << std::endl;

    CHTL::CHTLUnifiedScanner scanner(source);
    std::vector<CHTL::CodeFragment> fragments = scanner.scan();
    const auto& placeholder_map = scanner.getPlaceholderMap();

    // 1. Assertions on Fragments
    assert(fragments.size() == 4);
    std::cout << "OK: Correct number of fragments (4)." << std::endl;

    assert(fragments[0].type == CHTL::FragmentType::CHTL);
    assert(fragments[1].type == CHTL::FragmentType::CSS);
    assert(fragments[2].type == CHTL::FragmentType::CHTL);
    assert(fragments[3].type == CHTL::FragmentType::JS);
    std::cout << "OK: Fragments have correct types (CHTL, CSS, CHTL, JS)." << std::endl;

    // 2. Assertions on Style Block
    const auto& css_content = fragments[1].content;
    assert(contains(css_content, "/* _PLACEHOLDER_0_ */"));
    assert(contains(css_content, "/* _PLACEHOLDER_1_ */"));
    assert(contains(css_content, "font-family: Arial, sans-serif;"));
    std::cout << "OK: CSS fragment contains style placeholders and preserves pure CSS." << std::endl;

    // 3. Assertions on Script Block
    const auto& js_content = fragments[3].content;
    assert(contains(js_content, "_PLACEHOLDER_2_")); // First JS placeholder after the two from style
    assert(contains(js_content, "{{b}}"));
    assert(contains(js_content, "Listen {"));
    assert(contains(js_content, "$someValue$"));
    std::cout << "OK: JS fragment contains JS placeholders and preserves CHTL-JS." << std::endl;

    // 4. Assertions on Placeholder Map
    // Style block has 2 CHTL lines. Script block has 4 JS sections around 3 CHTL-JS constructs. Total = 2 + 4 = 6 placeholders.
    assert(placeholder_map.size() == 6);
    std::cout << "OK: Placeholder map has the correct number of entries (6)." << std::endl;

    // Spot check map content
    assert(contains(placeholder_map.at("_PLACEHOLDER_0_"), "width: 100px + 50px;"));
    assert(contains(placeholder_map.at("_PLACEHOLDER_1_"), "color: ThemeColor(primary);"));
    assert(contains(placeholder_map.at("_PLACEHOLDER_2_"), "function greet(name)"));
    assert(placeholder_map.at("_PLACEHOLDER_3_") == ";\n    ");
    assert(contains(placeholder_map.at("_PLACEHOLDER_4_"), "let myVar = "));
    assert(contains(placeholder_map.at("_PLACEHOLDER_5_"), "console.log(\"Done.\");"));
    std::cout << "OK: Placeholder map contains correct original code snippets." << std::endl;


    std::cout << "\n--- Scanner Test Passed! ---" << std::endl;

    return 0;
}
