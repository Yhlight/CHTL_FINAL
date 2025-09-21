#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../CHTL/Config/Configuration.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <memory>
#include <vector>
#include <cassert>
#include <string>
#include <algorithm>

// Helper to remove all whitespace for comparison
std::string remove_whitespace(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}

int main() {
    std::string file_path = "Test/UnifiedScannerTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    assert(fragments.size() == 7);

    // CHTL before script
    assert(fragments[0].type == CHTL::FragmentType::CHTL);

    // JS Fragments
    assert(fragments[1].type == CHTL::FragmentType::JS);
    assert(fragments[1].placeholder_id == "__JS_PLACEHOLDER_0__");
    assert(remove_whitespace(fragments[1].content) == "leta=1;letb=2;constmyObj=");

    assert(fragments[2].type == CHTL::FragmentType::JS);
    assert(fragments[2].placeholder_id == "__JS_PLACEHOLDER_1__");
    assert(remove_whitespace(fragments[2].content) == ";console.log(myObj->textContent);if(a>b){console.log(\"aisgreater\");}myObj");

    assert(fragments[3].type == CHTL::FragmentType::JS);
    assert(fragments[3].placeholder_id == "__JS_PLACEHOLDER_2__");
    assert(remove_whitespace(fragments[3].content) == ";letmyClass=\"dynamic-class\";");

    // CHTL_JS Fragment
    assert(fragments[4].type == CHTL::FragmentType::CHTL_JS);
    assert(fragments[4].placeholder_id.empty());
    std::string expected_chtl_js = "__JS_PLACEHOLDER_0__{{myDiv}}__JS_PLACEHOLDER_1__&-> click: () => {console.log(\"Clicked!\");}__JS_PLACEHOLDER_2__";
    assert(remove_whitespace(fragments[4].content) == remove_whitespace(expected_chtl_js));

    // CHTL after script
    assert(fragments[5].type == CHTL::FragmentType::CHTL);
    assert(fragments[6].type == CHTL::FragmentType::CHTL);


    std::cout << "UnifiedScannerTest PASSED!" << std::endl;

    return 0;
}
