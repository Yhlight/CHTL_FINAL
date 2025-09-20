#include "../../CHTL/Scanner/CHTLUnifiedScanner.h"
#include <iostream>
#include <vector>
#include <cassert>

void print_fragments(const std::vector<CHTL::CodeFragment>& fragments) {
    for (const auto& fragment : fragments) {
        std::string type_str;
        switch (fragment.type) {
            case CHTL::FragmentType::CHTL: type_str = "CHTL"; break;
            case CHTL::FragmentType::CSS: type_str = "CSS"; break;
            case CHTL::FragmentType::JS: type_str = "JS"; break;
            case CHTL::FragmentType::CHTL_JS: type_str = "CHTL_JS"; break;
            case CHTL::FragmentType::CHTL_in_CSS: type_str = "CHTL_in_CSS"; break;
        }
        std::cout << "--- Fragment ---" << std::endl;
        std::cout << "Type: " << type_str << std::endl;
        std::cout << "Content: ```\n" << fragment.content << "\n```" << std::endl;
    }
}

void test_pure_chtl() {
    std::cout << "\n--- Running Test: Pure CHTL ---\n";
    std::string source = "div { text: \"Hello\"; }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    assert(fragments.size() == 1);
    assert(fragments[0].type == CHTL::FragmentType::CHTL);
    assert(fragments[0].content.find("div") != std::string::npos);
    std::cout << "PASS\n";
}

void test_top_level_split() {
    std::cout << "\n--- Running Test: Top Level Split ---\n";
    std::string source = "div { } style { color: red; } script { let a = 1; }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    print_fragments(fragments);

    bool has_chtl = false;
    bool has_css = false;
    bool has_chtl_js = false;

    for (const auto& frag : fragments) {
        if (frag.type == CHTL::FragmentType::CHTL && !frag.content.empty() && frag.content.find("div") != std::string::npos) has_chtl = true;
        if (frag.type == CHTL::FragmentType::CSS && frag.content.find("red") != std::string::npos) has_css = true;
        if (frag.type == CHTL::FragmentType::CHTL_JS) has_chtl_js = true;
    }

    assert(has_chtl && has_css && has_chtl_js);
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

    int chtl_js_count = 0;
    int js_count = 0;
    std::string chtl_js_content;
    for(const auto& f : fragments) {
        if (f.type == CHTL::FragmentType::CHTL_JS) {
            chtl_js_count++;
            chtl_js_content = f.content;
        } else if (f.type == CHTL::FragmentType::JS) {
            js_count++;
        }
    }
    assert(chtl_js_count == 1);
    assert(js_count == 2);
    assert(chtl_js_content.find("{{box}}") != std::string::npos);
    assert(chtl_js_content.find("_JS_PLACEHOLDER_") != std::string::npos);
    std::cout << "PASS\n";
}

void test_nested_script_block() {
    std::cout << "\n--- Running Test: Nested Script Block ---\n";
    std::string source = "script { Listen { click: () => { if (true) { {{box}}->show(); } } } }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    print_fragments(fragments);

    int chtl_js_count = 0;
    int js_count = 0;
    std::string chtl_js_content;
     for(const auto& f : fragments) {
        if (f.type == CHTL::FragmentType::CHTL_JS) {
            chtl_js_count++;
            chtl_js_content = f.content;
        } else if (f.type == CHTL::FragmentType::JS) {
            js_count++;
        }
    }
    assert(chtl_js_count == 1);
    assert(js_count > 1); // Expecting multiple JS parts to be extracted
    assert(chtl_js_content.find("Listen") != std::string::npos);
    assert(chtl_js_content.find("{{box}}") != std::string::npos);
    assert(chtl_js_content.find("click:") == std::string::npos); // The JS part should be in a placeholder
    std::cout << "PASS\n";
}


int main() {
    try {
        test_pure_chtl();
        test_top_level_split();
        test_style_block_processing();
        test_simple_script_block();
        test_nested_script_block();
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
