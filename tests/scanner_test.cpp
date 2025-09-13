#include "../src/Scanner/CHTLUnifiedScanner.h"
#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include <map>

// A simple assertion helper
void assertEquals(const std::string& expected, const std::string& actual, const std::string& message) {
    if (expected != actual) {
        throw std::runtime_error("Assertion failed: " + message + "\n  Expected: " + expected + "\n  Actual:   " + actual);
    }
}

void assertEquals(size_t expected, size_t actual, const std::string& message) {
    if (expected != actual) {
        throw std::runtime_error("Assertion failed: " + message + "\n  Expected: " + std::to_string(expected) + "\n  Actual:   " + std::to_string(actual));
    }
}

void assertEquals(CHTL::ChunkType expected, CHTL::ChunkType actual, const std::string& message) {
    if (expected != actual) {
        throw std::runtime_error("Assertion failed: " + message +
            "\n  Expected: " + std::to_string(static_cast<int>(expected)) +
            "\n  Actual:   " + std::to_string(static_cast<int>(actual)));
    }
}

// Test runner
int total_tests = 0;
int failed_tests = 0;
void run_test(const std::string& test_name, void (*test_func)()) {
    total_tests++;
    std::cout << "TEST: " << test_name << " ... ";
    try {
        test_func();
        std::cout << "PASSED" << std::endl;
    } catch (const std::exception& e) {
        failed_tests++;
        std::cout << "FAILED" << std::endl;
        std::cerr << "      " << e.what() << std::endl;
    }
}

// --- Test Cases ---

void test_simple_chtl() {
    std::string source = "div { text { \"Hello\" } }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto chunks = scanner.scan();
    assertEquals((size_t)1, chunks.size(), "Should be 1 chunk for simple CHTL");
    assertEquals(CHTL::ChunkType::CHTL, chunks[0].type, "Chunk type should be CHTL");
    assertEquals(source, chunks[0].content, "Chunk content should match source");
}

void test_script_tag() {
    std::string source = "html { <script>let a = 1;</script> }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto chunks = scanner.scan();
    auto placeholder_map = scanner.getPlaceholderMap();

    assertEquals((size_t)1, chunks.size(), "Should be 1 CHTL chunk");
    assertEquals(CHTL::ChunkType::CHTL, chunks[0].type, "Chunk type should be CHTL");
    assertEquals("html { <script>__JS_PLACEHOLDER_0__</script> }", chunks[0].content, "Content of CHTL chunk");

    assertEquals((size_t)1, placeholder_map.size(), "Placeholder map should have 1 entry");
    assertEquals("let a = 1;", placeholder_map.at("__JS_PLACEHOLDER_0__"), "Placeholder content");
}

void test_style_tag() {
    std::string source = "<style>body { color: red; }</style>";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto chunks = scanner.scan();
    auto placeholder_map = scanner.getPlaceholderMap();

    assertEquals((size_t)1, chunks.size(), "Should be 1 CHTL chunk for style tag");
    assertEquals("<style>__CSS_PLACEHOLDER_0__</style>", chunks[0].content, "Content of style chunk");

    assertEquals((size_t)1, placeholder_map.size(), "Placeholder map should have 1 entry for style");
    assertEquals("body { color: red; }", placeholder_map.at("__CSS_PLACEHOLDER_0__"), "Style placeholder content");
}

void test_local_script_block() {
    std::string source = "div { script { console.log('hi'); } }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto chunks = scanner.scan();

    assertEquals((size_t)2, chunks.size(), "Should be 2 chunks for local script (CHTL and ChtlJs)");
    assertEquals(CHTL::ChunkType::CHTL, chunks[0].type, "First chunk should be CHTL");
    assertEquals("div {  }", chunks[0].content, "Content of CHTL chunk (local script)");

    assertEquals(CHTL::ChunkType::ChtlJs, chunks[1].type, "Second chunk should be ChtlJs");
    assertEquals(" console.log('hi'); ", chunks[1].content, "Content of ChtlJs chunk");
}

void test_origin_js_block() {
    std::string source = "[Origin] @JavaScript { alert(1) }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto chunks = scanner.scan();
    auto placeholder_map = scanner.getPlaceholderMap();

    assertEquals((size_t)1, chunks.size(), "Should be 1 chunk for origin JS block");
    assertEquals("[Origin] @JavaScript {__JS_PLACEHOLDER_0__}", chunks[0].content, "Origin JS block content");

    assertEquals((size_t)1, placeholder_map.size(), "Placeholder map should have 1 entry for origin JS");
    assertEquals(" alert(1) ", placeholder_map.at("__JS_PLACEHOLDER_0__"), "Origin JS placeholder content");
}

void test_mixed_content() {
    std::string source = "div { <style>a{}</style> script {let d=1;} }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto chunks = scanner.scan();
    auto placeholder_map = scanner.getPlaceholderMap();

    assertEquals((size_t)2, chunks.size(), "Mixed: Should be 2 chunks");

    assertEquals(CHTL::ChunkType::CHTL, chunks[0].type, "Mixed: First chunk CHTL");
    assertEquals("div { <style>__CSS_PLACEHOLDER_0__</style>  }", chunks[0].content, "Mixed: CHTL part");

    assertEquals(CHTL::ChunkType::ChtlJs, chunks[1].type, "Mixed: Second chunk ChtlJs");
    assertEquals("let d=1;", chunks[1].content, "Mixed: ChtlJs part");

    assertEquals((size_t)1, placeholder_map.count("__CSS_PLACEHOLDER_0__"), "Mixed: CSS placeholder exists");
    assertEquals("a{}", placeholder_map.at("__CSS_PLACEHOLDER_0__"), "Mixed: CSS content");
}


int main() {
    std::cout << "--- CHTLUnifiedScanner Tests ---" << std::endl;

    run_test("Simple CHTL only", test_simple_chtl);
    run_test("HTML Script Tag", test_script_tag);
    run_test("HTML Style Tag", test_style_tag);
    run_test("CHTL Local Script Block", test_local_script_block);
    run_test("CHTL Origin JS Block", test_origin_js_block);
    run_test("Complex Mixed Content", test_mixed_content);

    std::cout << "--------------------------------" << std::endl;
    if (failed_tests > 0) {
        std::cerr << failed_tests << " out of " << total_tests << " tests failed." << std::endl;
        return 1;
    }
    std::cout << "All " << total_tests << " tests passed." << std::endl;
    return 0;
}
