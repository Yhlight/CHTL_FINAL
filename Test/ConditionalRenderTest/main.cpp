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

// Helper to check if a string contains a substring
bool contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

int main() {
    std::string file_path = "Test/ConditionalRenderTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    auto config = std::make_shared<CHTL::Configuration>();
    // Manually configure the 'if' keyword for this test
    config->keyword_map["KEYWORD_IF"] = {"if"};


    // 1. Scanner
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    // 2. Dispatcher
    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    // 3. Assert
    // The final style should have width, height, and the green background from the true condition.
    std::string expected_style_part_1 = "width:600px;";
    std::string expected_style_part_2 = "height:100px;";
    std::string expected_style_part_3 = "background-color:green;";
    std::string unexpected_style_part = "background-color:red;";

    std::string processed_html_result = remove_whitespace(result.html);

    bool pass = true;

    if (!contains(processed_html_result, expected_style_part_1)) {
        std::cerr << "ConditionalRenderTest FAILED: Missing expected style '" << expected_style_part_1 << "'" << std::endl;
        pass = false;
    }
    if (!contains(processed_html_result, expected_style_part_2)) {
        std::cerr << "ConditionalRenderTest FAILED: Missing expected style '" << expected_style_part_2 << "'" << std::endl;
        pass = false;
    }
    if (!contains(processed_html_result, expected_style_part_3)) {
        std::cerr << "ConditionalRenderTest FAILED: Missing expected style '" << expected_style_part_3 << "'" << std::endl;
        pass = false;
    }
    if (contains(processed_html_result, unexpected_style_part)) {
        std::cerr << "ConditionalRenderTest FAILED: Found unexpected style '" << unexpected_style_part << "'" << std::endl;
        pass = false;
    }

    if (!pass) {
        std::cerr << "Full generated HTML: " << result.html << std::endl;
        return 1;
    }


    std::cout << "ConditionalRenderTest PASSED!" << std::endl;

    return 0;
}
