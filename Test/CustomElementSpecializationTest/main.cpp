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
    std::string file_path = "Test/CustomElementSpecializationTest/main.chtl";
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

    // The specialized style "color: red" should be applied to the div with class "title".
    std::string expected_div = R"(
        <div class="title" style="color:red;font-size:24px;">DefaultTitle</div>
    )";

    if (!contains(processed_html, remove_whitespace(expected_div))) {
        std::cerr << "CustomElementSpecializationTest FAILED!" << std::endl;
        std::cerr << "Expected to find: " << remove_whitespace(expected_div) << std::endl;
        std::cerr << "Got HTML: " << processed_html << std::endl;
        return 1;
    }

    std::cout << "CustomElementSpecializationTest PASSED!" << std::endl;

    return 0;
}
