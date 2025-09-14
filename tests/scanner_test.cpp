#include "Scanner/CHTLUnifiedScanner.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <string>
#include <algorithm>
#include <cctype>

// Helper to print raw string content for debugging
void print_raw(const std::string& s) {
    for (char c : s) {
        if (c == '\n') std::cout << "\\n";
        else if (c == '\r') std::cout << "\\r";
        else if (c == '\t') std::cout << "\\t";
        else if (isprint(c)) std::cout << c;
        else std::cout << "\\" << (int)c;
    }
}

void run_scanner_test() {
    std::cout << "Running Unified Scanner Test..." << std::endl;

    std::string source = R"~(
div {
    text: "Hello";
}
style {
    color: red;
}
body {
    script {
        let a = 1;
        {{box}}->show();
        let b = 2;
    }
}
)~";

    CHTL::CHTLUnifiedScanner scanner(source);
    std::vector<CHTL::CodeChunk> chunks = scanner.scan();

    // This is for debugging to get the exact output
    std::cout << "==== ACTUAL CHUNKS ====\n";
    for(size_t i = 0; i < chunks.size(); ++i) {
        std::cout << "Chunk " << i << " type " << (int)chunks[i].type << ":\n---\n";
        print_raw(chunks[i].content);
        std::cout << "\n---\n";
    }
    std::cout << "=======================\n";


    // Expected chunks - to be updated after we see the actual output
    std::vector<CHTL::CodeChunk> expected_chunks = {
        {CHTL::ChunkType::CHTL, "\ndiv {\n    text: \"Hello\";\n}\n"},
        {CHTL::ChunkType::Css, "\n    color: red;\n"},
        {CHTL::ChunkType::CHTL, "\nbody {\n    "},
        {CHTL::ChunkType::ChtlJs, "_JS_PLACEHOLDER_0_{{box}}->show();_JS_PLACEHOLDER_1_"},
        {CHTL::ChunkType::CHTL, "\n}\n"}
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
