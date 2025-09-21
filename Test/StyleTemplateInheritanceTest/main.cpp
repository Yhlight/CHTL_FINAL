#include "../../CHTL/CompilerDispatcher.h"
#include "../../CHTL/CodeMerger.h"
#include "../../CHTL/Config/Configuration.h"
#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>
#include <memory>

// Helper function to remove all whitespace from a string for robust comparison
std::string remove_whitespace(std::string str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
    return str;
}

// Helper function to check if a string contains a substring
bool contains(const std::string& str, const std::string& sub) {
    return str.find(sub) != std::string::npos;
}

int main() {
    std::string file_path = "Test/StyleTemplateInheritanceTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    auto config = std::make_shared<CHTL::Configuration>();

    // 1. Scanner
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    // 2. Dispatcher
    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    // 3. Assert HTML Output
    std::string processed_html = remove_whitespace(result.html);

    // The order of styles is not guaranteed because of map iteration.
    // So we check for the presence of each expected style.
    assert(contains(processed_html, "class=\"test-div\""));
    assert(contains(processed_html, "style=\""));
    assert(contains(processed_html, "color:black;"));
    assert(contains(processed_html, "font-size:20px;"));
    assert(contains(processed_html, "padding:10px;"));
    assert(contains(processed_html, "background-color:lightgray;"));
    assert(contains(processed_html, "border:1pxsolidblack;"));


    std::cout << "StyleTemplateInheritanceTest PASSED!" << std::endl;

    return 0;
}
