#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <cassert>
#include <string>

#include "Scanner/CHTLUnifiedScanner.h"

// Helper to convert ChunkType to string for printing
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

void runScannerTest() {
    std::cout << "Running Unified Scanner Test..." << std::endl;

    std::ifstream t("tests/scanner_test.chtl");
    if (!t.is_open()) {
        std::cerr << "Failed to open tests/scanner_test.chtl" << std::endl;
        exit(1);
    }
    std::stringstream buffer;
    buffer << t.rdbuf();
    std::string test_source = buffer.str();

    CHTL::CHTLUnifiedScanner scanner(test_source);
    std::vector<CHTL::CodeChunk> chunks = scanner.scan();

    // Print generated chunks for debugging
    std::cout << "Generated " << chunks.size() << " chunks:" << std::endl;
    for (size_t i = 0; i < chunks.size(); ++i) {
        std::cout << i << ": " << chunkTypeToString(chunks[i].type) << " -> \"";
        for (char c : chunks[i].content) {
            if (c == '\n') std::cout << "\\n";
            else std::cout << c;
        }
        std::cout << "\"" << std::endl;
    }

    // Expected chunks:
    // 0. CHTL: before style
    // 1. CSS: content of style
    // 2. CHTL: between style and script
    // 3. Placeholder (JS)
    // 4. ChtlJs ({{.box}})
    // 5. Placeholder (JS)
    // 6. ChtlJs (listen {...})
    // 7. Placeholder (JS)
    // 8. CHTL (final '}')

    assert(chunks.size() == 9);
    assert(chunks[0].type == CHTL::ChunkType::CHTL);
    assert(chunks[1].type == CHTL::ChunkType::Css);
    assert(chunks[2].type == CHTL::ChunkType::CHTL);
    assert(chunks[3].type == CHTL::ChunkType::Placeholder);
    assert(chunks[4].type == CHTL::ChunkType::ChtlJs);
    assert(chunks[5].type == CHTL::ChunkType::Placeholder);
    assert(chunks[6].type == CHTL::ChunkType::ChtlJs);
    assert(chunks[7].type == CHTL::ChunkType::Placeholder);
    assert(chunks[8].type == CHTL::ChunkType::CHTL);

    assert(chunks[1].content.find("body {") != std::string::npos);
    assert(chunks[4].content == "{{.box}}");
    assert(chunks[6].content.find("listen {") == 0);
    assert(chunks[8].content.find("}") != std::string::npos);

    std::cout << "Unified Scanner Test PASSED" << std::endl;
}

int main() {
    try {
        runScannerTest();
    } catch (const std::exception& e) {
        std::cerr << "Test FAILED: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
