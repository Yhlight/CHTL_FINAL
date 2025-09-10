#include <iostream>
#include <vector>
#include <string>

// Include all our compiler components
#include "Util/FileSystem/FileSystemUtil.h"
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"

// A helper function to print tokens for debugging
void printTokens(const std::vector<Token>& tokens) {
    for (const auto& token : tokens) {
        std::cout << "Type: " << token.typeToString()
                  << ", Lexeme: '" << token.lexeme
                  << "', Line: " << token.line << std::endl;
    }
}

int main() {
    const std::string filename = "test.chtl";

    // 1. Read the source file
    std::string source;
    try {
        source = FileSystemUtil::readFile(filename);
        std::cout << "--- Successfully read " << filename << " ---\n" << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    // 2. Lexer: Tokenize the source code
    Lexer lexer(source);
    auto tokens = lexer.tokenize();
    // printTokens(tokens); // Uncomment for debugging

    // 3. Parser: Build the AST
    Parser parser(tokens);
    auto program = parser.parseProgram();

    // 4. Check for parsing errors
    const auto& errors = parser.getErrors();
    if (!errors.empty()) {
        std::cerr << "--- Parser found " << errors.size() << " errors: ---" << std::endl;
        for (const auto& error : errors) {
            std::cerr << error << std::endl;
        }
        return 1;
    }

    // 5. Generator: Convert AST to HTML
    Generator generator;
    std::string html_output = generator.generate(program);

    // 6. Print the final result
    std::cout << "--- Generated HTML Output: ---" << std::endl;
    std::cout << html_output << std::endl;

    return 0;
}
