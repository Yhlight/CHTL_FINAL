#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <stdexcept>

void runTest(const std::string& test_file) {
    std::cout << "--- Running test for: " << test_file << " ---\n";
    std::string source;
    try {
        source = CHTL::FileSystem::readFile(test_file);
    } catch (const std::runtime_error& e) {
        std::cerr << "Failed to read test file: " << test_file << ". Error: " << e.what() << std::endl;
        return;
    }

    std::cout << "--- Input CHTL ---\n" << source << "\n------------------\n" << std::endl;

    try {
        CHTL::CHTLLexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();
        CHTL::CHTLParser parser(source, tokens, test_file);
        std::unique_ptr<CHTL::BaseNode> ast = parser.parse();
        CHTL::CHTLGenerator generator(parser.getTemplateDefinitions());
        CHTL::CompilationResult result = generator.generate(ast.get());

        std::cout << "--- Generated HTML ---\n" << result.html << "\n----------------------\n" << std::endl;
        if (!result.css.empty()) {
            std::cout << "--- Generated CSS ---\n" << result.css << "\n---------------------\n" << std::endl;
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime_error exception: " << e.what() << std::endl;
    }
     std::cout << "\n--- Test for: " << test_file << " finished ---\n" << std::endl;
}

int main() {
    runTest("Test/CustomTest/test.chtl");
    return 0;
}
