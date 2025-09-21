#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../CHTL/CompilerDispatcher.h"
#include "../../CHTL/Config/Configuration.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>
#include <memory>

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

// Helper to check if a string contains a substring
bool contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

int main() {
    std::string file_path = "Test/ListenTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    auto config = std::make_shared<CHTL::Configuration>();
    // CHTL JS keywords are not yet configurable, so no need to set them up
    // for this test.

    // 1. Scanner
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    // 2. Dispatcher
    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    // 3. Assert
    bool pass = true;
    std::string processed_js = remove_whitespace(result.js);

    std::string expected_js_part_1 = "document.querySelector('#my-button').addEventListener('click',()=>";
    std::string expected_js_part_2 = "document.querySelector('#my-button').textContent=\"Clicked!\";";
    std::string expected_js_part_3 = "document.querySelector('#my-button').addEventListener('mouseenter',function(){console.log(\"Mouseentered!\");});";


    if (!contains(processed_js, expected_js_part_1)) {
        std::cerr << "ListenTest FAILED: JS does not contain the click event listener setup." << std::endl;
        pass = false;
    }
    if (!contains(processed_js, expected_js_part_2)) {
        std::cerr << "ListenTest FAILED: JS does not contain the click event handler body." << std::endl;
        pass = false;
    }
    if (!contains(processed_js, expected_js_part_3)) {
        std::cerr << "ListenTest FAILED: JS does not contain the mouseenter event listener setup." << std::endl;
        pass = false;
    }

    if (!pass) {
        std::cerr << "Full generated JS: \n" << result.js << std::endl;
        return 1;
    }

    std::cout << "ListenTest PASSED!" << std::endl;

    return 0;
}
