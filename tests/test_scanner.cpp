#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "../src/Scanner/CHTLUnifiedScanner.h"

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
        CHTL::ScanningResult result = scanner.scan();

        std::cout << "--- MODIFIED SOURCE ---" << std::endl;
        std::cout << result.modified_source << std::endl;
        std::cout << "\n--- PLACEHOLDER MAP ---" << std::endl;
        for (const auto& pair : result.placeholder_map) {
            std::cout << "[[ " << pair.first << " ]]" << " : " << "[[ " << pair.second << " ]]" << std::endl;
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
