#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
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
    std::string file_path = "Test/ConfigTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    // Create a shared configuration object
    auto config = std::make_shared<CHTL::Configuration>();

    // 1. Lexer
    // The lexer is created after the parser has populated the config
    // This is a slight deviation from the previous tests, but necessary
    // for config to be applied before lexing the main content.
    // A more robust implementation would involve a multi-pass approach.
    // For this test, we can pre-parse the config first.

    // Simplified flow for testing:
    // Pass 1: Parse config. We expect this to fail when it hits content
    // that relies on the new config, but that's okay. The config object
    // will be populated before the error is thrown.
    try {
        CHTL::CHTLLexer pre_lexer(source, config);
        std::vector<CHTL::Token> pre_tokens = pre_lexer.scanTokens();
        CHTL::CHTLParser pre_parser(source, pre_tokens, file_path, config);
        pre_parser.parse();
    } catch (const std::runtime_error& e) {
        // Ignore expected error.
    }

    // Pass 2: Re-lex and parse with the populated config
    CHTL::CHTLLexer lexer(source, config);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();
    CHTL::CHTLParser parser(source, tokens, file_path, config);
    auto root = parser.parse();
    bool use_doctype = parser.getUseHtml5Doctype();

    // 3. Generator
    CHTL::CHTLGenerator generator(parser.getTemplateDefinitions(), config);
    CHTL::CompilationResult result = generator.generate(root.get(), use_doctype);

    // 4. Assert
    std::string expected_html = R"(
        <div style="border: 1px solid black;"></div>
    )";

    std::string expected_css = R"(
        .configured-box {
          color: green;
        }
    )";

    std::string processed_html_result = remove_whitespace(result.html);
    std::string processed_html_expected = remove_whitespace(expected_html);
    assert(processed_html_result == processed_html_expected);

    std::string processed_css_result = remove_whitespace(result.css);
    std::string processed_css_expected = remove_whitespace(expected_css);
    assert(processed_css_result == processed_css_expected);

    assert(result.js.empty());

    std::cout << "ConfigTest PASSED!" << std::endl;

    return 0;
}
