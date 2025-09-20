#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../CHTL/Config/Configuration.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include <memory>
#include <algorithm>

// Helper to find a fragment by type and assert its existence
const CHTL::CodeFragment& find_fragment(const std::vector<CHTL::CodeFragment>& fragments, CHTL::FragmentType type, const std::string& contains) {
    auto it = std::find_if(fragments.begin(), fragments.end(), [&](const CHTL::CodeFragment& frag) {
        return frag.type == type && frag.content.find(contains) != std::string::npos;
    });
    assert(it != fragments.end());
    return *it;
}

int main() {
    std::string file_path = "Test/ScannerFixTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);
    auto config = std::make_shared<CHTL::Configuration>();

    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    // --- Verification ---
    std::cout << "Verifying scanner output..." << std::endl;

    // 1. Verify the pure CSS fragment from the <style> block
    const auto& css_fragment = find_fragment(fragments, CHTL::FragmentType::CSS, "font-family");
    assert(css_fragment.content.find("sans-serif") != std::string::npos);
    std::cout << "CSS fragment verified." << std::endl;

    // 2. Verify the JS fragment for the Listen block's callback function
    const auto& js_placeholder_fragment = find_fragment(fragments, CHTL::FragmentType::JS, "console.log");
    assert(!js_placeholder_fragment.placeholder_id.empty());
    std::cout << "JS placeholder fragment verified." << std::endl;

    // 3. Verify the CHTL_JS fragment for the Listen block contains the placeholder
    const auto& listen_fragment = find_fragment(fragments, CHTL::FragmentType::CHTL_JS, "Listen");
    assert(listen_fragment.content.find(js_placeholder_fragment.placeholder_id) != std::string::npos);
    std::cout << "CHTL_JS Listen fragment verified." << std::endl;

    // 4. Verify the CHTL_JS fragment for the enhanced selector
    const auto& selector_fragment = find_fragment(fragments, CHTL::FragmentType::CHTL_JS, "{{#test-div}}");
    assert(selector_fragment.content.find("test-div") != std::string::npos);
    std::cout << "CHTL_JS selector fragment verified." << std::endl;

    // 5. Verify a pure JS fragment
    const auto& js_fragment = find_fragment(fragments, CHTL::FragmentType::JS, "const myDiv");
    assert(js_fragment.placeholder_id.empty()); // This is a top-level JS fragment, no placeholder ID
    std::cout << "Pure JS fragment verified." << std::endl;

    std::cout << "ScannerFixTest PASSED!" << std::endl;

    return 0;
}
