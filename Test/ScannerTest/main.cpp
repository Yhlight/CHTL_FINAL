#include "../../CHTL/Scanner/CHTLUnifiedScanner.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

// Helper function to convert FragmentType to string for printing
std::string fragmentTypeToString(CHTL::FragmentType type) {
    switch (type) {
        case CHTL::FragmentType::CHTL: return "CHTL";
        case CHTL::FragmentType::CSS: return "CSS";
        case CHTL::FragmentType::JS: return "JS";
        case CHTL::FragmentType::CHTL_JS: return "CHTL_JS";
        case CHTL::FragmentType::JS_PLACEHOLDER: return "JS_PLACEHOLDER";
        default: return "UNKNOWN";
    }
}

int main() {
    // Read the test file
    std::ifstream file("Test/ScannerTest/scanner_test.chtl");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open test file." << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    // Create and run the scanner
    CHTL::CHTLUnifiedScanner scanner(source);
    std::vector<CHTL::CodeFragment> fragments = scanner.scan();
    auto placeholders = scanner.getPlaceholders();

    // Print the results
    std::cout << "----------- SCANNER RESULTS -----------" << std::endl;
    for (const auto& fragment : fragments) {
        std::cout << "=======================================" << std::endl;
        std::cout << "Type: " << fragmentTypeToString(fragment.type) << std::endl;
        std::cout << "Line: " << fragment.start_line << std::endl;
        std::cout << "--- Content ---" << std::endl;
        std::cout << fragment.content << std::endl;
        std::cout << "=======================================" << std::endl << std::endl;
    }

    std::cout << "----------- PLACEHOLDER MAPPINGS -----------" << std::endl;
    for (const auto& pair : placeholders) {
        std::cout << "=======================================" << std::endl;
        std::cout << "Placeholder ID: " << pair.first << std::endl;
        std::cout << "--- Original JS Content ---" << std::endl;
        std::cout << pair.second << std::endl;
        std::cout << "=======================================" << std::endl << std::endl;
    }

    return 0;
}
