#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../CHTL/CompilerDispatcher.h"
#include "../../CHTL/Config/Configuration.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <string>
#include <vector>

// Helper to check if a string contains a substring
bool contains(const std::string& haystack, const std::string& needle) {
    return haystack.find(needle) != std::string::npos;
}

int main() {
    std::string input_file = "Test/EventHandlerTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(input_file);
    if (source.empty()) {
        std::cerr << "Test Error: Could not read " << input_file << std::endl;
        return 1;
    }

    auto config = std::make_shared<CHTL::Configuration>();
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    CHTL::CompilerDispatcher dispatcher(config, input_file);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    // --- Verification ---
    int status = 0;

    // Test 1: Listen block
    std::string expected_js1 = "document.querySelector('#myBtn').addEventListener('click', () => {";
    if (!contains(result.js, expected_js1)) {
        std::cerr << "Test Failed: JS output missing expected Listen block code." << std::endl;
        std::cerr << "Expected to find: " << expected_js1 << std::endl;
        status = 1;
    }

    // Test 2: Single event handler
    std::string expected_js2 = "document.querySelector('.box').addEventListener('mouseover', function() {";
    if (!contains(result.js, expected_js2)) {
        std::cerr << "Test Failed: JS output missing expected single event handler code." << std::endl;
        std::cerr << "Expected to find: " << expected_js2 << std::endl;
        status = 1;
    }

    // Test 3: Multi-event handler
    std::string expected_js3a = "document.querySelector('#multi-event-span').addEventListener('mouseenter', (e) => {";
    std::string expected_js3b = "document.querySelector('#multi-event-span').addEventListener('mouseleave', (e) => {";
    if (!contains(result.js, expected_js3a) || !contains(result.js, expected_js3b)) {
        std::cerr << "Test Failed: JS output missing expected multi-event handler code." << std::endl;
        std::cerr << "Expected to find calls for both 'mouseenter' and 'mouseleave'." << std::endl;
        status = 1;
    }

    if (status == 0) {
        std::cout << "EventHandlerTest Passed!" << std::endl;
    } else {
        std::cerr << "\n--- Generated JS ---\n" << result.js << "\n--------------------\n";
    }

    return status;
}
