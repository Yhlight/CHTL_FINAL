#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>

// Helper to remove whitespace for comparison
std::string remove_whitespace(const std::string& str) {
    std::string out;
    out.reserve(str.size());
    for (char c : str) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            out += c;
        }
    }
    return out;
}


int main() {
    std::string file_path = "Test/ScannerRobustnessTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    // 1. Scanner
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    // 2. Assert on the fragments
    // We expect 4 fragments in total:
    // 1. CHTL: "script {"
    // 2. JS: the variable declaration
    // 3. CHTL_JS: the Listen block
    // 4. CHTL: "}"

    assert(fragments.size() == 4);

    const auto& js_fragment = fragments[1];
    const auto& chtl_js_fragment = fragments[2];

    assert(js_fragment.type == CHTL::FragmentType::JS);
    assert(chtl_js_fragment.type == CHTL::FragmentType::CHTL_JS);

    std::string expected_js = "var Listen = { event: 'click' }; console.log(Listen.event);";
    std::string expected_chtl_js = "Listen { click: () => { console.log(\"Real listener\"); } }";

    assert(remove_whitespace(js_fragment.content) == remove_whitespace(expected_js));
    assert(remove_whitespace(chtl_js_fragment.content) == remove_whitespace(expected_chtl_js));

    std::cout << "ScannerRobustnessTest PASSED!" << std::endl;

    return 0;
}
