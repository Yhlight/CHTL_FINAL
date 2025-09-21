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

int main() {
    std::string file_path = "Test/ImportTest/main.chtl";
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

    std::string expected_html = R"(
        <html>
            <body>
                <button class="my-button">ClickMe</button>
            </body>
        </html>
    )";

    if (processed_html != remove_whitespace(expected_html)) {
        std::cerr << "ImportTest FAILED!" << std::endl;
        std::cerr << "Expected HTML: " << remove_whitespace(expected_html) << std::endl;
        std::cerr << "Got HTML: " << processed_html << std::endl;
        return 1;
    }

    std::cout << "ImportTest PASSED!" << std::endl;

    return 0;
}
