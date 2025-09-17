#include <iostream>
#include <fstream>
#include <sstream>
#include "../src/Scanner/CHTLUnifiedScanner.h"

void printHorizontalLine() {
    std::cout << "\n" << std::string(80, '-') << "\n" << std::endl;
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

    std::cout << "--- ORIGINAL SOURCE ---" << std::endl;
    std::cout << source << std::endl;

    try {
        CHTL::CHTLUnifiedScanner scanner(source);
        CHTL::ScannedData result = scanner.scan();

        printHorizontalLine();

        std::cout << "--- SCANNED CHTL (WITH PLACEHOLDERS) ---" << std::endl;
        std::cout << result.chtl_source << std::endl;

        printHorizontalLine();

        std::cout << "--- JS PLACEHOLDER MAP (" << result.js_placeholder_map.size() << " entries) ---" << std::endl;
        for (const auto& pair : result.js_placeholder_map) {
            std::cout << "[[ " << pair.first << " ]]" << "  =>  " << "{" << pair.second << "}" << std::endl;
        }

        printHorizontalLine();

        std::cout << "--- CSS FRAGMENTS (" << result.css_fragments.size() << " entries) ---" << std::endl;
        for (const auto& fragment : result.css_fragments) {
            std::cout << "{" << fragment.content << "}" << std::endl;
        }

        printHorizontalLine();


    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
