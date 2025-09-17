#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLResolver/TemplateResolver.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include <iostream>

int main() {
    std::string source = R"(
[Custom] @Style TextSet {
    color;
    font-size;
    font-weight: bold;
}

html {
    body {
        p {
            style {
                @Style TextSet {
                    color: blue;
                    font-size: 20px;
                }
            }
            text: "This text should be blue, 20px, and bold.";
        }
    }
}
)";

    try {
        std::cout << "--- Compiling with Custom Templates ---" << std::endl;

        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        std::shared_ptr<RootNode> rawAst = parser.parse();

        TemplateResolver resolver;
        std::shared_ptr<RootNode> resolvedAst = resolver.resolve(rawAst);

        Generator generator;
        std::string html = generator.generate(resolvedAst);

        std::cout << "--- Generated HTML ---" << std::endl;
        std::cout << html << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
