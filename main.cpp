#include <iostream>
#include <string>
#include <stdexcept>
#include "Util/FileSystem/FileSystem.h"
#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLContext/CHTLContext.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: ./chtl_compiler <input_file.chtl>" << std::endl;
        return 1;
    }

    std::string filePath = argv[1];
    std::string source = CHTL::FileSystem::readFile(filePath);

    if (source.empty() && filePath != "/dev/null") {
        std::cerr << "Error: Could not read file or file is empty: " << filePath << std::endl;
        return 1;
    }

    try {
        // 1. Initialize Lexer and Context
        CHTL::CHTLLexer lexer(source);
        CHTL::CHTLContext context;

        // 2. Parse the source to build the AST and populate the context
        CHTL::CHTLParser parser(lexer, context);
        std::unique_ptr<CHTL::BaseNode> ast = parser.parse();

        // 3. Generate HTML from the AST
        CHTL::CHTLGenerator generator(context);
        std::string html = generator.generate(*ast);

        // 4. Output the result
        std::cout << html << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
