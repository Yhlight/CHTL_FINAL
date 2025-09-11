#include <iostream>
#include <fstream>
#include <sstream>
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLResolver/CHTLTemplateResolver.h"
#include "CHTLAnalyzer/CHTLSemanticAnalyzer.h"
#include "CHTLGenerator/CHTLGenerator.h"

// Function to read a file into a string
std::string readFile(const std::string& filepath) {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << filepath << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

void printErrors(const std::vector<std::string>& errors) {
    std::cerr << "Parser has " << errors.size() << " errors:" << std::endl;
    for (const auto& error : errors) {
        std::cerr << "\t" << error << std::endl;
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename.chtl>" << std::endl;
        return 1;
    }

    std::string filepath = argv[1];
    std::string source = readFile(filepath);

    if (source.empty()) {
        return 1;
    }

    // 1. Lexing
    CHTL::CHTLLexer lexer(source);

    // 2. Parsing
    CHTL::CHTLParser parser(lexer);
    auto programNode = parser.ParseProgram();

    if (!parser.Errors().empty()) {
        printErrors(parser.Errors());
        if (programNode) {
            std::cout << "\n--- Partial AST for Debugging ---\n";
            std::cout << programNode->ToString() << std::endl;
            std::cout << "---------------------------------\n";
        }
        return 1;
    }
    if (!programNode) {
        std::cerr << "Parsing failed: Program node is null." << std::endl;
        return 1;
    }

    // 3. Template Resolution
    CHTL::CHTLTemplateResolver resolver;
    resolver.Process(programNode);

    // 4. Semantic Analysis
    CHTL::CHTLSemanticAnalyzer analyzer;
    analyzer.Process(programNode);

    // 5. Generation
    // The generator currently expects a single root node, but now we have a program.
    // We will generate code for each statement in the program.
    std::string finalHtml;
    for(const auto& stmt : programNode->statements) {
        CHTL::CHTLGenerator generator(stmt);
        finalHtml += generator.Generate();
    }


    std::cout << "--- CHTL Compiler Output ---" << std::endl;
    std::cout << finalHtml << std::endl;
    std::cout << "----------------------------" << std::endl;

    return 0;
}
