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

// Helper function to check if a string contains a substring
bool contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

int main() {
    std::string file_path = "Test/ConditionalRenderTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);
    auto config = std::make_shared<CHTL::Configuration>();

    // 1. Scanner
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    // 2. Dispatcher
    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    std::string processed_html = remove_whitespace(result.html);

    std::cout << "Generated HTML: " << result.html << std::endl;

    // 3. Assert
    assert(contains(result.html, "id=\"test-box\"") && contains(result.html, "color: red;"));
    assert(contains(result.html, "id=\"test-box-2\"") && contains(result.html, "color: orange;"));
    assert(contains(result.html, "id=\"test-box-3\"") && contains(result.html, "color: black;"));


    std::cout << "ConditionalRenderTest PASSED!" << std::endl;

    return 0;
}
