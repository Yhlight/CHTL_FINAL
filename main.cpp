#include <iostream>
#include <string>
#include "Util/FileSystem/FileSystem.hpp"
#include "CHTL/CHTLLexer/Lexer.hpp"
#include "CHTL/CHTLParser/Parser.hpp"
#include "CHTL/CHTLGenerator/Generator.hpp"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <input.chtl> <output.html>" << std::endl;
        return 1;
    }

    std::string inputPath = argv[1];
    std::string outputPath = argv[2];

    std::cout << "Reading from: " << inputPath << std::endl;
    std::string source = CHTL::FileSystem::readFile(inputPath);
    if (source.empty()) {
        std::cerr << "Error: Could not read file or file is empty: " << inputPath << std::endl;
        return 1;
    }

    CHTL::Lexer lexer(source);
    CHTL::Parser parser(lexer);

    auto program = parser.parseProgram();

    auto errors = parser.getErrors();
    if (!errors.empty()) {
        std::cerr << "Parser errors:" << std::endl;
        for (const auto& error : errors) {
            std::cerr << "- " << error << std::endl;
        }
        return 1;
    }

    CHTL::Generator generator;
    std::string htmlOutput = generator.generate(program.get());

    std::cout << "Writing to: " << outputPath << std::endl;
    if (!CHTL::FileSystem::writeFile(outputPath, htmlOutput)) {
        std::cerr << "Error: Could not write to file: " << outputPath << std::endl;
        return 1;
    }

    std::cout << "Compilation successful!" << std::endl;

    return 0;
}
