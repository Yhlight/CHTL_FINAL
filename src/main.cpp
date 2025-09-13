#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include "Scanner/UnifiedScanner.h"
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL/CHTLLoader/CHTLLoader.h"

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl>" << std::endl;
        return 1;
    }

    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << argv[1] << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    // 1. Use the Unified Scanner to separate code fragments
    CHTL::UnifiedScanner scanner(source);
    CHTL::CodeFragments fragments = scanner.scan();

    // --- For debugging purposes, print the fragments ---
    std::cout << "--- CHTL Fragment ---" << std::endl;
    std::cout << fragments.chtl << std::endl;
    std::cout << "--- CSS Fragment ---" << std::endl;
    std::cout << fragments.css << std::endl;
    std::cout << "--- JS Fragment (raw) ---" << std::endl;
    std::cout << fragments.js << std::endl;
    std::cout << "--- CHTL JS Fragment (with placeholders) ---" << std::endl;
    std::cout << fragments.chtl_js << std::endl;
    std::cout << "--- JS Placeholders Map ---" << std::endl;
    for (const auto& pair : fragments.js_placeholders) {
        std::cout << pair.first << " : " << pair.second << std::endl;
    }
    std::cout << "--- End of Fragments ---" << std::endl << std::endl;
    // --- End of debugging prints ---

    // 2. Setup compilation context
    CHTL::CHTLLoader loader;
    auto context = std::make_shared<CHTL::ParserContext>();

    // 3. Lex CHTL fragment
    CHTL::CHTLLexer lexer(fragments.chtl);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();

    // 4. Parse CHTL fragment
    // Note: The parser might need the full source for error reporting, which is a refactor for later.
    // For now, we pass it the CHTL fragment.
    CHTL::CHTLParser parser(fragments.chtl, tokens, loader, argv[1], context);
    std::unique_ptr<CHTL::RootNode> ast = parser.parse();

    // 5. Generate HTML from the CHTL AST
    CHTL::CHTLGenerator generator;
    std::string html_output = generator.generate(*ast);

    // 6. Combine and print final output (simplified for now)
    std::cout << "<style>" << std::endl;
    std::cout << fragments.css << std::endl;
    std::cout << "</style>" << std::endl;
    std::cout << html_output << std::endl;
    // For now, the final output will be incomplete due to parser issues.
    // The goal is to verify the scanner's fragments.
    // std::cout << "<script>" << std::endl;
    // std::cout << fragments.js << std::endl; // This would need re-assembly
    // std::cout << "</script>" << std::endl;

    return 0;
}
