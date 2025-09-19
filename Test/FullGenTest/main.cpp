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
    std::string file_path = "Test/FullGenTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    // 1. Lexer
    CHTL::CHTLLexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();

    // 2. Parser
    CHTL::CHTLParser parser(source, tokens, file_path);
    auto root = parser.parse();
    bool use_doctype = parser.getUseHtml5Doctype();

    // 3. Generator
    CHTL::CHTLGenerator generator(parser.getTemplateDefinitions());
    CHTL::CompilationResult result = generator.generate(root.get(), use_doctype);

    // 4. Assert
    std::string expected_html = R"(
        <!DOCTYPE html>
        <div class="box" style="height: 200px;">Hello World</div>
    )";

    std::string expected_css = R"(
        .box {
          color: red;
        }
    )";

    std::string processed_html_result = remove_whitespace(result.html);
    std::string processed_html_expected = remove_whitespace(expected_html);
    assert(processed_html_result == processed_html_expected);

    std::string processed_css_result = remove_whitespace(result.css);
    std::string processed_css_expected = remove_whitespace(expected_css);
    assert(processed_css_result == processed_css_expected);

    assert(result.js.empty());

    std::cout << "FullGenTest PASSED!" << std::endl;

    return 0;
}
