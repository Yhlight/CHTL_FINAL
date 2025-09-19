#include "../../Scanner/CHTLUnifiedScanner.h"
#include "../../CHTL/CompilerDispatcher.h"
#include "../../CHTL/CodeMerger.h"
#include "../../CHTL/Config/Configuration.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>
#include <memory>
#include <vector>

// Helper function to check if a string contains a substring
bool contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

int main() {
    std::string file_path = "Test/CustomStyleTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);
    auto config = std::make_shared<CHTL::Configuration>();

    std::vector<CHTL::CodeFragment> fragments = {{source, CHTL::FragmentType::CHTL}};

    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    std::cout << "Generated HTML: " << result.html << std::endl;

    // --- Assertions ---
    // Check for properties that should be there
    assert(contains(result.html, "font-size: 16px;"));
    assert(contains(result.html, "padding: 10px;"));
    assert(contains(result.html, "background-color: red;"));

    // Check for properties that should NOT be there
    assert(!contains(result.html, "border:"));
    assert(!contains(result.html, "background-color: grey;"));

    std::cout << "CustomStyleTest PASSED!" << std::endl;

    return 0;
}
