#include <iostream>
#include <string>
#include "lexer/chtl_lexer.h"
#include "parser/chtl_parser.h"
#include "ast/ast_node.h"

using namespace chtl;

int main() {
    std::string source = "div { id: \"main\"; class: \"container\"; text { Hello World } }";
    
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
            std::cout << "Div node attributes: " << div_node->attributes.size() << std::endl;
            
            for (const auto& attr : div_node->attributes) {
                std::cout << "  " << attr.first << " = " << attr.second << std::endl;
            }
            
            std::cout << "Div node children: " << div_node->children.size() << std::endl;
            
            for (size_t i = 0; i < div_node->children.size(); ++i) {
                auto child = div_node->children[i];
                std::cout << "Child " << i << " type: " << static_cast<int>(child->type) << std::endl;
                std::cout << "Child " << i << " name: " << child->name << std::endl;
                std::cout << "Child " << i << " value: " << child->value << std::endl;
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