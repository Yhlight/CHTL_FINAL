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
    std::string file_path = "Test/CommentTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    // Create a shared configuration object
    auto config = std::make_shared<CHTL::Configuration>();

    // 1. Lexer
    CHTL::CHTLLexer lexer(source, config);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();

    // 2. Parser
    CHTL::CHTLParser parser(source, tokens, file_path, config);
    auto root = parser.parse();
    bool use_doctype = parser.getUseHtml5Doctype();

    // 3. Generator
    CHTL::CHTLGenerator generator(parser.getTemplateDefinitions(), config);
    CHTL::CompilationResult result = generator.generate(root.get(), use_doctype);

    // 4. Assert
    std::string expected_html = R"(
        <!-- This is a top-level comment. -->
        <div>
            <!-- This is a comment inside a div. -->
            Hello, world!
        </div>
        <!-- This is a final comment. -->
    )";

    std::string processed_html_result = remove_whitespace(result.html);
    std::string processed_html_expected = remove_whitespace(expected_html);

    if (processed_html_result != processed_html_expected) {
        std::cerr << "CommentTest FAILED!" << std::endl;
        std::cerr << "Expected:\n" << expected_html << std::endl;
        std::cerr << "Got:\n" << result.html << std::endl;
        return 1;
    }

    std::cout << "CommentTest PASSED!" << std::endl;

    return 0;
}
