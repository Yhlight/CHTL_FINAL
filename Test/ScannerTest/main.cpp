#include "../../CHTL/Scanner/CHTLUnifiedScanner.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <vector>
#include <string>

// Helper function to convert FragmentType to string for printing
std::string fragmentTypeToString(CHTL::FragmentType type) {
    switch (type) {
        case CHTL::FragmentType::CHTL:           return "CHTL";
        case CHTL::FragmentType::CSS:            return "CSS";
        case CHTL::FragmentType::JS:             return "JS";
        case CHTL::FragmentType::CHTL_JS:        return "CHTL_JS";
        case CHTL::FragmentType::CHTL_in_CSS:    return "CHTL_in_CSS";
        case CHTL::FragmentType::JS_PLACEHOLDER: return "JS_PLACEHOLDER";
        default:                                 return "UNKNOWN";
    }
}

void run_scanner_test(const std::string& test_file_path) {
    std::cout << "=========================================================\n";
    std::cout << "Running Scanner Test for: " << test_file_path << "\n";
    std::cout << "=========================================================\n\n";

    std::string content;
    try {
        // Corrected the namespace and function name casing
        content = CHTL::FileSystem::readFile(test_file_path);
    } catch (const std::runtime_error& e) {
        std::cerr << "Error reading file: " << e.what() << std::endl;
        return;
    }

    CHTL::CHTLUnifiedScanner scanner(content);
    std::vector<CHTL::CodeFragment> fragments = scanner.scan();

    for (const auto& fragment : fragments) {
        std::cout << "---------------------------------------------------------\n";
        std::cout << "Fragment Type: " << fragmentTypeToString(fragment.type) << "\n";
        if (fragment.type == CHTL::FragmentType::JS_PLACEHOLDER) {
            std::cout << "Placeholder ID: " << fragment.placeholder_id << "\n";
        }
        std::cout << "Content:\n" << fragment.content << "\n";
        std::cout << "---------------------------------------------------------\n\n";
    }
}


int main() {
    std::vector<std::string> test_files = {
        "Test/ScannerTest/advanced_scanner_test.chtl"
    };

    for (const auto& file_path : test_files) {
        run_scanner_test(file_path);
    }

    return 0;
}
