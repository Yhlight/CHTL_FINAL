#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include <iostream>
#include <fstream>
#include <sstream>

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file) {
        throw std::runtime_error("Could not open file: " + path);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void writeFile(const std::string& path, const std::string& content) {
    std::ofstream file(path);
    if (!file) {
        throw std::runtime_error("Could not open file for writing: " + path);
    }
    file << content;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: chtl_compiler <input_file.chtl> <output_file.html>" << std::endl;
        return 1;
    }

    std::string inputFile = argv[1];
    std::string outputFile = argv[2];

    try {
        // 1. Read source file
        std::string source = readFile(inputFile);

        // 2. Lex
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();

        // 3. Parse
        Parser parser(tokens);
        std::shared_ptr<RootNode> ast = parser.parse();

        // 4. Generate
        Generator generator;
        std::string html = generator.generate(ast);

        // 5. Write to output file
        writeFile(outputFile, html);

        std::cout << "Successfully compiled " << inputFile << " to " << outputFile << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
