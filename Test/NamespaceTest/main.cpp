#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>

// Helper function to check if a string contains a substring
bool contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

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
    std::string file_path = "Test/NamespaceTest/main.chtl";
    std::string source_code = CHTL::FileSystem::readFile(file_path);

    // Create a shared configuration object
    auto config = std::make_shared<CHTL::Configuration>();

    // 1. Lexer
    CHTL::CHTLLexer lexer(source_code, config);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();

    // 2. Parser
    CHTL::CHTLParser parser(source_code, tokens, file_path, config);
    auto root = parser.parse();
    bool use_doctype = parser.getUseHtml5Doctype();

    // 3. Generator
    CHTL::CHTLGenerator generator(parser.getTemplateDefinitions(), config);
    CHTL::CompilationResult result = generator.generate(root.get(), use_doctype);

    // Using a whitespace-insensitive comparison is more robust
    std::string processed_html = remove_whitespace(result.html);

    std::cout << "--- Generated HTML ---\n" << result.html << "\n----------------------\n";

    // 4. Assert
    // Properties are sorted alphabetically by the generator's std::map

    // Check the div that uses the 'Theme' namespace
    assert(contains(processed_html, "id=\"themed\"style=\"font-family:Arial;font-size:16px;\""));

    // Check the div that uses the 'Theme.Dark' nested namespace
    assert(contains(processed_html, "id=\"dark-themed\"style=\"background-color:black;color:white;\""));

    // Check the div that uses the imported 'Library' namespace
    assert(contains(processed_html, "id=\"lib-themed\"style=\"border:1pxsolidblue;padding:10px;\""));

    // CSS and JS should be empty for this test
    assert(result.css.empty());
    assert(result.js.empty());

    std::cout << "NamespaceTest PASSED!" << std::endl;

    return 0;
}
