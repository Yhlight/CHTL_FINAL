#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLResolver/TemplateResolver.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include <iostream>

int main() {
    std::string source = R"(
html {
    body {
        div {
            style {
                width: 100px + 10%;
                height: 50% - 25px;
            }
        }
    }
}
)";

    try {
        std::cout << "--- Compiling with Mixed-Unit Arithmetic ---" << std::endl;

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
