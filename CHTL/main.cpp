#include <iostream>
#include <fstream>
#include <string>
#include "CHTLCompiler/CHTLLexer/CHTLLexer.h"
#include "CHTLCompiler/CHTLParser/CHTLParser.h"
#include "CHTLCompiler/CHTLGenerator/CHTLGenerator.h"
#include "CHTLCompiler/CHTLContext/CHTLContext.h"

using namespace CHTL;

void printHelp() {
    std::cout << "CHTL Compiler - A hypertext language based on C++\n";
    std::cout << "\n";
    std::cout << "USAGE:\n";
    std::cout << "    chtl <input_file> [output_file]\n";
    std::cout << "\n";
    std::cout << "ARGS:\n";
    std::cout << "    <input_file>    Input CHTL file to compile\n";
    std::cout << "    [output_file]   Output HTML file (default: output.html)\n";
    std::cout << "\n";
    std::cout << "OPTIONS:\n";
    std::cout << "    --help          Show this help message\n";
    std::cout << "\n";
    std::cout << "EXAMPLES:\n";
    std::cout << "    chtl input.chtl\n";
    std::cout << "    chtl input.chtl output.html\n";
    std::cout << "\n";
    std::cout << "For more information, visit: https://github.com/chtl-project/chtl\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file> [output_file]\n";
        std::cerr << "       " << argv[0] << " --help\n";
        return 1;
    }
    
    if (std::string(argv[1]) == "--help") {
        printHelp();
        return 0;
    }
    
    std::string input_file = argv[1];
    std::string output_file = (argc > 2) ? argv[2] : "output.html";
    
    try {
        // Read input file
        std::ifstream file(input_file);
        if (!file.is_open()) {
            std::cerr << "Error: Could not open input file: " << input_file << std::endl;
            return 1;
        }
        
        std::string source((std::istreambuf_iterator<char>(file)),
                          std::istreambuf_iterator<char>());
        file.close();
        
        // Create lexer and tokenize
        CHTLLexer lexer;
        std::vector<Token> tokens = lexer.tokenize(source);
        
        // Print tokens for debugging
        std::cout << "Tokens found: " << tokens.size() << std::endl;
        for (const auto& token : tokens) {
            std::cout << "  " << static_cast<int>(token.token_type) << ": '" << token.value 
                      << "' at line " << token.line << ", column " << token.column << std::endl;
        }
        
        // Create parser and parse
        CHTLParser parser(tokens);
        auto ast = parser.parse();
        
        if (!ast) {
            std::cerr << "Error: Failed to parse CHTL file" << std::endl;
            return 1;
        }
        
        // Create generator and generate HTML
        CHTLGenerator generator;
        std::string html = generator.generateHTML(ast);
        
        // Write output file
        std::ofstream outfile(output_file);
        if (!outfile.is_open()) {
            std::cerr << "Error: Could not create output file: " << output_file << std::endl;
            return 1;
        }
        
        outfile << html;
        outfile.close();
        
        std::cout << "Successfully compiled " << input_file << " to " << output_file << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Compilation error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}