#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

int main() {
    std::ifstream file("Test/sample.chtl");
    if (!file.is_open()) {
        std::cerr << "Error: Could not open Test/sample.chtl" << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    std::cout << "--- Starting End-to-End Compiler Test ---" << std::endl;

    try {
        // 1. Lexer
        Lexer lexer(source);

        // 2. Parser
        Parser parser(lexer);
        NodeList ast = parser.parse();
        std::cout << "[SUCCESS] Parsing completed." << std::endl;

        // 3. Generator
        Generator generator(ast);
        std::string htmlOutput = generator.generate();
        std::cout << "[SUCCESS] HTML generation completed." << std::endl;

        // 4. Print output
        std::cout << "\n--- Generated HTML ---\n" << std::endl;
        std::cout << htmlOutput << std::endl;

        // 5. Save to file
        std::ofstream outFile("index.html");
        if (outFile.is_open()) {
            outFile << htmlOutput;
            outFile.close();
            std::cout << "\n[SUCCESS] Output saved to index.html" << std::endl;
        } else {
            std::cerr << "Error: Could not open index.html for writing." << std::endl;
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\n--- Compiler Test Finished ---" << std::endl;

    return 0;
}
