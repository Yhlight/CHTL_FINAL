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

// Helper function to check if a string contains another string
bool contains(const std::string& haystack, const std::string& needle) {
    return haystack.find(needle) != std::string::npos;
}

int main() {
    std::string file_path = "Test/StyleArithmeticTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    // Create a shared configuration object
    auto config = std::make_shared<CHTL::Configuration>();

    // 1. Lexer
    CHTL::CHTLLexer lexer(source, config);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();

    // 2. Parser
    CHTL::CHTLParser parser(source, tokens, file_path, config);
    auto root = parser.parse();

    // 3. Generator
    CHTL::CHTLGenerator generator(parser.getTemplateDefinitions(), config);
    CHTL::CompilationResult result = generator.generate(root.get(), false);

    // 4. Assert
    // Assertions are now order-independent
    std::string processed_html_result = remove_whitespace(result.html);
    assert(contains(processed_html_result, "width:calc(100px+50px)"));
    assert(contains(processed_html_result, "height:calc(200%-(10px*2))"));

    std::string processed_css_result = remove_whitespace(result.css);
    assert(contains(processed_css_result, ".box{"));
    assert(contains(processed_css_result, "font-size:calc(12pt+2pt)"));
    assert(contains(processed_css_result, "color:red"));

    assert(result.js.empty());

    std::cout << "StyleArithmeticTest PASSED!" << std::endl;

    return 0;
}
