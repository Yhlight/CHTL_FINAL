#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../CHTL/CompilerDispatcher.h"
#include "../../CHTL/CodeMerger.h"
#include "../../CHTL/Config/Configuration.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>
#include <memory>

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
    std::string file_path = "Test/FullGenTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    // Create a shared configuration object
    auto config = std::make_shared<CHTL::Configuration>();

    // 1. Scanner
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    // 2. Dispatcher
    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    // 3. Assert
    // Note: The CodeMerger is not strictly needed for this test, as we want to
    // verify the separate outputs of the compilation process.

    std::string expected_html = R"(
        <!DOCTYPE html>
        <div class="box" style="height: 200px;">Hello World</div>
    )";

    std::string expected_css = R"(
        .box {
          color: red;
        }
    )";

    std::string processed_html_result = remove_whitespace(result.html);
    std::string processed_html_expected = remove_whitespace(expected_html);
    assert(processed_html_result == processed_html_expected);

    std::string processed_css_result = remove_whitespace(result.css);
    std::string processed_css_expected = remove_whitespace(expected_css);
    assert(processed_css_result == processed_css_expected);

    assert(result.js.empty());

    std::cout << "FullGenTest PASSED!" << std::endl;

    return 0;
}
