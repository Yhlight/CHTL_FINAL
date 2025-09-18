#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include <iostream>

int main() {
    std::string source = R"(
        [Template] @Var MyTheme {
            primary-color: #3498db;
            base-padding: 10px;
            large-padding: MyTheme(base-padding) * 2;
        }

        div {
            style {
                color: MyTheme(primary-color);
                padding: MyTheme(large-padding);
                margin: MyTheme(base-padding) / 2;
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
        CHTL::CHTLGenerator generator(parser.getTemplateDefinitions());
        CHTL::CompilationResult result = generator.generate(ast.get());

        // 4. Verification
        std::cout << "--- Generated HTML ---\n" << result.html << "\n----------------------\n" << std::endl;
        std::cout << "--- Generated CSS ---\n" << result.css << "\n---------------------\n" << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime_error exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
