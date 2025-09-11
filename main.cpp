#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "CHTL/CHTLContext.h"
#include "Scanner/UnifiedScanner.h"
#include "Lexer.h"
#include "Parser.h"
#include "Generator.h"
#include "Token.h"

// Function to read a whole file into a string
std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << path << std::endl;
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl>" << std::endl;
        return 1;
    }

    std::string sourcePath = argv[1];
    std::string rawSource = readFile(sourcePath);

    // 1. Unified Scanner
    UnifiedScanner unifiedScanner;
    ScannedSource scanned = unifiedScanner.scan(rawSource);

    // 2. Setup Context
    CHTLContext context;
    context.scriptBlocks = scanned.scriptBlocks;

    // 3. Lexing
    Lexer lexer(scanned.chtlSource);
    std::vector<Token> tokens = lexer.scanTokens();

    // 4. Parsing
    std::vector<std::unique_ptr<BaseNode>> topLevelNodes;
    try {
        Parser parser(tokens, context);
        topLevelNodes = parser.parse();
    } catch (const std::runtime_error& e) {
        std::cerr << "Parsing Error: " << e.what() << std::endl;
        return 1;
    }

    // 5. Generating HTML
    Generator generator;
    std::stringstream finalHtml;
    for (const auto& node : topLevelNodes) {
        if (dynamic_cast<ElementNode*>(node.get())) {
             finalHtml << generator.generate(node.get(), context);
        }
    }

    // 6. Print the final output
    std::cout << finalHtml.str() << std::endl;

    return 0;
}
