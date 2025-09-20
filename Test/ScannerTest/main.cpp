#include "../../CHTL/Scanner/CHTLUnifiedScanner.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cassert>

// Helper to convert FragmentType to string for printing
std::string fragmentTypeToString(CHTL::FragmentType type) {
    switch (type) {
        case CHTL::FragmentType::CHTL: return "CHTL";
        case CHTL::FragmentType::CSS: return "CSS";
        case CHTL::FragmentType::JS: return "JS";
        case CHTL::FragmentType::CHTL_JS: return "CHTL_JS";
        default: return "UNKNOWN";
    }
}

void printFragments(const std::vector<CHTL::CodeFragment>& fragments) {
    std::cout << "--- Scanner Output ---" << std::endl;
    for (size_t i = 0; i < fragments.size(); ++i) {
        const auto& fragment = fragments[i];
        std::cout << "--- Fragment " << i << " ---" << std::endl;
        std::cout << "Type: " << fragmentTypeToString(fragment.type) << std::endl;
        std::cout << "Line: " << fragment.start_line << std::endl;
        std::cout << "Content: <<<" << std::endl << fragment.content << std::endl << ">>>" << std::endl;
    }
    std::cout << "--------------------" << std::endl;
}


int main() {
    std::string path = "Test/ScannerTest/test_scanner.chtl";
    std::ifstream file_stream(path);
    if (!file_stream) {
        std::cerr << "Error: Could not open file " << path << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << file_stream.rdbuf();
    std::string source = buffer.str();

    CHTL::CHTLUnifiedScanner scanner(source);
    std::vector<CHTL::CodeFragment> fragments = scanner.scan();

    printFragments(fragments);

    // Basic assertions to verify the scanner's output
    assert(fragments.size() > 10); // Check that it produced a reasonable number of fragments

    // CHTL -> <style> -> CSS -> </style> -> CHTL -> <script>
    assert(fragments[0].type == CHTL::FragmentType::CHTL);
    assert(fragments[1].type == CHTL::FragmentType::CHTL); // <style>
    assert(fragments[1].content.find("<style") != std::string::npos);
    assert(fragments[2].type == CHTL::FragmentType::CSS);
    assert(fragments[3].type == CHTL::FragmentType::CHTL); // </style>
    assert(fragments[4].type == CHTL::FragmentType::CHTL);
    assert(fragments[5].type == CHTL::FragmentType::CHTL); // <script>

    // Script content fragments
    assert(fragments[6].type == CHTL::FragmentType::JS);
    assert(fragments[7].type == CHTL::FragmentType::CHTL_JS); // {{.box}}
    assert(fragments[7].content.find("{{.box}}") != std::string::npos);
    assert(fragments[8].type == CHTL::FragmentType::JS);
    assert(fragments[9].type == CHTL::FragmentType::CHTL_JS); // Listen {..}
    assert(fragments[9].content.find("Listen") != std::string::npos);
    assert(fragments[10].type == CHTL::FragmentType::JS);
    assert(fragments[11].type == CHTL::FragmentType::CHTL_JS); // $my_class$
    assert(fragments[11].content.find("$my_class$") != std::string::npos);
    assert(fragments[12].type == CHTL::FragmentType::JS);

    // </script> -> CHTL
    assert(fragments[13].type == CHTL::FragmentType::CHTL); // </script>
    assert(fragments[14].type == CHTL::FragmentType::CHTL);

    std::cout << "All basic assertions passed!" << std::endl;

    return 0;
}
