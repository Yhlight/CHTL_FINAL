#include <iostream>
#include <fstream>
#include <string>
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "ast/ast_node.h"

using namespace chtl;

int main() {
    std::string source = R"(
        html
        {
            head
            {
                title
                {
                    text
                    {
                        Hello CHTL
                    }
                }
            }
            
            body
            {
                div
                {
                    id: "main";
                    class: "container";
                    
                    text
                    {
                        Welcome to CHTL!
                    }
                    
                    style
                    {
                        .container
                        {
                            width: 100%;
                            height: 100vh;
                            background-color: #f0f0f0;
                        }
                    }
                }
            }
        }
    )";
    
    try {
        // Lexical analysis
        lexer::Lexer lexer(source);
        auto tokens = lexer.tokenize();
        
        std::cout << "=== Tokens ===" << std::endl;
        for (const auto& token : tokens.get_tokens()) {
            if (token.type != lexer::TokenType::WHITESPACE && 
                token.type != lexer::TokenType::NEWLINE) {
                std::cout << token.to_string() << std::endl;
            }
        }
        
        // Syntax analysis
        parser::Parser parser(tokens);
        auto ast = parser.parse();
        
        std::cout << "\n=== AST ===" << std::endl;
        std::cout << ast->to_string() << std::endl;
        
        // Generate HTML
        std::cout << "\n=== Generated HTML ===" << std::endl;
        std::cout << ast->to_html() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}