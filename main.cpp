#include <iostream>
#include <fstream>
#include <vector>
#include "Util/FileSystem/FileSystem.hpp"
#include "CHTL/CHTLLexer/Lexer.hpp"
#include "CHTL/CHTLParser/Parser.hpp"
#include "CHTL/AstPrinter.hpp"
#include "CHTL/CHTLGenerator/HtmlGenerator.hpp"


int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    try {
        // 1. Read file
        std::string source = CHTL::FileSystem::readFile(filename);

        // 2. Lex
        CHTL::Lexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();

        // 3. Parse
        CHTL::Parser parser(tokens);
        std::cout << "DEBUG: Lexer produced " << tokens.size() << " tokens." << std::endl;
        std::vector<CHTL::NodePtr> ast = parser.parse();
        std::cout << "DEBUG: Parser produced " << ast.size() << " AST nodes." << std::endl;

        // 4. (Optional) Print AST for debugging
        // CHTL::AstPrinter printer;
        // std::string astString = printer.print(ast);
        // std::cout << astString << std::endl;

        // 5. Generate HTML
        CHTL::HtmlGenerator generator;
        std::string html = generator.generate(ast);
        std::cout << html << std::endl;


    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
