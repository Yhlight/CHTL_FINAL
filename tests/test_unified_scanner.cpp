#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <map>
#include "../src/Scanner/CHTLUnifiedScanner.h"

using namespace CHTL;

// Function to convert FragmentType enum to a string for printing
std::string fragmentTypeToString(CHTL::FragmentType type) {
    switch (type) {
        case CHTL::FragmentType::CHTL: return "CHTL";
        case CHTL::FragmentType::CSS: return "CSS";
        case CHTL::FragmentType::JS: return "JS";
        case CHTL::FragmentType::CHTL_JS: return "CHTL_JS";
        case CHTL::FragmentType::UNKNOWN: return "UNKNOWN";
        default: return "INVALID";
    }
}

// Overload the << operator for std::ostream to allow printing FragmentType
std::ostream& operator<<(std::ostream& os, CHTL::FragmentType type) {
    os << fragmentTypeToString(type);
    return os;
}

// --- Test Harness ---
int tests_run = 0;
int tests_failed = 0;

#define TEST_CASE(name) void name()
#define RUN_TEST(test) \
    tests_run++; \
    try { \
        test(); \
        std::cout << "[PASS] " << #test << std::endl; \
    } catch (const std::runtime_error& e) { \
        tests_failed++; \
        std::cout << "[FAIL] " << #test << " (exception: " << e.what() << ")" << std::endl; \
    }

#define ASSERT_EQUAL(expected, actual) \
    if ((expected) != (actual)) { \
        tests_failed++; \
        std::cout << "[FAIL] " << __func__ << " at line " << __LINE__ << ": " \
                  << "Expected '" << (expected) << "', but got '" << (actual) << "'" << std::endl; \
        return; \
    }

// --- Test Cases ---

TEST_CASE(test_simple_chtl) {
    std::string source = "div { text: 'hello'; }";
    CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    ASSERT_EQUAL(1, fragments.size());
    ASSERT_EQUAL(FragmentType::CHTL, fragments[0].type);
    ASSERT_EQUAL(source, fragments[0].content);
}

TEST_CASE(test_simple_style_block) {
    std::string source = "style { color: red; }";
    CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    ASSERT_EQUAL(1, fragments.size());
    ASSERT_EQUAL(FragmentType::CSS, fragments[0].type);
    ASSERT_EQUAL(" color: red; ", fragments[0].content);
}

TEST_CASE(test_nested_script_block) {
    std::string source = "div { script { console.log(1); } }";
    CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    ASSERT_EQUAL(3, fragments.size());
    ASSERT_EQUAL(FragmentType::CHTL, fragments[0].type);
    ASSERT_EQUAL("div { ", fragments[0].content);
    ASSERT_EQUAL(FragmentType::CHTL_JS, fragments[1].type);
    ASSERT_EQUAL(FragmentType::CHTL, fragments[2].type);
    ASSERT_EQUAL(" }", fragments[2].content);
}

TEST_CASE(test_keyword_in_string) {
    std::string source = "div { text: 'style { color: blue; }'; }";
    CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    ASSERT_EQUAL(1, fragments.size());
    ASSERT_EQUAL(FragmentType::CHTL, fragments[0].type);
    ASSERT_EQUAL(source, fragments[0].content);
}

TEST_CASE(test_keyword_in_comment) {
    std::string source = "div { /* script { alert('x'); } */ }";
    CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    ASSERT_EQUAL(1, fragments.size());
    ASSERT_EQUAL(FragmentType::CHTL, fragments[0].type);
}

TEST_CASE(test_placeholder_logic_simple) {
    std::string source = "script { const a = 1; {{box}}->show(); const b = 2; }";
    CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    ASSERT_EQUAL(1, fragments.size());
    ASSERT_EQUAL(FragmentType::CHTL_JS, fragments[0].type);

    const auto& placeholder_map = scanner.getPlaceholderMap();
    ASSERT_EQUAL(2, placeholder_map.size());

    std::string expected_chtl_js = "__CHTL_JS_PLACEHOLDER_0__{{box}}->__CHTL_JS_PLACEHOLDER_1__";
    ASSERT_EQUAL(expected_chtl_js, fragments[0].content);

    ASSERT_EQUAL(" const a = 1; ", placeholder_map.at("__CHTL_JS_PLACEHOLDER_0__"));
    ASSERT_EQUAL("show(); const b = 2; ", placeholder_map.at("__CHTL_JS_PLACEHOLDER_1__"));
}

TEST_CASE(test_placeholder_with_robust_block_finding) {
    std::string source = "script { animate { target: {{box}}, duration: 100, easing: 'ease-in-out' } }";
    CHTLUnifiedScanner scanner(source);
    scanner.scan();
    const auto& placeholder_map = scanner.getPlaceholderMap();
    ASSERT_EQUAL(0, placeholder_map.size());
}

TEST_CASE(test_multiple_blocks) {
    std::string source = "style{a:b} div{} script{c:d}style{e:f}";
    CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    ASSERT_EQUAL(4, fragments.size());
    ASSERT_EQUAL(FragmentType::CSS, fragments[0].type);
    ASSERT_EQUAL("a:b", fragments[0].content);
    ASSERT_EQUAL(FragmentType::CHTL, fragments[1].type);
    ASSERT_EQUAL(" div{} ", fragments[1].content);
    ASSERT_EQUAL(FragmentType::CHTL_JS, fragments[2].type);
    ASSERT_EQUAL(FragmentType::CSS, fragments[3].type);
    ASSERT_EQUAL("e:f", fragments[3].content);
}

// --- Main Runner ---
int main() {
    RUN_TEST(test_simple_chtl);
    RUN_TEST(test_simple_style_block);
    RUN_TEST(test_nested_script_block);
    RUN_TEST(test_keyword_in_string);
    RUN_TEST(test_keyword_in_comment);
    RUN_TEST(test_placeholder_logic_simple);
    RUN_TEST(test_placeholder_with_robust_block_finding);
    RUN_TEST(test_multiple_blocks);

    std::cout << "\nScanner Unit Tests finished. " << tests_run << " run, " << tests_failed << " failed." << std::endl;
    return tests_failed > 0 ? 1 : 0;
}
