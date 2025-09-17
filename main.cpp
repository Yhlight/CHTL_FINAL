#include <iostream>
#include <fstream>
#include <string>

#include "Util/FileSystem/FileSystem.h"
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"

void printParserErrors(const std::vector<std::string>& errors) {
    std::cerr << "Parser has " << errors.size() << " errors:" << std::endl;
    for (const auto& msg : errors) {
        std::cerr << "\t" << msg << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename.chtl>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    if (!CHTL::FileSystem::fileExists(filename)) {
        std::cerr << "Error: File '" << filename << "' not found." << std::endl;
        return 1;
    }

    std::string source = CHTL::FileSystem::readFile(filename);
    if (source.empty() && CHTL::FileSystem::fileExists(filename)) {
         // File might be empty, which is not an error.
         // Let the parser handle it.
    } else if (source.empty()) {
        std::cerr << "Error: Could not read file '" << filename << "'." << std::endl;
        return 1;
    }


    CHTL::CHTLLexer lexer(source);
    CHTL::CHTLParser parser(lexer);
    auto program = parser.ParseProgram();

    if (!parser.Errors().empty()) {
        printParserErrors(parser.Errors());
        return 1;
    }

    std::cout << "Successfully parsed program. AST:" << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << program->ToString() << std::endl;
    std::cout << "------------------------------------" << std::endl << std::endl;

    CHTL::CHTLGenerator generator;
    std::string html_output = generator.Generate(program.get());

    std::cout << "Generated HTML Output:" << std::endl;
    std::cout << "------------------------------------" << std::endl;
    std::cout << html_output << std::endl;

    return 0;
}
