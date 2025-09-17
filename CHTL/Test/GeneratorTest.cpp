#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include <iostream>

int main() {
    std::string source = R"(
html {
    head {
        title { text: "Global Styles Test"; }
    }
    body {
        div {
            // This div should get class="box" added automatically
            style {
                // This is an inline style
                background-color: #eee;

                // This is a global rule
                .box {
                    border: 1px solid black;
                    padding: 10px + 5px;
                }

                // This should become .box:hover
                &:hover {
                    background-color: #ccc;
                }
            }
            text: "This box should have a border and padding.";
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
        std::cout << "--- Generated HTML with Global Styles ---" << std::endl;
        std::cout << html << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
