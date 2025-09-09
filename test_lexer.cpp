#include "include/CHTL/CHTLLexer.h"
#include <iostream>
#include <fstream>
#include <sstream>

using namespace CHTL;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cout << "Usage: " << argv[0] << " <input.chtl>" << std::endl;
        return 1;
    }
    
    // Read input file
    std::ifstream file(argv[1]);
    if (!file.is_open()) {
        std::cerr << "Error: Cannot open file " << argv[1] << std::endl;
        return 1;
    }
    
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();
    file.close();
    
    // Create context and lexer
    auto context = std::make_shared<CHTLContext>();
    CHTLLexer lexer(context);
    
    // Tokenize
    std::vector<Token> tokens = lexer.tokenize(source);
    
    // Check for errors
    if (lexer.hasErrors()) {
        std::cerr << "Lexer errors:" << std::endl;
        for (const auto& error : lexer.getErrors()) {
            std::cerr << "  " << error << std::endl;
        }
        return 1;
    }
    
    // Print tokens
    std::cout << "Tokens found: " << tokens.size() << std::endl;
    std::cout << "===========================================" << std::endl;
    
    for (const auto& token : tokens) {
        std::cout << "[" << token.line << ":" << token.column << "] "
                  << CHTLLexer::tokenTypeToString(token.type);
        
        if (!token.value.empty()) {
            std::cout << " = \"" << token.value << "\"";
        }
        
        std::cout << std::endl;
    }
    
    std::cout << "===========================================" << std::endl;
    std::cout << "Lexer test completed successfully!" << std::endl;
    
    return 0;
}