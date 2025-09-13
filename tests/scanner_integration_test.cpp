#include "Scanner/CHTLUnifiedScanner.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

// Helper function to convert ChunkType to string for printing
std::string chunkTypeToString(CHTL::ChunkType type) {
    switch (type) {
        case CHTL::ChunkType::CHTL: return "CHTL";
        case CHTL::ChunkType::JavaScript: return "JavaScript";
        case CHTL::ChunkType::Css: return "Css";
        case CHTL::ChunkType::ChtlJs: return "ChtlJs";
        case CHTL::ChunkType::Placeholder: return "Placeholder";
        default: return "Unknown";
    }
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl>" << std::endl;
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

    // We need to access the placeholder map, but it's private.
    // For this test, we'll just have to trust the processed ChtlJs content is correct.
    // A better approach in a real project would be to make the map accessible for testing,
    // or log it from within the scanner in a debug mode.

    CHTL::CHTLUnifiedScanner scanner(source);
    std::vector<CHTL::CodeChunk> chunks = scanner.scan();

    std::cout << "--- SCANNER CHUNKS ---" << std::endl;
    for (const auto& chunk : chunks) {
        std::cout << "--- CHUNK (" << chunkTypeToString(chunk.type) << ") ---" << std::endl;
        std::cout << chunk.content << std::endl;
    }
    std::cout << "--- END SCANNER CHUNKS ---" << std::endl;

    // In a future step, we would also print the placeholder map to verify it.
    // For now, this output is sufficient to create a snapshot.

    return 0;
}
