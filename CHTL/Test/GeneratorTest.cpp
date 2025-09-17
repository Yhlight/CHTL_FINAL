#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include <iostream>

int main() {
    std::string source = R"(
html {
    head {
        title { text: "My CHTL Page"; }
    }
    body {
        div {
            id = main-content;
            class: "container";
            text {
                "Hello, CHTL!"
            }
        }
        span {
            text: "Another element";
        }
    }
}
)";

    try {
        // 1. Lex
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();

        // 2. Parse
        Parser parser(tokens);
        std::shared_ptr<RootNode> ast = parser.parse();

        // 3. Generate
        Generator generator;
        std::string html = generator.generate(ast);

        // 4. Print
        std::cout << "--- Generated HTML ---" << std::endl;
        std::cout << html << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
