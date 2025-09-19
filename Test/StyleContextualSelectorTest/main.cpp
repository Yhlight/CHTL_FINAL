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
    std::string entry_point = "Test/StyleContextualSelectorTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(entry_point);

    std::cout << "--- Input CHTL ---\n" << source << "\n------------------\n" << std::endl;

    std::string expected_css =
        ".box:hover{color:red;}"
        ".box::before{opacity:1;}"
        "#main-span:hover{text-decoration:underline;}"
        ".para-class.active{font-weight:bold;}";

    std::string generated_css;

    try {
        CHTL::CHTLLexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();
        CHTL::CHTLParser parser(source, tokens, entry_point);
        std::unique_ptr<CHTL::BaseNode> ast = parser.parse();
        CHTL::CHTLGenerator generator(parser.getTemplateDefinitions());
        CHTL::CompilationResult result = generator.generate(ast.get());

        generated_css = result.css;

        std::cout << "--- Generated CSS ---\n" << generated_css << "\n---------------------\n" << std::endl;
        std::cout << "--- Expected CSS ---\n" << expected_css << "\n--------------------\n" << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime_error exception: " << e.what() << std::endl;
        return 1;
    }

    // Normalize strings for comparison
    std::string normalized_gen_css = generated_css;
    std::string normalized_exp_css = expected_css;
    removeWhitespace(normalized_gen_css);
    removeWhitespace(normalized_exp_css);

    assert(normalized_gen_css == normalized_exp_css);

    std::cout << "Contextual Selector Test Passed!" << std::endl;

    return 0;
}
