#include <iostream>
#include <string>
#include "lexer/chtl_lexer.h"
#include "parser/chtl_parser.h"
#include "ast/ast_node.h"

using namespace chtl;

int main() {
    std::string source = "div { text { Hello World } }";
    
    try {
        // Lexical analysis
        lexer::CHTLLexer lexer(source);
        auto tokens = lexer.tokenize();
        
        std::cout << "=== Tokens ===" << std::endl;
        for (const auto& token : tokens.get_tokens()) {
            std::cout << token.to_string() << std::endl;
        }
        
        std::cout << "\n=== Starting Parser ===" << std::endl;
        
        // Syntax analysis
        parser::CHTLParser parser(tokens);
        auto ast = parser.parse();
        
        std::cout << "\n=== AST Structure ===" << std::endl;
        std::cout << "Root children: " << ast->children.size() << std::endl;
        
        if (!ast->children.empty()) {
            auto div_node = ast->children[0];
            std::cout << "Div node type: " << static_cast<int>(div_node->type) << std::endl;
            std::cout << "Div node name: " << div_node->name << std::endl;
            std::cout << "Div node children: " << div_node->children.size() << std::endl;
            
            if (!div_node->children.empty()) {
                auto text_node = div_node->children[0];
                std::cout << "Text node type: " << static_cast<int>(text_node->type) << std::endl;
                std::cout << "Text node name: " << text_node->name << std::endl;
                std::cout << "Text node value: " << text_node->value << std::endl;
            }
        }
        
        std::cout << "\n=== Generated HTML ===" << std::endl;
        std::cout << ast->to_html() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}