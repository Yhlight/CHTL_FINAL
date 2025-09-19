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
    std::string file_path = "Test/TemplateStyleTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);
    auto config = std::make_shared<CHTL::Configuration>();

    // Since the scanner is not fully implemented, we treat the whole file as one CHTL fragment.
    std::vector<CHTL::CodeFragment> fragments = {{source, CHTL::FragmentType::CHTL}};

    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    // The generator's output order for styles isn't guaranteed,
    // so we'll check for the presence of each expected style property.
    std::string expected_div_start = "<div";
    std::string expected_text = ">Hello Template!</div>";

    // Check that the generated HTML contains the div and the text
    assert(contains(result.html, expected_div_start));
    assert(contains(result.html, expected_text));

    // Check that the necessary style properties are present.
    std::cout << "Generated HTML: " << result.html << std::endl;
    assert(contains(result.html, "color: red;"));
    assert(contains(result.html, "font-size: 16px;"));
    assert(contains(result.html, "background-color: blue;"));

    std::cout << "TemplateStyleTest PASSED!" << std::endl;

    return 0;
}
