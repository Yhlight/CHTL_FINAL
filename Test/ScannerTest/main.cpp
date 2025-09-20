#include "../../CHTL/Scanner/CHTLUnifiedScanner.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <string>
#include <vector>

// Helper function to convert FragmentType to string for printing
std::string fragmentTypeToString(CHTL::FragmentType type) {
    switch (type) {
        case CHTL::FragmentType::CHTL: return "CHTL";
        case CHTL::FragmentType::CSS: return "CSS";
        case CHTL::FragmentType::JS: return "JS";
        case CHTL::FragmentType::CHTL_JS: return "CHTL_JS";
        default: return "UNKNOWN";
    }
}

int main() {
    std::string test_file_path = "Test/unified_scanner_test.chtl";
    std::string source;
    try {
        source = CHTL::FileSystem::readFile(test_file_path);
        std::cout << "--- Successfully read test file: " << test_file_path << " ---\n" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Error reading test file: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "--- Input CHTL ---\n" << source << "\n------------------\n" << std::endl;

    std::cout << "\n--- Running Unified Scanner ---\n" << std::endl;

    try {
        CHTL::CHTLUnifiedScanner scanner(source);
        std::vector<CHTL::CodeFragment> fragments = scanner.scan();

        std::cout << "--- Scanner Output ---" << std::endl;
        for (const auto& fragment : fragments) {
            std::cout << "========================================\n";
            std::cout << "Type: " << fragmentTypeToString(fragment.type)
                      << " (Line: " << fragment.start_line << ")\n";
            std::cout << "----------------------------------------\n";
            std::cout << fragment.content << "\n";
            std::cout << "========================================\n" << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "An exception occurred during scanning: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
