#include "../../CHTL/Scanner/CHTLUnifiedScanner.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <map>

// Helper to find a fragment of a specific type
const CHTL::CodeFragment* find_fragment(const std::vector<CHTL::CodeFragment>& fragments, CHTL::FragmentType type) {
    for (const auto& f : fragments) {
        if (f.type == type) {
            return &f;
        }
    }
    return nullptr;
}

void print_fragments(const std::vector<CHTL::CodeFragment>& fragments) {
    std::cout << "--- Fragments (" << fragments.size() << ") ---" << std::endl;
    for (const auto& fragment : fragments) {
        std::string type_str;
        switch (fragment.type) {
            case CHTL::FragmentType::CHTL: type_str = "CHTL"; break;
            case CHTL::FragmentType::CSS: type_str = "CSS"; break;
            case CHTL::FragmentType::JS: type_str = "JS"; break;
            case CHTL::FragmentType::CHTL_JS: type_str = "CHTL_JS"; break;
            case CHTL::FragmentType::CHTL_in_CSS: type_str = "CHTL_in_CSS"; break;
        }
        std::cout << "Type: " << type_str << std::endl;
        std::cout << "Content: ```\n" << fragment.content << "\n```" << std::endl;
        if (!fragment.js_placeholders.empty()) {
            std::cout << "Placeholders:" << std::endl;
            for (const auto& pair : fragment.js_placeholders) {
                std::cout << "  " << pair.first << " -> `" << pair.second << "`" << std::endl;
            }
        }
    }
    std::cout << "---------------------\n";
}

void test_pure_chtl() {
    std::cout << "\n--- Running Test: Pure CHTL ---\n";
    std::string source = "div { text: \"Hello\"; }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    print_fragments(fragments);
    assert(fragments.size() == 1);
    assert(fragments[0].type == CHTL::FragmentType::CHTL);
    assert(fragments[0].content == source);
    std::cout << "PASS\n";
}

void test_top_level_split() {
    std::cout << "\n--- Running Test: Top Level Split ---\n";
    std::string source = "div { } style { color: red; } script { let a = 1; }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    print_fragments(fragments);

    assert(fragments.size() == 3);
    assert(fragments[0].type == CHTL::FragmentType::CHTL);
    assert(fragments[0].content.find("div { }") != std::string::npos);
    assert(fragments[1].type == CHTL::FragmentType::CSS);
    assert(fragments[1].content.find(" color: red; ") != std::string::npos);
    assert(fragments[2].type == CHTL::FragmentType::CHTL_JS);
    assert(fragments[2].js_placeholders.size() == 1);
    std::cout << "PASS\n";
}

void test_style_block_processing() {
    std::cout << "\n--- Running Test: Style Block Processing ---\n";
    std::string source = "style { color: red; @Style Theme; width: 100px; }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    print_fragments(fragments);
    assert(fragments.size() == 3);
    assert(fragments[0].type == CHTL::FragmentType::CSS);
    assert(fragments[0].content.find("color: red;") != std::string::npos);
    assert(fragments[1].type == CHTL::FragmentType::CHTL_in_CSS);
    assert(fragments[1].content.find("@Style Theme;") != std::string::npos);
    assert(fragments[2].type == CHTL::FragmentType::CSS);
    assert(fragments[2].content.find("width: 100px;") != std::string::npos);
    std::cout << "PASS\n";
}

void test_simple_script_block() {
    std::cout << "\n--- Running Test: Simple Script Block ---\n";
    std::string source = "script { const a = 1; {{box}}; const b = 2; }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    print_fragments(fragments);

    assert(fragments.size() == 1);
    assert(fragments[0].type == CHTL::FragmentType::CHTL_JS);
    assert(fragments[0].content.find("{{box}}") != std::string::npos);
    assert(fragments[0].content.find("_JS_PLACEHOLDER_") != std::string::npos);
    assert(fragments[0].js_placeholders.size() == 2);
    std::cout << "PASS\n";
}

void test_chtl_js_block_detection() {
    std::cout << "\n--- Running Test: CHTL JS Block Detection ---\n";
    std::string source = "script { const a = 1; Listen { click: 'foo' } const b = 2; }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    print_fragments(fragments);

    assert(fragments.size() == 1);
    const auto& frag = fragments[0];
    assert(frag.type == CHTL::FragmentType::CHTL_JS);

    // The CHTL JS construct should be in the main content
    assert(frag.content.find("Listen { click: 'foo' }") != std::string::npos);
    // The pure JS parts should be placeholders
    assert(frag.content.find("_JS_PLACEHOLDER_0_") == 0);
    assert(frag.content.find("_JS_PLACEHOLDER_1_") != std::string::npos);

    // The placeholder map should contain the pure JS
    assert(frag.js_placeholders.size() == 2);
    assert(frag.js_placeholders.at("_JS_PLACEHOLDER_0_").find("const a = 1;") != std::string::npos);
    assert(frag.js_placeholders.at("_JS_PLACEHOLDER_1_").find("const b = 2;") != std::string::npos);

    std::cout << "PASS\n";
}


void test_advanced_style_features() {
    std::cout << "\n--- Running Test: Advanced Style Features ---\n";
    std::string source = "style { \n"
                         "  width: 100px + 20px; \n"
                         "  div[type=\"button\"] { color: blue; } \n"
                         "  @media (min-width: 600px) { font-size: 1rem; } \n"
                         "  # a comment \n"
                         "  color: #fff; \n"
                         "}";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    print_fragments(fragments);

    int chtl_in_css_count = 0;
    int css_count = 0;

    for(const auto& f : fragments) {
        if (f.type == CHTL::FragmentType::CHTL_in_CSS) chtl_in_css_count++;
        else if (f.type == CHTL::FragmentType::CSS) css_count++;
    }

    assert(chtl_in_css_count == 2);
    assert(css_count == 3);
    std::cout << "PASS\n";
}

void test_whole_word_matching() {
    std::cout << "\n--- Running Test: Whole Word Matching ---\n";
    std::string source = "div { class: style; } stylesheet {}";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    print_fragments(fragments);
    // Should NOT find a 'style' block and treat everything as CHTL
    assert(fragments.size() == 1);
    assert(fragments[0].type == CHTL::FragmentType::CHTL);
    std::cout << "PASS\n";
}


int main() {
    try {
        test_pure_chtl();
        test_top_level_split();
        test_style_block_processing();
        test_simple_script_block();
        test_chtl_js_block_detection();
        test_advanced_style_features();
        test_whole_word_matching();
        std::cout << "\nAll scanner tests passed!\n";
    } catch (const std::exception& e) {
        std::cerr << "A test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "A test failed with an unknown exception." << std::endl;
        return 1;
    }
    return 0;
}
