#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../CHTL/CompilerDispatcher.h"
#include "../../CHTL/Config/Configuration.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>
#include <memory>
#include <stdexcept>

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

void test_success_case() {
    std::cout << "--- Testing Custom Style Success Case ---" << std::endl;
    std::string file_path = "Test/CustomStyleTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    // Manually remove the failing span block for the success test
    size_t span_pos = source.find("span");
    if (span_pos != std::string::npos) {
        source = source.substr(0, span_pos);
        // also remove the closing brace of the body and html
        source += "}}";
    }

    auto config = std::make_shared<CHTL::Configuration>();
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    std::string expected_html = R"(
        <p style="border:2px dotted red;padding:10px;color:blue;font-size:20px;"></p>
    )";
    std::string processed_html_result = remove_whitespace(result.html);
    std::string processed_html_expected = remove_whitespace(expected_html);

    assert(processed_html_result == processed_html_expected);
    std::cout << "Success case PASSED!" << std::endl;
}

void test_failure_case() {
    std::cout << "--- Testing Custom Style Failure Case ---" << std::endl;
    std::string file_path = "Test/CustomStyleTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    auto config = std::make_shared<CHTL::Configuration>();
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();
    CHTL::CompilerDispatcher dispatcher(config, file_path);

    bool exception_thrown = false;
    try {
        dispatcher.dispatch(fragments);
    } catch (const std::runtime_error& e) {
        exception_thrown = true;
        std::string message = e.what();
        std::cout << "Caught expected exception: " << message << std::endl;
        assert(std::string(message).find("Valueless property 'font-size' was not provided a value.") != std::string::npos);
    }

    assert(exception_thrown);
    std::cout << "Failure case PASSED!" << std::endl;
}

int main() {
    test_success_case();
    test_failure_case();

    std::cout << "CustomStyleTest PASSED!" << std::endl;

    return 0;
}
