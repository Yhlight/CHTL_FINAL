#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "CHTL/CHTLContext.h"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"

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

    CHTLContext context;
    Lexer lexer(rawSource);
    std::vector<Token> tokens = lexer.scanTokens();

    Parser parser(tokens, context);
    std::vector<std::unique_ptr<BaseNode>> topLevelNodes = parser.parse();

    Generator generator;
    std::stringstream finalHtml;
    for (const auto& node : topLevelNodes) {
        if (dynamic_cast<ElementNode*>(node.get()) || dynamic_cast<OriginNode*>(node.get())) {
             finalHtml << generator.generate(node.get(), context);
        }
    }

    std::cout << finalHtml.str() << std::endl;

    return 0;
}
