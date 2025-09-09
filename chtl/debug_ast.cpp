#include <iostream>
#include <string>
#include "lexer/chtl_lexer.h"
#include "parser/chtl_parser.h"
#include "ast/ast_node.h"

using namespace chtl;

void print_ast(ast::ASTNode::NodePtr node, int depth = 0) {
    std::string indent(depth * 2, ' ');
    std::cout << indent << "Node: " << node->name << " (type=" << static_cast<int>(node->type) << ")" << std::endl;
    
    if (!node->value.empty()) {
        std::cout << indent << "  Value: " << node->value << std::endl;
    }
    
    if (!node->attributes.empty()) {
        std::cout << indent << "  Attributes:" << std::endl;
        for (const auto& attr : node->attributes) {
            std::cout << indent << "    " << attr.first << " = " << attr.second << std::endl;
        }
    }
    
    if (!node->children.empty()) {
        std::cout << indent << "  Children (" << node->children.size() << "):" << std::endl;
        for (const auto& child : node->children) {
            print_ast(child, depth + 1);
        }
    }
}

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
        
        // Syntax analysis
        parser::CHTLParser parser(tokens);
        auto ast = parser.parse();
        
        std::cout << "\n=== AST Structure ===" << std::endl;
        print_ast(ast);
        
        std::cout << "\n=== Generated HTML ===" << std::endl;
        std::cout << ast->to_html() << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}