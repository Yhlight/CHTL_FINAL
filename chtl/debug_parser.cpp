#include <iostream>
#include <string>
#include "lexer/chtl_lexer.h"
#include "parser/chtl_parser.h"
#include "ast/ast_node.h"

using namespace chtl;

int main() {
    std::string source = "html { head { title { text { Hello World } } } }";
    
    try {
        // Lexical analysis
        lexer::CHTLLexer lexer(source);
        auto tokens = lexer.tokenize();
        
        std::cout << "=== Tokens ===" << std::endl;
        for (const auto& token : tokens.get_tokens()) {
            std::cout << token.to_string() << std::endl;
        }
        
        std::cout << "\n=== Starting Parser ===" << std::endl;
        
        // Test token stream
        std::cout << "Current token: " << tokens.current().to_string() << std::endl;
        std::cout << "Has next: " << tokens.has_next() << std::endl;
        
        // Move to next token
        tokens.next();
        std::cout << "After next, current token: " << tokens.current().to_string() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}