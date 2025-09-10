#include "../CHTL_JS/CHTLJSLexer/CHTLJSLexer.h"
#include "../CHTL_JS/CHTLJSParser/CHTLJSParser.h"
#include "../CHTL_JS/CHTLJSGenerator/CHTLJSGenerator.h"
#include <iostream>

int main() {
    std::string source = R"(
        {{.my-button}}->listen {
            click: someClickHandler
        }
    )";

    std::cout << "--- Starting CHTL JS Generator Test ---" << std::endl;

    try {
        // 1. Lexer
        CHTLJSLexer lexer(source);

        // 2. Parser
        CHTLJSParser parser(lexer);
        auto ast = parser.parse();
        std::cout << "[SUCCESS] CHTL JS Parsing completed." << std::endl;

        // 3. Generator
        CHTLJSGenerator generator(ast);
        std::string jsOutput = generator.generate();
        std::cout << "[SUCCESS] CHTL JS Generation completed." << std::endl;

        // 4. Print output
        std::cout << "\n--- Generated JavaScript ---\n" << std::endl;
        std::cout << jsOutput << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\n--- CHTL JS Generator Test Finished ---" << std::endl;
    return 0;
}
