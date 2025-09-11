#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "CHTL/CHTLContext.h"
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
    std::string chtlSource = readFile(sourcePath);

    // 1. Setup Context
    CHTLContext context;

    // 2. Lexing
    Lexer lexer(chtlSource);
    std::vector<Token> tokens = lexer.scanTokens();

    // 3. Parsing
    // The parser populates the context with definitions and returns renderable root nodes.
    // We must keep the `topLevelNodes` vector alive, as the context holds raw pointers to nodes owned by it.
    std::vector<std::unique_ptr<BaseNode>> topLevelNodes;
    try {
        Parser parser(tokens, context);
        topLevelNodes = parser.parse();
    } catch (const std::runtime_error& e) {
        std::cerr << "Parsing Error: " << e.what() << std::endl;
        return 1;
    }

    // 4. Generating HTML
    // We iterate through the renderable nodes (non-definitions) and generate HTML for each.
    Generator generator;
    std::stringstream finalHtml;
    for (const auto& node : topLevelNodes) {
        // We only try to render nodes that are not definitions.
        // A better design might be to have the parser return two separate lists.
        if (dynamic_cast<ElementNode*>(node.get())) {
             finalHtml << generator.generate(node.get(), context);
        }
    }

    // 5. Print the final output
    std::cout << finalHtml.str() << std::endl;

    return 0;
}
