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
    std::string file_path = "Test/VarTemplateTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);
    auto config = std::make_shared<CHTL::Configuration>();

    std::vector<CHTL::CodeFragment> fragments = {{source, CHTL::FragmentType::CHTL}};

    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    std::cout << "Generated HTML: " << result.html << std::endl;

    // --- Assertions ---
    assert(contains(result.html, "color: blue;"));
    assert(contains(result.html, "font-size: 18px;"));

    std::cout << "VarTemplateTest PASSED!" << std::endl;

    return 0;
}
