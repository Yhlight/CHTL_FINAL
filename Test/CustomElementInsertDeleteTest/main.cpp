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
    std::string file_path = "Test/CustomElementInsertDeleteTest/main.chtl";
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

    // Test delete
    std::string expected_delete = "<ul><li>Item1</li><li>Item3</li></ul>";
    assert(contains(processed_html, expected_delete));

    // Test insert after
    std::string expected_insert_after = "<ul><li>Item1</li><li>Item1.5</li><li>Item2</li><li>Item3</li></ul>";
    assert(contains(processed_html, expected_insert_after));

    // Test insert before
    std::string expected_insert_before = "<ul><li>Item0.5</li><li>Item1</li><li>Item2</li><li>Item3</li></ul>";
    assert(contains(processed_html, expected_insert_before));

    // Test replace
    std::string expected_replace = "<ul><li>Item1</li><li>Item2</li><li>Item3wasreplaced</li></ul>";
    assert(contains(processed_html, expected_replace));


    std::cout << "CustomElementInsertDeleteTest PASSED!" << std::endl;

    return 0;
}
