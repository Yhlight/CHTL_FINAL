#include "Scanner/CHTLUnifiedScanner.h"
#include <iostream>
#include <vector>
#include <cassert>

// Helper function to compare chunks
bool compareChunks(const std::vector<CHTL::CodeChunk>& actual, const std::vector<CHTL::CodeChunk>& expected) {
    if (actual.size() != expected.size()) {
        std::cerr << "TEST FAILED: Mismatched chunk count. Expected " << expected.size() << ", Got " << actual.size() << std::endl;
        return false;
    }

    for (size_t i = 0; i < actual.size(); ++i) {
        if (actual[i].type != expected[i].type) {
            std::cerr << "TEST FAILED: Mismatched chunk type at index " << i << ". Expected " << (int)expected[i].type << ", Got " << (int)actual[i].type << std::endl;
            return false;
        }
        if (actual[i].content != expected[i].content) {
            std::cerr << "TEST FAILED: Mismatched chunk content at index " << i << "." << std::endl;
            std::cerr << "Expected: <<<" << expected[i].content << ">>>" << std::endl;
            std::cerr << "Got:      <<<" << actual[i].content << ">>>" << std::endl;
            return false;
        }
    }

    return true;
}


int main() {
    std::string source = R"~(
div {
    text: "Hello CHTL";

    style {
        color: red;
        font-size: 16px;
    }

    script {
        let a = 10;
        console.log("Setup");

        listen {
            click: () => { console.log("Listen fired!"); }
        }

        const my_element = {{.my-class}};

        animate {
            target: my_element,
            duration: 1000
        }

        my_element.addEventListener('click', () => {
            console.log("Clicked!");
        });
    }
}
    )~";

    CHTL::CHTLUnifiedScanner scanner(source);
    std::vector<CHTL::CodeChunk> actual_chunks = scanner.scan();

    // Define the expected output
    std::vector<CHTL::CodeChunk> expected_chunks = {
        {CHTL::ChunkType::CHTL, "\ndiv {\n    text: \"Hello CHTL\";\n    \n    style {"},
        {CHTL::ChunkType::Css, "\n        color: red;\n        font-size: 16px;\n    "},
        {CHTL::ChunkType::CHTL, "}\n\n    script {"},
        {CHTL::ChunkType::ChtlJs, "__JS_PLACEHOLDER_0__listen {\n            click: () => { console.log(\"Listen fired!\"); }\n        }__JS_PLACEHOLDER_1__{{.my-class}}__JS_PLACEHOLDER_2__animate {\n            target: my_element,\n            duration: 1000\n        }__JS_PLACEHOLDER_3__"},
        {CHTL::ChunkType::CHTL, "}\n}\n    "}
    };

    if (compareChunks(actual_chunks, expected_chunks)) {
        std::cout << "TEST PASSED" << std::endl;
        return 0;
    } else {
        std::cout << "TEST FAILED" << std::endl;
        return 1;
    }
}
