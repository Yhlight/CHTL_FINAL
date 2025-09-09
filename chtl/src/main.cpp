#include <iostream>
#include <fstream>
#include <string>
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "ast/ast_node.h"

using namespace chtl;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file>" << std::endl;
        return 1;
    }
    
    std::string filename = argv[1];
    std::ifstream file(filename);
    
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return 1;
    }
    
    // Read file content
    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());
    file.close();
    
    try {
        // Lexical analysis
        lexer::Lexer lexer(content);
        auto tokens = lexer.tokenize();
        
        std::cout << "Tokens:" << std::endl;
        for (const auto& token : tokens.get_tokens()) {
            std::cout << token.to_string() << std::endl;
        }
        
        // Syntax analysis
        parser::Parser parser(tokens);
        auto ast = parser.parse();
        
        std::cout << "\nAST:" << std::endl;
        std::cout << ast->to_string() << std::endl;
        
        // Generate HTML
        std::cout << "\nGenerated HTML:" << std::endl;
        std::cout << ast->to_html() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}