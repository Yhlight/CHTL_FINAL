#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../CHTL/CompilerDispatcher.h"
#include "../../CHTL/CodeMerger.h"
#include "../../CHTL/Config/Configuration.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <memory>
#include <algorithm>

// Helper function to remove all whitespace from a string for robust comparison
std::string remove_whitespace(const std::string& str) {
    std::string out;
    out.reserve(str.size());
    for (char c : str) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            out += c;
        }
    }
    return out;
}

int main() {
    std::string file_path = "Test/ListenTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    auto config = std::make_shared<CHTL::Configuration>();
    config->keyword_map["KEYWORD_LISTEN"] = {"Listen"};

    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    std::string expected_js1 = "document.querySelector('#myButton').addEventListener('click', () => {console.log(\"Button was clicked!\");});";
    std::string expected_js2 = "document.querySelector('#myButton').addEventListener('mouseover', () => {console.log(\"Mouse over button!\");});";

    std::string processed_js = remove_whitespace(result.js);

    if (processed_js.find(remove_whitespace(expected_js1)) == std::string::npos) {
        std::cerr << "TEST FAILED: Generated JavaScript does not contain click handler.\n";
        std::cerr << "--- Expected to find ---\n" << remove_whitespace(expected_js1) << "\n";
        std::cerr << "--- Got ---\n" << processed_js << "\n";
        return 1;
    }

    if (processed_js.find(remove_whitespace(expected_js2)) == std::string::npos) {
        std::cerr << "TEST FAILED: Generated JavaScript does not contain mouseover handler.\n";
        std::cerr << "--- Expected to find ---\n" << remove_whitespace(expected_js2) << "\n";
        std::cerr << "--- Got ---\n" << processed_js << "\n";
        return 1;
    }

    std::cout << "ListenTest PASSED!" << std::endl;
    return 0;
}
