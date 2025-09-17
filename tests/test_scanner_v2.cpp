#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <map>
#include <iomanip>
#include "../src/Scanner/CHTLUnifiedScanner.h"

// --- V2 Test Framework ---
namespace TestFrameworkV2 {
    int tests_run = 0;
    int tests_failed = 0;

    // Helper to print fragment types for easier debugging
    std::ostream& operator<<(std::ostream& os, const CHTL::FragmentType& type) {
        switch (type) {
            case CHTL::FragmentType::CHTL: os << "CHTL"; break;
            case CHTL::FragmentType::CHTL_JS: os << "CHTL_JS"; break;
            case CHTL::FragmentType::CHTL_STYLE_BLOCK: os << "CHTL_STYLE_BLOCK"; break;
            case CHTL::FragmentType::CHTL_SCRIPT_BLOCK: os << "CHTL_SCRIPT_BLOCK"; break;
            case CHTL::FragmentType::UNKNOWN: os << "UNKNOWN"; break;
            default: os << "INVALID_TYPE";
        }
        return os;
    }

    // Helper to print CodeFragment for easier debugging
    std::ostream& operator<<(std::ostream& os, const CHTL::CodeFragment& frag) {
        os << "Fragment{ Type: " << frag.type << ", Content: " << std::quoted(frag.content) << " }";
        return os;
    }

    // Assertion function with detailed logging
    template<typename T1, typename T2>
    void AssertEqual(const T1& expected, const T2& actual, const std::string& test_name, int line, const std::string& message) {
        if (expected != actual) {
            std::cerr << "\n[FAIL] " << test_name << std::endl;
            std::cerr << "  (L" << line << ") Assertion failed: " << message << std::endl;
            std::cerr << "    Expected: " << expected << std::endl;
            std::cerr << "    Actual  : " << actual << std::endl;
            throw std::runtime_error("Assertion failed");
        }
    }

    // Macro to simplify calling AssertEqual
    #define ASSERT_EQUAL(expected, actual, message) \
        AssertEqual(expected, actual, __func__, __LINE__, message)

    // Macro to run a test function
    #define RUN_TEST(test_func) \
        tests_run++; \
        try { \
            test_func(); \
            std::cout << "[PASS] " << #test_func << std::endl; \
        } catch (const std::runtime_error&) { \
            tests_failed++; \
        } catch (const std::exception& e) { \
            tests_failed++; \
            std::cerr << "\n[FAIL] " << #test_func << ": Unhandled exception: " << e.what() << std::endl; \
        }

} // namespace TestFrameworkV2

using namespace TestFrameworkV2;
using namespace CHTL;

// --- Test Cases ---

void test_global_style_block() {
    std::string source = "div {}\nstyle { color: red; }\n";
    CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    ASSERT_EQUAL(3, (int)fragments.size(), "Should produce 3 fragments");

    ASSERT_EQUAL(FragmentType::CHTL, fragments[0].type, "Fragment 1 type");
    ASSERT_EQUAL("div {}\n", fragments[0].content, "Fragment 1 content");

    ASSERT_EQUAL(FragmentType::CHTL_STYLE_BLOCK, fragments[1].type, "Fragment 2 type");
    ASSERT_EQUAL(" color: red; ", fragments[1].content, "Fragment 2 content");

    ASSERT_EQUAL(FragmentType::CHTL, fragments[2].type, "Fragment 3 type");
    ASSERT_EQUAL("\n", fragments[2].content, "Fragment 3 content");
}


void test_local_style_block() {
    std::string source = "div {\n  style { color: red; }\n}";
    CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    ASSERT_EQUAL(3, (int)fragments.size(), "Should produce 3 fragments");

    ASSERT_EQUAL(FragmentType::CHTL, fragments[0].type, "Fragment 1 type");
    ASSERT_EQUAL("div {\n  ", fragments[0].content, "Fragment 1 content");

    ASSERT_EQUAL(FragmentType::CHTL_STYLE_BLOCK, fragments[1].type, "Fragment 2 type");
    ASSERT_EQUAL(" color: red; ", fragments[1].content, "Fragment 2 content");

    ASSERT_EQUAL(FragmentType::CHTL, fragments[2].type, "Fragment 3 type");
    ASSERT_EQUAL("\n}", fragments[2].content, "Fragment 3 content");
}

void test_local_script_block() {
    std::string source = "div { script { {{a}} } }";
    CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    ASSERT_EQUAL(3, (int)fragments.size(), "Should produce 3 fragments");

    ASSERT_EQUAL(FragmentType::CHTL, fragments[0].type, "Fragment 1 type");
    ASSERT_EQUAL("div { ", fragments[0].content, "Fragment 1 content");

    ASSERT_EQUAL(FragmentType::CHTL_SCRIPT_BLOCK, fragments[1].type, "Fragment 2 type");
    // The content is processed by scanJsAndChtlJs, which should wrap non-keyword JS in placeholders
    ASSERT_EQUAL(true, fragments[1].content.find("{{a}}") != std::string::npos, "Fragment 2 should contain the keyword");
    ASSERT_EQUAL(true, fragments[1].content.find("__CHTL_JS_PLACEHOLDER_") != std::string::npos, "Fragment 2 should contain placeholders for whitespace");

    ASSERT_EQUAL(FragmentType::CHTL, fragments[2].type, "Fragment 3 type");
    ASSERT_EQUAL(" }", fragments[2].content, "Fragment 3 content");
}

void test_bracket_constructs() {
    std::string source = "[Template] @Element Box { }\n[Import] from \"./file\";";
    CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    ASSERT_EQUAL(3, (int)fragments.size(), "Should produce 3 fragments");

    ASSERT_EQUAL(FragmentType::CHTL, fragments[0].type, "Fragment 1 type");
    ASSERT_EQUAL("[Template] @Element Box { }", fragments[0].content, "Fragment 1 content");

    ASSERT_EQUAL(FragmentType::CHTL, fragments[1].type, "Fragment 2 type");
    ASSERT_EQUAL("\n", fragments[1].content, "Fragment 2 content");

    ASSERT_EQUAL(FragmentType::CHTL, fragments[2].type, "Fragment 3 type");
    ASSERT_EQUAL("[Import] from \"./file\";", fragments[2].content, "Fragment 3 content");
}


// --- Main Runner ---
int main() {
    std::cout << "Running V2 Unified Scanner Tests..." << std::endl;

    RUN_TEST(test_global_style_block);
    RUN_TEST(test_local_style_block);
    RUN_TEST(test_local_script_block);
    RUN_TEST(test_bracket_constructs);

    std::cout << "\n--- V2 Test Summary ---" << std::endl;
    if (tests_failed == 0) {
        std::cout << "All " << tests_run << " tests passed." << std::endl;
    } else {
        std::cout << tests_failed << " of " << tests_run << " tests failed." << std::endl;
    }
    std::cout << "-----------------------" << std::endl;

    return tests_failed > 0 ? 1 : 0;
}
