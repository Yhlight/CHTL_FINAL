#include <iostream>
#include <string>
#include "lexer/chtl_lexer.h"
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
        
        std::cout << "\n=== Simple Parsing ===" << std::endl;
        
        // Simple parsing
        auto root = std::make_shared<ast::ASTNode>(ast::NodeType::ROOT, "root");
        
        while (tokens.current().type != lexer::TokenType::EOF_TOKEN) {
            auto token = tokens.current();
            std::cout << "Processing token: " << token.to_string() << std::endl;
            
            if (token.type == lexer::TokenType::IDENTIFIER) {
                auto element = std::make_shared<ast::ElementNode>(token.value);
                root->add_child(element);
                std::cout << "Created element: " << token.value << std::endl;
            }
            
            tokens.next();
        }
        
        std::cout << "\n=== AST ===" << std::endl;
        std::cout << "Root has " << root->child_count() << " children" << std::endl;
        for (const auto& child : root->children) {
            std::cout << "Child: " << child->to_string() << std::endl;
        }
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}