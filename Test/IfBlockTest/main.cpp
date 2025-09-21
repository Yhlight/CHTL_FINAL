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
    std::string file_path = "Test/IfBlockTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    // Create a shared configuration object
    auto config = std::make_shared<CHTL::Configuration>();
    // Add default keywords for the test
    config->keyword_map["KEYWORD_IF"] = {"if"};
    config->keyword_map["KEYWORD_ELSE"] = {"else"};
    config->keyword_map["KEYWORD_CONDITION"] = {"condition"};


    // 1. Scanner
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    // 2. Dispatcher
    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    // 3. Merger
    std::string final_html = CHTL::CodeMerger::mergeToFinalHtml(result.html, result.css, result.js);

    // 4. Assert
    std::string expected_final_html = R"(
        <html>
            <head>
                <title>If Block Test</title>
            </head>
            <body>
                <div id="test1">Test 1 Passed</div>
                <div id="test3-else">Test 3 Passed</div>
                <div id="test4-elseif">Test 4 Passed</div>
                <div id="test5-outer">
                    <span id="test5-inner">Test 5 Passed</span>
                </div>
            </body>
        </html>
    )";

    std::string processed_result = remove_whitespace(final_html);
    std::string processed_expected = remove_whitespace(expected_final_html);

    if (processed_result != processed_expected) {
        std::cerr << "IfBlockTest FAILED!" << std::endl;
        std::cerr << "Expected: " << processed_expected << std::endl;
        std::cerr << "Got:      " << final_html << std::endl; // Print with whitespace for readability
        return 1;
    }

    std::cout << "IfBlockTest PASSED!" << std::endl;

    return 0;
}
