#include <iostream>
#include <vector>
#include "CHTLLexer.h"
#include "CHTLParser.h"
#include "CHTLGenerator.h"

void test_full_pipeline() {
    std::cout << "--- Running Full Pipeline Test ---" << std::endl;
    std::string source = R"~(
html {
    head {
        title {
            text { "My CHTL Page" }
        }
    }
    body {
        div {
            text { "Hello, World!" }
        }
    }
}
    )~";

    // 1. Lexer
    CHTL::Lexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();

    // 2. Parser
    CHTL::Parser parser(tokens);
    CHTL::NodeList ast;
    try {
        ast = parser.parse();
    } catch (const CHTL::ParseError& e) {
        std::cerr << "Parse Error: " << e.what() << std::endl;
        return;
    }

    // 3. Generator
    CHTL::Generator generator;
    std::string html_output = generator.generate(ast);

    // 4. Print final HTML
    std::cout << "\n--- Generated HTML ---\n" << std::endl;
    std::cout << html_output << std::endl;
}

int main() {
    test_full_pipeline();
    return 0;
}
