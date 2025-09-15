#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "../src/Scanner/CHTLUnifiedScanner.h"

// Function to convert FragmentType enum to a string for printing
std::string fragmentTypeToString(CHTL::FragmentType type) {
    switch (type) {
        case CHTL::FragmentType::CHTL: return "CHTL";
        case CHTL::FragmentType::CSS: return "CSS";
        case CHTL::FragmentType::JS: return "JS";
        case CHTL::FragmentType::CHTL_JS: return "CHTL_JS";
        case CHTL::FragmentType::UNKNOWN: return "UNKNOWN";
        default: return "INVALID";
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    try {
        CHTL::CHTLUnifiedScanner scanner(source);
        CHTL::ScanResult result = scanner.scan();

        std::cout << "--- SCANNER OUTPUT ---" << std::endl;

        std::cout << "=== CHTL with Placeholders ===" << std::endl;
        std::cout << result.chtl_with_placeholders << std::endl;

        std::cout << "\n=== Placeholder Map Contents ===" << std::endl;
        for (const auto& pair : result.placeholder_map) {
            std::cout << "====================" << std::endl;
            std::cout << "PLACEHOLDER: " << pair.first << std::endl;
            std::cout << "TYPE: " << fragmentTypeToString(pair.second.type) << std::endl;
            std::cout << "--------------------" << std::endl;
            std::cout << pair.second.content << std::endl;
        }
        std::cout << "====================" << std::endl;


    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
