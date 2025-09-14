#include "Scanner/CHTLUnifiedScanner.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <string>

void run_scanner_test() {
    std::cout << "Running Unified Scanner Test..." << std::endl;

    std::string source = R"~(
        console.log("Setup");
        {{.container}}->hide();
        console.log("Done");
    )~";

    CHTL::CHTLUnifiedScanner scanner(source);
    std::vector<CHTL::CodeChunk> chunks = scanner.scan();

    std::vector<CHTL::CodeChunk> expected_chunks = {
        {CHTL::ChunkType::JavaScript, "\n        console.log(\"Setup\");\n        "},
        {CHTL::ChunkType::ChtlJs, "{{.container}}->hide();"},
        {CHTL::ChunkType::JavaScript, "\n        console.log(\"Done\");\n    "}
    };

    assert(chunks.size() == expected_chunks.size());

    for (size_t i = 0; i < chunks.size(); ++i) {
        assert(chunks[i].type == expected_chunks[i].type);
        assert(chunks[i].content == expected_chunks[i].content);
    }

    std::cout << "Unified Scanner Test Passed!" << std::endl;
}

int main() {
    try {
        run_scanner_test();
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
    return 0;
}
