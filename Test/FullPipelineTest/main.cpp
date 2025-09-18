#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include <iostream>

int main() {
    std::string source = R"(
        div {
            id: box;
            style {
                width: 100px;
                height: 200px;
                // Simple conditional using implicit self-reference
                background-color: width > 50px ? "red" : "blue";
                // Chained conditional and logical operators
                border: width > 200px || height < 100px ? "1px solid red" : "2px solid green";
            }
        }
    )";

    std::cout << "--- Input CHTL ---\n" << source << "\n------------------\n" << std::endl;

    try {
        // 1. Lexing
        CHTL::CHTLLexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();

        // 2. Parsing
        CHTL::CHTLParser parser(source, tokens);
        std::unique_ptr<CHTL::BaseNode> ast = parser.parse();

        // 3. Generation
        if (ast) {
            CHTL::CHTLGenerator generator(parser.getTemplateDefinitions());
            CHTL::CompilationResult result = generator.generate(ast.get());

            // 4. Verification
            std::cout << "--- Generated HTML ---\n" << result.html << "\n----------------------\n" << std::endl;
            std::cout << "--- Generated CSS ---\n" << result.css << "\n---------------------\n" << std::endl;
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime_error exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
