#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>
#include <sstream>

// Helper to remove all whitespace from a string
void removeWhitespace(std::string& str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
}

int main() {
    std::string entry_point = "Test/StyleBlockTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(entry_point);

    std::cout << "--- Input CHTL ---\n" << source << "\n------------------\n" << std::endl;

    std::string expected_html = "<div style=\"background-color:#f0f0f0;color:red;width:100px;\">Hello Style!</div>";
    std::string generated_html;

    try {
        CHTL::CHTLLexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();
        CHTL::CHTLParser parser(source, tokens, entry_point);
        std::unique_ptr<CHTL::BaseNode> ast = parser.parse();
        CHTL::CHTLGenerator generator(parser.getTemplateDefinitions());
        CHTL::CompilationResult result = generator.generate(ast.get());

        generated_html = result.html;

        std::cout << "--- Generated HTML ---\n" << generated_html << "\n----------------------\n" << std::endl;
        std::cout << "--- Expected HTML ---\n" << expected_html << "\n---------------------\n" << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime_error exception: " << e.what() << std::endl;
        return 1;
    }

    // Normalize strings by removing all whitespace for comparison
    std::string normalized_generated = generated_html;
    std::string normalized_expected = expected_html;
    removeWhitespace(normalized_generated);
    removeWhitespace(normalized_expected);

    assert(normalized_generated == normalized_expected);

    std::cout << "Style Block Test Passed!" << std::endl;

    return 0;
}
