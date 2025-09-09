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
        
        // Syntax analysis
        parser::CHTLParser parser(tokens);
        auto ast = parser.parse();
        
        std::cout << "=== AST Structure ===" << std::endl;
        std::cout << "Root type: " << static_cast<int>(ast->type) << std::endl;
        std::cout << "Root children: " << ast->children.size() << std::endl;
        
        if (!ast->children.empty()) {
            auto html_node = ast->children[0];
            std::cout << "HTML node type: " << static_cast<int>(html_node->type) << std::endl;
            std::cout << "HTML node name: " << html_node->name << std::endl;
            std::cout << "HTML node children: " << html_node->children.size() << std::endl;
            
            std::cout << "\n=== HTML Generation Test ===" << std::endl;
            std::cout << "HTML node to_html(): " << html_node->to_html() << std::endl;
            
            if (!html_node->children.empty()) {
                auto head_node = html_node->children[0];
                std::cout << "Head node type: " << static_cast<int>(head_node->type) << std::endl;
                std::cout << "Head node name: " << head_node->name << std::endl;
                std::cout << "Head node to_html(): " << head_node->to_html() << std::endl;
            }
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}