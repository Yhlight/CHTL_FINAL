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

    // 3. Merger
    std::string final_html = CHTL::CodeMerger::mergeToFinalHtml(result.html, result.css, result.js);

    // Debug output
    std::cout << "HTML: " << result.html << std::endl;
    std::cout << "CSS: " << result.css << std::endl;
    std::cout << "JS: " << result.js << std::endl;

    // 4. Assert
    std::string expected_final_html = R"(
        <!DOCTYPE html>
        <html>
            <head>
                <title>Full Gen Test</title>
                <style>
                    .box {
                        color: red;
                    }
                </style>
            </head>
            <body>
                <div class="box" style="height:200px;">Hello World</div>
            </body>
        </html>
    )";

    std::string processed_result = remove_whitespace(final_html);
    std::string processed_expected = remove_whitespace(expected_final_html);

    if (processed_result != processed_expected) {
        std::cerr << "FullGenTest FAILED!" << std::endl;
        std::cerr << "Expected: " << processed_expected << std::endl;
        std::cerr << "Got:      " << processed_result << std::endl;
        return 1;
    }

    std::cout << "FullGenTest PASSED!" << std::endl;

    return 0;
}
