#include <iostream>
#include <vector>
#include <string>
#include <memory>

#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLContext/CHTLContext.h"
#include "CHTL/CHTLNode/DocumentNode.h"

int main() {
    std::string source = R"(
        [Template] @Element Inner
        {
            span { text { "Inner" } }
        }

        [Template] @Element Outer
        {
            div {
                @Element Inner;
            }
        }

        body {
            @Element Outer;
        }
    )";

    std::cout << "--- CHTL Template Inheritance Test ---" << std::endl;
    std::cout << "\n--- Source Code ---\n" << source << std::endl;

    try {
        CHTL::Lexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.tokenize();

        CHTL::CHTLContext context;
        CHTL::Parser parser(tokens, context);
        std::unique_ptr<CHTL::DocumentNode> ast = parser.parse();
        std::cout << "\n--- Parsing Successful ---" << std::endl;

        CHTL::Generator generator;
        std::string html = generator.generate(ast.get(), context);
        std::cout << "--- Code Generation Successful ---" << std::endl;

        std::cout << "\n--- Generated HTML ---\n" << html << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "\n--- COMPILATION FAILED ---\n" << e.what() << std::endl;
        return 1;
    }

    std::cout << "\n--------------------------------------" << std::endl;
    return 0;
}
