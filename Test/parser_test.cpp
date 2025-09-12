#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLNode/ProgramNode.h"
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

// Helper function to read a file into a string
static std::string readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Could not open test file: " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    std::cout << "--- Parser Debug Test ---" << std::endl;

    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename.chtl>" << std::endl;
        return 1;
    }
    std::string filepath = argv[1];
    std::string source = readFile(filepath);
    if (source.empty()) return 1;

    std::cout << "--- Source: " << filepath << " ---" << std::endl;
    std::cout << source << std::endl;
    std::cout << "--------------" << std::endl;

    CHTL::CHTLLexer lexer(source);
    CHTL::CHTLParser parser(lexer, false); // Disable debug logging for cleaner output

    auto program = parser.ParseProgram();

    const auto& errors = parser.Errors();
    if (!errors.empty()) {
        std::cout << "\n--- Parser Errors ---" << std::endl;
        for (const auto& err : errors) {
            std::cout << err << std::endl;
        }
    } else {
        std::cout << "\n--- No Parser Errors ---" << std::endl;
    }

    if (program) {
        std::cout << "\n--- Final AST ---" << std::endl;
        std::cout << program->ToString() << std::endl;
    } else {
        std::cout << "\n--- Parsing Failed: Null Program Node ---" << std::endl;
    }

    std::cout << "-----------------------" << std::endl;

    return 0;
}
