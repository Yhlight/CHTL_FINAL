#include "../../CHTL/CompilerDispatcher.h"
#include "../../CHTL/Config/Configuration.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>
#include <memory>
#include <vector>

// Helper function to remove all whitespace from a string for robust comparison
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
    std::string file_path = "Test/CHTLJSGenTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    // Create a shared configuration object
    auto config = std::make_shared<CHTL::Configuration>();

    // Manually create a CHTL_JS fragment to simulate the scanner's output
    std::vector<CHTL::CodeFragment> fragments = {
        {source, CHTL::FragmentType::CHTL_JS}
    };

    // Dispatch the fragment
    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    // Assert the output
    std::string expected_js = R"(
        document.querySelector('.myButton').addEventListener('click', () => { console.log("Button clicked!"); });
    )";

    std::string processed_js_result = remove_whitespace(result.js);
    std::string processed_js_expected = remove_whitespace(expected_js);
    assert(processed_js_result == processed_js_expected);

    // HTML and CSS should be empty
    assert(result.html.empty());
    assert(result.css.empty());

    std::cout << "CHTLJSGenTest PASSED!" << std::endl;

    return 0;
}
