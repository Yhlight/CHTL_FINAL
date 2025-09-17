#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <stdexcept>
#include "../src/Scanner/CHTLUnifiedScanner.h"

// Helper to print FragmentType for test assertions
namespace CHTL {
    std::ostream& operator<<(std::ostream& os, const FragmentType& type) {
        switch (type) {
            case FragmentType::CHTL:    os << "CHTL"; break;
            case FragmentType::CSS:     os << "CSS"; break;
            case FragmentType::JS:      os << "JS"; break;
            case FragmentType::CHTL_JS: os << "CHTL_JS"; break;
            case FragmentType::UNKNOWN: os << "UNKNOWN"; break;
            default: os << "INVALID_TYPE";
        }
        return os;
    }
}

// --- Simple Assertion Framework ---
int test_count = 0;
int failed_count = 0;

#define ASSERT_TRUE(condition) \
    do { \
        test_count++; \
        if (!(condition)) { \
            failed_count++; \
            std::cerr << "FAIL: " << __FILE__ << ":" << __LINE__ << " (" << #condition << ")" << std::endl; \
        } \
    } while (false)

#define ASSERT_EQUAL(expected, actual) \
    do { \
        test_count++; \
        if ((expected) != (actual)) { \
            failed_count++; \
            std::cerr << "FAIL: " << __FILE__ << ":" << __LINE__ << " -- Expected: '" << (expected) << "', Actual: '" << (actual) << "'" << std::endl; \
        } \
    } while (false)

// --- Test Cases ---

void test_simple_script_block() {
    std::cout << "Running " << __FUNCTION__ << "..." << std::endl;
    std::string source = "div { script { const a = 1; {{box}}->show(); } }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    ASSERT_EQUAL(3, fragments.size());
    ASSERT_EQUAL(CHTL::FragmentType::CHTL, fragments[0].type);
    ASSERT_EQUAL("div { ", fragments[0].content);
    ASSERT_EQUAL(CHTL::FragmentType::CHTL_JS, fragments[1].type);
    ASSERT_EQUAL(CHTL::FragmentType::CHTL, fragments[2].type);
    ASSERT_EQUAL(" }", fragments[2].content);

    auto placeholder_map = scanner.getPlaceholderMap();
    ASSERT_EQUAL(2, placeholder_map.size());
    ASSERT_EQUAL(" const a = 1; ", placeholder_map.at("__JS_PLACEHOLDER_0__"));
    ASSERT_EQUAL("show(); ", placeholder_map.at("__JS_PLACEHOLDER_1__"));

    ASSERT_EQUAL("__JS_PLACEHOLDER_0__{{box}}->__JS_PLACEHOLDER_1__", fragments[1].content);
}

void test_non_recursive_complex_script() {
    std::cout << "Running " << __FUNCTION__ << "..." << std::endl;
    std::string source = "script { let x = 1; Listen { click: () => { console.log('hi'); } }; let y = 2; }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    ASSERT_EQUAL(2, fragments.size());
    ASSERT_EQUAL(CHTL::FragmentType::CHTL, fragments[0].type);
    ASSERT_EQUAL("", fragments[0].content);
    ASSERT_EQUAL(CHTL::FragmentType::CHTL_JS, fragments[1].type);

    auto placeholder_map = scanner.getPlaceholderMap();
    ASSERT_EQUAL(2, placeholder_map.size());
    ASSERT_EQUAL(" let x = 1; ", placeholder_map.at("__JS_PLACEHOLDER_0__"));
    ASSERT_EQUAL("; let y = 2; ", placeholder_map.at("__JS_PLACEHOLDER_1__"));

    ASSERT_EQUAL("__JS_PLACEHOLDER_0__Listen { click: () => { console.log('hi'); } }__JS_PLACEHOLDER_1__", fragments[1].content);
}

void test_global_origin_blocks() {
    std::cout << "Running " << __FUNCTION__ << "..." << std::endl;
    std::string source = "[Origin] @JavaScript { console.log('raw js'); } div {} [Origin] @Style { body { color: red; } }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    ASSERT_EQUAL(4, fragments.size());
    ASSERT_EQUAL(CHTL::FragmentType::CHTL, fragments[0].type);
    ASSERT_EQUAL("", fragments[0].content);
    ASSERT_EQUAL(CHTL::FragmentType::JS, fragments[1].type);
    ASSERT_EQUAL(" console.log('raw js'); ", fragments[1].content);
    ASSERT_EQUAL(CHTL::FragmentType::CHTL, fragments[2].type);
    ASSERT_EQUAL(" div {} ", fragments[2].content);
    ASSERT_EQUAL(CHTL::FragmentType::CSS, fragments[3].type);
    ASSERT_EQUAL(" body { color: red; } ", fragments[3].content);
}

void test_keyword_in_string_or_comment() {
    std::cout << "Running " << __FUNCTION__ << "..." << std::endl;
    // Added a newline before the closing brace to correctly terminate the comment.
    std::string source = "script { const my_var = \"Listen\"; /* Animate */ // {{box}} \n}";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    ASSERT_EQUAL(2, fragments.size());
    ASSERT_EQUAL(CHTL::FragmentType::CHTL, fragments[0].type);
    ASSERT_EQUAL("", fragments[0].content);
    ASSERT_EQUAL(CHTL::FragmentType::CHTL_JS, fragments[1].type);

    auto placeholder_map = scanner.getPlaceholderMap();
    ASSERT_EQUAL(1, placeholder_map.size());

    ASSERT_EQUAL(" const my_var = \"Listen\"; /* Animate */ // {{box}} \n", placeholder_map.at("__JS_PLACEHOLDER_0__"));
    ASSERT_EQUAL("__JS_PLACEHOLDER_0__", fragments[1].content);
}

void test_only_js_in_script() {
    std::cout << "Running " << __FUNCTION__ << "..." << std::endl;
    std::string source = "script { function hello() { return 1; } }";
     CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    ASSERT_EQUAL(2, fragments.size());
    ASSERT_EQUAL(CHTL::FragmentType::CHTL, fragments[0].type);
    ASSERT_EQUAL("", fragments[0].content);
    ASSERT_EQUAL(CHTL::FragmentType::CHTL_JS, fragments[1].type);

    auto placeholder_map = scanner.getPlaceholderMap();
    ASSERT_EQUAL(1, placeholder_map.size());
    ASSERT_EQUAL(" function hello() { return 1; } ", placeholder_map.at("__JS_PLACEHOLDER_0__"));
    ASSERT_EQUAL("__JS_PLACEHOLDER_0__", fragments[1].content);
}


// --- Test Runner Main ---

int main() {
    std::cout << "--- Running CHTLUnifiedScanner Unit Tests ---" << std::endl;

    test_simple_script_block();
    test_non_recursive_complex_script();
    test_global_origin_blocks();
    test_keyword_in_string_or_comment();
    test_only_js_in_script();

    std::cout << "---------------------------------------------" << std::endl;
    if (failed_count > 0) {
        std::cerr << failed_count << " of " << test_count << " assertions failed." << std::endl;
        return 1;
    }

    std::cout << "All " << test_count << " assertions passed." << std::endl;
    return 0;
}
