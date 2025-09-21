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

// Helper to check if a string contains a substring
bool contains(const std::string& haystack, const std::string& needle) {
    return haystack.find(needle) != std::string::npos;
}

int main() {
    std::string file_path = "Test/ImportTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    auto config = std::make_shared<CHTL::Configuration>();
    config->keyword_map["KEYWORD_TEMPLATE"] = {"Template"};
    config->keyword_map["KEYWORD_ORIGIN"] = {"Origin"};
    config->keyword_map["KEYWORD_IMPORT"] = {"Import"};
    config->keyword_map["KEYWORD_FROM"] = {"from"};
    config->keyword_map["KEYWORD_AS"] = {"as"};

    CHTL::CHTLUnifiedScanner scanner(source);
    auto fragments = scanner.scan();

    CHTL::CompilerDispatcher dispatcher(config, file_path);
    CHTL::FinalCompilationResult result = dispatcher.dispatch(fragments);

    std::string expected_html_part = R"(<div class="box imported-box">This box was imported from lib.chtl</div>)";
    std::string expected_css_part = R"(.imported-style {
    font-family: "Comic Sans MS", cursive, sans-serif;
    color: papayawhip;
    padding: 20px;
})";

    bool pass = true;

    if (!contains(result.html, expected_html_part)) {
        std::cerr << "TEST FAILED: HTML output does not contain imported template content.\n";
        pass = false;
    }

    // The merger adds newlines, so we check the raw CSS output from the generator
    if (remove_whitespace(result.css) != remove_whitespace(expected_css_part)) {
        std::cerr << "TEST FAILED: CSS output does not contain imported CSS content.\n";
        pass = false;
    }

    if (!pass) {
        std::cerr << "\n--- Generated HTML ---\n" << result.html << "\n";
        std::cerr << "\n--- Generated CSS ---\n" << result.css << "\n";
        return 1;
    }

    std::cout << "ImportTest PASSED!" << std::endl;
    return 0;
}
