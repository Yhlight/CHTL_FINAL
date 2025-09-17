#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <memory>

#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLContext/Context.h"

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
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << filename << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    Lexer lexer(source);
    Context context;
    Parser parser(lexer, context);
    std::shared_ptr<Program> program = parser.parseProgram();

    if (!parser.getErrors().empty()) {
        printParserErrors(parser.getErrors());
        return 1;
    }

    Generator generator(program, context);
    generator.generate();

    std::cout << "--- Generated HTML ---" << std::endl;
    std::cout << generator.getHtml() << std::endl;

    std::cout << "\n--- Generated CSS ---" << std::endl;
    std::cout << generator.getCss() << std::endl;

    return 0;
}
