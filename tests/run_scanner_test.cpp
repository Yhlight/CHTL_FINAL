#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include "Scanner/CHTLUnifiedScanner.h"

// Helper function to convert ChunkType to a string for printing
std::string chunkTypeToString(CHTL::ChunkType type) {
    switch (type) {
        case CHTL::ChunkType::CHTL:       return "CHTL";
        case CHTL::ChunkType::JavaScript: return "JavaScript";
        case CHTL::ChunkType::Css:        return "Css";
        case CHTL::ChunkType::ChtlJs:     return "ChtlJs";
        case CHTL::ChunkType::Placeholder:return "Placeholder";
        default:                          return "Unknown";
    }
}

int main() {
    std::string test_filepath = "tests/scanner_test.chtl";
    std::ifstream file(test_filepath);

    if (!file.is_open()) {
        std::cerr << "FATAL: Could not open test file: " << test_filepath << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source_code = buffer.str();

    std::cout << "--- Running CHTLUnifiedScanner Test ---" << std::endl;
    std::cout << "Input source from " << test_filepath << ":\n" << std::endl;
    std::cout << source_code << std::endl;
    std::cout << "\n--- Scanner Output ---" << std::endl;

    CHTL::CHTLUnifiedScanner scanner(source_code);
    std::vector<CHTL::CodeChunk> chunks = scanner.scan();

    for (const auto& chunk : chunks) {
        std::cout << "========================================" << std::endl;
        std::cout << "Chunk Type: " << chunkTypeToString(chunk.type) << std::endl;
        std::cout << "----------------------------------------" << std::endl;
        std::cout << chunk.content << std::endl;
        std::cout << "========================================" << std::endl;
    }

    std::cout << "\n--- Test Finished ---" << std::endl;

    if (chunks.empty()) {
        std::cerr << "Warning: Scanner produced no chunks." << std::endl;
        return 1;
    }

    return 0;
}
