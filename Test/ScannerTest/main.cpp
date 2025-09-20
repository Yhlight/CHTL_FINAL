#include "../../CHTL/Scanner/CHTLUnifiedScanner.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <string>

// Helper to trim whitespace from both ends of a string
std::string trim(const std::string& str) {
    const auto strBegin = str.find_first_not_of(" \t\n\r");
    if (strBegin == std::string::npos) return "";
    const auto strEnd = str.find_last_not_of(" \t\n\r");
    const auto strRange = strEnd - strBegin + 1;
    return str.substr(strBegin, strRange);
}

// Simple assertion helper
void assert_equal(const std::string& a, const std::string& b, const std::string& message) {
    if (trim(a) != trim(b)) {
        std::cerr << "Assertion failed: " << message << std::endl;
        std::cerr << "  Expected: \"" << trim(b) << "\"" << std::endl;
        std::cerr << "  Actual:   \"" << trim(a) << "\"" << std::endl;
        exit(1);
    }
}

void assert_equal(CHTL::FragmentType a, CHTL::FragmentType b, const std::string& message) {
    if (a != b) {
        std::cerr << "Assertion failed: " << message << std::endl;
        exit(1);
    }
}


void test_simple_script_separation() {
    std::cout << "Running test: test_simple_script_separation" << std::endl;
    std::string source = "script { const a = {{b}}; }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    assert(fragments.size() == 3);
    assert_equal(fragments[0].type, CHTL::FragmentType::JS, "Frag 0 Type");
    assert_equal(fragments[1].type, CHTL::FragmentType::CHTL_JS, "Frag 1 Type");
    assert_equal(fragments[1].content, "{{b}}", "Frag 1 Content");
    assert_equal(fragments[2].type, CHTL::FragmentType::JS, "Frag 2 Type");

    auto placeholders = scanner.getPlaceholders();
    assert_equal(placeholders.at(fragments[0].content), "const a =", "Placeholder 0");
    assert_equal(placeholders.at(fragments[2].content), ";", "Placeholder 1");
    std::cout << "PASSED" << std::endl;
}

void test_chtl_js_block_keyword() {
    std::cout << "Running test: test_chtl_js_block_keyword" << std::endl;
    std::string source = "script { console.log('hello'); Listen { click: () => {} } console.log('world'); }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    assert(fragments.size() == 3);
    assert_equal(fragments[0].type, CHTL::FragmentType::JS, "Frag 0 Type");
    assert_equal(fragments[1].type, CHTL::FragmentType::CHTL_JS, "Frag 1 Type");
    assert_equal(fragments[1].content, "Listen { click: () => {} }", "Frag 1 Content");
    assert_equal(fragments[2].type, CHTL::FragmentType::JS, "Frag 2 Type");

    auto placeholders = scanner.getPlaceholders();
    assert(placeholders.size() == 2);
    assert_equal(placeholders.at(fragments[0].content), "console.log('hello');", "Placeholder 0");
    assert_equal(placeholders.at(fragments[2].content), "console.log('world');", "Placeholder 1");
    std::cout << "PASSED" << std::endl;
}

void test_style_separation() {
    std::cout << "Running test: test_style_separation" << std::endl;
    std::string source = "style { color: red; @Style MyTemplate; width: 100px + 20px; }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    assert(fragments.size() == 3);
    assert_equal(fragments[0].type, CHTL::FragmentType::CSS, "Frag 0 Type");
    assert_equal(fragments[0].content, "color: red;", "Frag 0 Content");
    assert_equal(fragments[1].type, CHTL::FragmentType::CHTL, "Frag 1 Type");
    assert_equal(fragments[1].content, "@Style MyTemplate;", "Frag 1 Content");
    assert_equal(fragments[2].type, CHTL::FragmentType::CHTL, "Frag 2 Type");
    assert_equal(fragments[2].content, "width: 100px + 20px;", "Frag 2 Content");
    std::cout << "PASSED" << std::endl;
}

void test_full_source_file() {
    std::cout << "Running test: test_full_source_file" << std::endl;
    std::string source = "div { text: 'hello' } style { color: blue; } script { let a = 1; }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    assert(fragments.size() == 3);
    assert_equal(fragments[0].type, CHTL::FragmentType::CHTL, "Frag 0 Type");
    assert_equal(fragments[0].content, "div { text: 'hello' }", "Frag 0 Content");
    assert_equal(fragments[1].type, CHTL::FragmentType::CSS, "Frag 1 Type");
    assert_equal(fragments[1].content, "color: blue;", "Frag 1 Content");
    assert_equal(fragments[2].type, CHTL::FragmentType::JS, "Frag 2 Type");

    auto placeholders = scanner.getPlaceholders();
    assert(placeholders.size() == 1);
    assert_equal(placeholders.at(fragments[2].content), "let a = 1;", "Placeholder Content");
    std::cout << "PASSED" << std::endl;
}


int main() {
    test_simple_script_separation();
    test_chtl_js_block_keyword();
    test_style_separation();
    test_full_source_file();

    std::cout << "\nAll scanner tests passed!" << std::endl;
    return 0;
}
