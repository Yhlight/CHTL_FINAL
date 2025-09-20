#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../CHTL/CompilerDispatcher.h"
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
    std::string file_path = "Test/AttributeRefTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    auto config = std::make_shared<CHTL::Configuration>();

    // 1. Scanner
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    // 2. Dispatcher
    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    // 3. Assert
    std::string expected_html = R"(
        <html>
            <head></head>
            <body>
                <div id="box1" style="width:150px;"></div>
                <div class="container">
                    <div id="box2" style="width:150px;height:75px;"></div>
                </div>
                <span>first</span>
                <span id="second-span"></span>
            </body>
        </html>
    )";
    // Note: The font-size property on the second span is not included because it
    // references a property that doesn't exist on the target span, so the evaluator
    // throws an error for that property, but the rest of the compilation succeeds.
    // This is the expected behavior for now.

    std::string processed_html_result = remove_whitespace(result.html);
    std::string processed_html_expected = remove_whitespace(expected_html);

    if (processed_html_result != processed_html_expected) {
        std::cerr << "AttributeRefTest FAILED!" << std::endl;
        std::cerr << "Expected: " << processed_html_expected << std::endl;
        std::cerr << "Got: " << processed_html_result << std::endl;
        return 1;
    }

    std::cout << "AttributeRefTest PASSED!" << std::endl;

    return 0;
}
