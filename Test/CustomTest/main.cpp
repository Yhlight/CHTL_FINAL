#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../../CHTL/CHTLContext/Configuration.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <stdexcept>
#include <memory>

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
        // --- Step 1: Create default configuration ---
        auto config = std::make_shared<CHTL::Configuration>();

        // --- Step 2: Lex the file ---
        CHTL::CHTLLexer lexer(source, config); // Lexer now needs config for keywords
        std::vector<CHTL::Token> tokens = lexer.scanTokens();

        // --- Step 3: Pre-parse for [Configuration] block ---
        CHTL::CHTLParser pre_parser(source, tokens, test_file, config);
        std::unique_ptr<CHTL::ConfigNode> config_node = pre_parser.preParseConfiguration();
        if (config_node) {
            std::cout << "--- Found [Configuration] block, applying settings... ---\n";
            config->update_from_node(config_node.get());
            // Re-lex with the new configuration
            lexer.reset();
            tokens = lexer.scanTokens();
        }

        // --- Step 4: Main parse with final configuration ---
        CHTL::CHTLParser parser(source, tokens, test_file, config);
        std::unique_ptr<CHTL::BaseNode> ast = parser.parse();

        // --- Step 5: Generate output ---
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
    std::cout << "===== STARTING test.chtl =====" << std::endl;
    runTest("Test/CustomTest/test.chtl");
    std::cout << "===== FINISHED test.chtl =====" << std::endl;

    std::cout << "===== STARTING config_test.chtl =====" << std::endl;
    runTest("Test/CustomTest/config_test.chtl");
    std::cout << "===== FINISHED config_test.chtl =====" << std::endl;
    return 0;
}
