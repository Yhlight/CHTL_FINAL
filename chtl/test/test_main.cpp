#include <iostream>
#include <cassert>
#include "lexer/lexer.h"
#include "parser/parser.h"
#include "ast/ast_node.h"

using namespace chtl;

void test_lexer() {
    std::cout << "Testing Lexer..." << std::endl;
    
    std::string source = R"(
        html
        {
            head
            {
                title
                {
                    text
                    {
                        Hello World
                    }
                }
            }
        }
    )";
    
    lexer::Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    std::cout << "Generated " << tokens.size() << " tokens" << std::endl;
    
    // Verify we have some expected tokens
    bool found_html = false;
    bool found_brace = false;
    
    for (const auto& token : tokens.get_tokens()) {
        if (token.value == "html") {
            found_html = true;
        }
        if (token.type == lexer::TokenType::LEFT_BRACE) {
            found_brace = true;
        }
    }
    
    assert(found_html);
    assert(found_brace);
    
    std::cout << "Lexer test passed!" << std::endl;
}

void test_parser() {
    std::cout << "Testing Parser..." << std::endl;
    
    std::string source = R"(
        html
        {
            head
            {
                title
                {
                    text
                    {
                        Hello World
                    }
                }
            }
        }
    )";
    
    lexer::Lexer lexer(source);
    auto tokens = lexer.tokenize();
    
    parser::Parser parser(tokens);
    auto ast = parser.parse();
    
    assert(ast != nullptr);
    assert(ast->type == ast::NodeType::ROOT);
    assert(ast->has_children());
    
    std::cout << "Parser test passed!" << std::endl;
}

void test_ast() {
    std::cout << "Testing AST..." << std::endl;
    
    auto element = std::make_shared<ast::ElementNode>("div");
    element->set_attribute("id", "test");
    element->set_attribute("class", "container");
    
    auto text = std::make_shared<ast::TextNode>("Hello World");
    element->add_child(text);
    
    assert(element->name == "div");
    assert(element->has_attribute("id"));
    assert(element->get_attribute("id") == "test");
    assert(element->has_children());
    assert(element->child_count() == 1);
    
    std::cout << "AST test passed!" << std::endl;
}

int main() {
    try {
        test_lexer();
        test_parser();
        test_ast();
        
        std::cout << "\nAll tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}