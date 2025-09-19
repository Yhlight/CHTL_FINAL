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
    std::string file_path = "Test/NamespaceTest/main.chtl";
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
            <body>
                <div style="width:100px;height:100px;background-color:blue;">This is a box from the 'space' namespace.</div>
            </body>
        </html>
    )";

    std::string processed_html_result = remove_whitespace(result.html);
    std::string processed_html_expected = remove_whitespace(expected_html);

    if (processed_html_result != processed_html_expected) {
        std::cerr << "NamespaceTest FAILED!" << std::endl;
        std::cerr << "Expected: " << processed_html_expected << std::endl;
        std::cerr << "Got: " << processed_html_result << std::endl;
        return 1;
    }

    std::cout << "NamespaceTest PASSED!" << std::endl;

    return 0;
}
