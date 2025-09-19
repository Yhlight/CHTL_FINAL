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
    std::string entry_point = "Test/StyleSelectorTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(entry_point);

    std::cout << "--- Input CHTL ---\n" << source << "\n------------------\n" << std::endl;

    // Note: class="box existing" is also valid, but the current implementation appends.
    // The test should match the implementation.
    std::string expected_html = "<div class=\"existing box\" id=\"main-box\">Selector Test</div>";
    std::string expected_css = ".box{font-size:16px;}#main-box{font-weight:bold;}";

    std::string generated_html;
    std::string generated_css;

    try {
        CHTL::CHTLLexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();
        CHTL::CHTLParser parser(source, tokens, entry_point);
        std::unique_ptr<CHTL::BaseNode> ast = parser.parse();
        CHTL::CHTLGenerator generator(parser.getTemplateDefinitions());
        CHTL::CompilationResult result = generator.generate(ast.get());

        generated_html = result.html;
        generated_css = result.css;

        std::cout << "--- Generated HTML ---\n" << generated_html << "\n----------------------\n" << std::endl;
        std::cout << "--- Generated CSS ---\n" << generated_css << "\n---------------------\n" << std::endl;
        std::cout << "--- Expected HTML ---\n" << expected_html << "\n---------------------\n" << std::endl;
        std::cout << "--- Expected CSS ---\n" << expected_css << "\n--------------------\n" << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime_error exception: " << e.what() << std::endl;
        return 1;
    }

    // Normalize strings by removing all whitespace for comparison
    std::string normalized_gen_html = generated_html;
    std::string normalized_exp_html = expected_html;
    removeWhitespace(normalized_gen_html);
    removeWhitespace(normalized_exp_html);

    std::string normalized_gen_css = generated_css;
    std::string normalized_exp_css = expected_css;
    removeWhitespace(normalized_gen_css);
    removeWhitespace(normalized_exp_css);

    assert(normalized_gen_html == normalized_exp_html);
    assert(normalized_gen_css == normalized_exp_css);

    std::cout << "Style Selector Test Passed!" << std::endl;

    return 0;
}
