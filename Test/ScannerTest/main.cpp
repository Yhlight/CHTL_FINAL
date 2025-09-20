#include "../../CHTL/Scanner/CHTLUnifiedScanner.h"
#include <iostream>
#include <vector>
#include <cassert>

void printFragments(const std::vector<CHTL::CodeFragment>& fragments) {
    for (const auto& fragment : fragments) {
        std::cout << "Type: ";
        switch (fragment.type) {
            case CHTL::FragmentType::CHTL: std::cout << "CHTL"; break;
            case CHTL::FragmentType::CSS: std::cout << "CSS"; break;
            case CHTL::FragmentType::JS: std::cout << "JS"; break;
            case CHTL::FragmentType::CHTL_JS: std::cout << "CHTL_JS"; break;
        }
        std::cout << ", Line: " << fragment.start_line << std::endl;
        std::cout << "--- Content ---" << std::endl;
        std::cout << fragment.content << std::endl;
        std::cout << "---------------" << std::endl;
    }
}

void test_only_chtl() {
    std::cout << "--- Running Test: test_only_chtl ---" << std::endl;
    std::string source = "div { text: \"Hello CHTL\"; }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    printFragments(fragments);
    assert(fragments.size() == 1);
    assert(fragments[0].type == CHTL::FragmentType::CHTL);
    std::cout << "--- Test Passed ---" << std::endl;
}

void test_chtl_and_css() {
    std::cout << "--- Running Test: test_chtl_and_css ---" << std::endl;
    std::string source = "style { color: red; @Style MyTemplate; background: blue; }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    printFragments(fragments);
    assert(fragments.size() == 3);
    assert(fragments[0].type == CHTL::FragmentType::CSS);
    assert(fragments[1].type == CHTL::FragmentType::CHTL);
    assert(fragments[2].type == CHTL::FragmentType::CSS);
    std::cout << "--- Test Passed ---" << std::endl;
}

void test_script_with_only_js() {
    std::cout << "--- Running Test: test_script_with_only_js ---" << std::endl;
    std::string source = "script { let x = 10; console.log(x); }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    printFragments(fragments);
    assert(fragments.size() == 1);
    assert(fragments[0].type == CHTL::FragmentType::JS);
    std::cout << "--- Test Passed ---" << std::endl;
}

void test_script_with_only_chtl_js() {
    std::cout << "--- Running Test: test_script_with_only_chtl_js ---" << std::endl;
    std::string source = "script { {{my_button}}->click(); Listen { event: handle; }; }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    printFragments(fragments);
    assert(fragments.size() >= 2); // Expect at least two CHTL_JS fragments
    assert(fragments[0].type == CHTL::FragmentType::CHTL_JS);
    assert(fragments[1].type == CHTL::FragmentType::CHTL_JS);
    std::cout << "--- Test Passed ---" << std::endl;
}

void test_mixed_script() {
    std::cout << "--- Running Test: test_mixed_script ---" << std::endl;
    std::string source = "script { console.log('start'); {{my_element}}->show(); let x = 1; }";
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    printFragments(fragments);
    assert(fragments.size() == 3);
    assert(fragments[0].type == CHTL::FragmentType::JS);
    assert(fragments[1].type == CHTL::FragmentType::CHTL_JS);
    assert(fragments[2].type == CHTL::FragmentType::JS);
    std::cout << "--- Test Passed ---" << std::endl;
}


int main() {
    test_only_chtl();
    test_chtl_and_css();
    test_script_with_only_js();
    test_script_with_only_chtl_js();
    test_mixed_script();

    std::cout << "\nAll scanner tests passed!" << std::endl;

    return 0;
}
