#include <iostream>
#include <fstream>
#include <sstream>
#include "CHTLLoader/CHTLLoader.h"
#include "CHTLResolver/CHTLTemplateResolver.h"
#include "CHTLAnalyzer/CHTLSemanticAnalyzer.h"
#include "CHTLGenerator/CHTLGenerator.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <entry_filename.chtl>" << std::endl;
        return 1;
    }

    std::string entry_filepath = argv[1];

    // 1. Loading & Parsing
    std::cout << "--- Loading ---\n";
    CHTL::CHTLLoader loader;
    auto programNode = loader.Load(entry_filepath);

    if (!programNode) {
        std::cerr << "Fatal error during loading phase." << std::endl;
        return 1;
    }
    std::cout << "--- Combined AST after Loading ---\n";
    std::cout << programNode->ToString() << std::endl;

    // 2. Template Resolution
    std::cout << "\n--- Resolving Templates ---\n";
    CHTL::CHTLTemplateResolver resolver;
    resolver.Process(programNode);
    std::cout << "--- AST after Resolving ---\n";
    std::cout << programNode->ToString() << std::endl;


    // 3. Semantic Analysis
    CHTL::CHTLSemanticAnalyzer analyzer;
    analyzer.Process(programNode);

    // 4. Generation
    std::string finalHtml;
    for(const auto& stmt : programNode->statements) {
        CHTL::CHTLGenerator generator(stmt);
        finalHtml += generator.Generate();
    }

    std::cout << "\n--- CHTL Compiler Output ---\n";
    std::cout << finalHtml << std::endl;
    std::cout << "----------------------------" << std::endl;

    return 0;
}
