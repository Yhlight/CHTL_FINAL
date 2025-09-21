#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../CHTL/CompilerDispatcher.h"
#include "../../CHTL/Config/Configuration.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>
#include <memory>

// Helper function to check if a string contains a substring
bool contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

int main() {
    std::string file_path = "Test/ImportTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    auto config = std::make_shared<CHTL::Configuration>();
    // The parser needs to know the keywords used in the test files
    config->keyword_map["KEYWORD_TEMPLATE"] = {"[Template]"}; // Not quite right, but let's see
    config->keyword_map["TEMPLATE_ELEMENT"] = {"@Element"};
    config->keyword_map["KEYWORD_FROM"] = {"from"};
    config->keyword_map["KEYWORD_IMPORT"] = {"[Import]"};
    config->keyword_map["IMPORT_CHTL"] = {"@Chtl"};


    // 1. Scanner
    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    // 2. Dispatcher
    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    // 3. Assert
    bool pass = true;

    if (!contains(result.html, "I am from lib.chtl")) {
        std::cerr << "ImportTest FAILED: Did not find content from lib.chtl." << std::endl;
        pass = false;
    }
    if (!contains(result.html, "I am from the module directory!")) {
        std::cerr << "ImportTest FAILED: Did not find content from module_lib.chtl." << std::endl;
        pass = false;
    }

    if (!pass) {
        std::cerr << "Full generated HTML: \n" << result.html << std::endl;
        return 1;
    }

    std::cout << "ImportTest PASSED!" << std::endl;

    return 0;
}
