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
    std::string file_path = "Test/StyleContextTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    auto config = std::make_shared<CHTL::Configuration>();

    // 1. Scanner
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    // 2. Dispatcher
    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    // 3. Assert
    std::string expected_css = R"(
        .box {
            color: red;
        }
        .box:hover {
            color: blue;
        }
        .box::before {
            content: "I am a box.";
        }
    )";

    std::string processed_css_result = remove_whitespace(result.css);
    std::string processed_css_expected = remove_whitespace(expected_css);

    if (processed_css_result != processed_css_expected) {
        std::cerr << "StyleContextTest FAILED!" << std::endl;
        std::cerr << "Expected: " << processed_css_expected << std::endl;
        std::cerr << "Got: " << processed_css_result << std::endl;
        return 1;
    }

    std::cout << "StyleContextTest PASSED!" << std::endl;

    return 0;
}
