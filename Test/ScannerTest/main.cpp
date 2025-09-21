#include "Scanner/CHTLUnifiedScanner.h"
#include "CHTL/Config/Configuration.h"
#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <memory>
#include <numeric>
#include <algorithm>

// --- Test Utilities ---
bool has_fragment_of_type(const std::vector<CHTL::CodeFragment>& fragments, CHTL::FragmentType type) {
    return std::any_of(fragments.begin(), fragments.end(), [&](const CHTL::CodeFragment& f) {
        return f.type == type;
    });
}

std::string get_first_fragment_content(const std::vector<CHTL::CodeFragment>& fragments, CHTL::FragmentType type) {
    auto it = std::find_if(fragments.begin(), fragments.end(), [&](const CHTL::CodeFragment& f) {
        return f.type == type;
    });
    if (it != fragments.end()) {
        return it->content;
    }
    return "";
}

std::string get_all_fragments_of_type(const std::vector<CHTL::CodeFragment>& fragments, CHTL::FragmentType type) {
    std::string combined_content;
    for(const auto& f : fragments) {
        if(f.type == type) {
            combined_content += f.content;
        }
    }
    return combined_content;
}

// --- Test Cases ---

void test_style_block_scanning() {
    std::cout << "--- Running Test: Style Block Scanning ---\n";
    std::string source = R"(
        style {
            div { font-size: 16px; }
        }
        script { let a = 1; }
    )";

    auto config = std::make_shared<CHTL::Configuration>();
    CHTL::CHTLUnifiedScanner scanner(source, config);
    auto fragments = scanner.scan();

    assert(has_fragment_of_type(fragments, CHTL::FragmentType::CSS));
    assert(has_fragment_of_type(fragments, CHTL::FragmentType::JS));

    std::string css_content = get_first_fragment_content(fragments, CHTL::FragmentType::CSS);
    assert(css_content.find("div { font-size: 16px; }") != std::string::npos);

    std::cout << "PASS\n";
}

void test_complex_script_scanning() {
    std::cout << "--- Running Test: Complex Script Scanning ---\n";
    std::string source = R"(
        script {
            function myFunc() {
                Listen {
                    click: () => { {{button}}->fireEvent(); }
                }
            }
        }
    )";

    auto config = std::make_shared<CHTL::Configuration>();
    CHTL::CHTLUnifiedScanner scanner(source, config);
    auto fragments = scanner.scan();

    std::string js_content = get_all_fragments_of_type(fragments, CHTL::FragmentType::JS);
    std::string chtl_js_content = get_all_fragments_of_type(fragments, CHTL::FragmentType::CHTL_JS);

    assert(js_content.find("function myFunc()") != std::string::npos);
    assert(js_content.find("() => {") != std::string::npos);
    assert(js_content.find("fireEvent();") != std::string::npos);
    assert(chtl_js_content.find("Listen") != std::string::npos);
    assert(chtl_js_content.find("{{button}}") != std::string::npos);
    assert(chtl_js_content.find("->") != std::string::npos);

    std::cout << "PASS\n";
}

void test_vir_and_operator_scanning() {
    std::cout << "--- Running Test: Vir and &-> Operator Scanning ---\n";
    std::string source = R"(
        script {
            Vir myVar = Listen { };
            {{box}} &-> click: myFunc;
        }
    )";

    auto config = std::make_shared<CHTL::Configuration>();
    CHTL::CHTLUnifiedScanner scanner(source, config);
    auto fragments = scanner.scan();

    std::string chtl_js_content = get_all_fragments_of_type(fragments, CHTL::FragmentType::CHTL_JS);

    assert(chtl_js_content.find("Vir myVar =") != std::string::npos);
    assert(chtl_js_content.find("Listen {") != std::string::npos);
    assert(chtl_js_content.find("{{box}}") != std::string::npos);
    assert(chtl_js_content.find("&->") != std::string::npos);

    std::cout << "PASS\n";
}

void test_configuration_integration() {
    std::cout << "--- Running Test: Configuration Integration ---\n";
    std::string source = R"(
        script {
            OnEvent {
                click: doSomething
            }
        }
    )";

    auto config = std::make_shared<CHTL::Configuration>();
    config->keyword_map["CHTLJS_LISTEN"] = {"OnEvent"};

    CHTL::CHTLUnifiedScanner scanner(source, config);
    auto fragments = scanner.scan();

    std::string chtl_js_content = get_all_fragments_of_type(fragments, CHTL::FragmentType::CHTL_JS);
    assert(chtl_js_content.find("OnEvent") != std::string::npos);

    std::cout << "PASS\n";
}


int main() {
    try {
        test_style_block_scanning();
        test_complex_script_scanning();
        test_vir_and_operator_scanning();
        test_configuration_integration();
    } catch (const std::exception& e) {
        std::cerr << "A test failed with exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "A test failed with an unknown exception." << std::endl;
        return 1;
    }

    std::cout << "\nAll scanner tests passed!\n";
    return 0;
}
