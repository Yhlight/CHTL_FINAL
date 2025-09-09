#include <iostream>
#include <string>
#include <vector>
#include "CHTL.h"
#include "CHTLJS.h"

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: chtl <input_file> <output_file>" << std::endl;
        std::cout << "       chtl --version" << std::endl;
        std::cout << "       chtl --help" << std::endl;
        return 1;
    }
    
    std::string command = argv[1];
    
    if (command == "--version") {
        std::cout << "CHTL Compiler v1.0.0" << std::endl;
        std::cout << "Based on CHTL.md specifications" << std::endl;
        return 0;
    }
    
    if (command == "--help") {
        std::cout << "CHTL Compiler - A C++ implementation of CHTL language" << std::endl;
        std::cout << "Usage: chtl <input_file> <output_file>" << std::endl;
        std::cout << "       chtl --version" << std::endl;
        std::cout << "       chtl --help" << std::endl;
        std::cout << std::endl;
        std::cout << "CHTL is a hypertext language that provides a better way to write HTML." << std::endl;
        std::cout << "Features:" << std::endl;
        std::cout << "  - Comments: // /* */ --" << std::endl;
        std::cout << "  - Text nodes: text { } and text: attribute" << std::endl;
        std::cout << "  - Literals: unquoted, single-quoted, double-quoted strings" << std::endl;
        std::cout << "  - CE equivalence: : and = are equivalent" << std::endl;
        std::cout << "  - Element nodes: all HTML elements supported" << std::endl;
        std::cout << "  - Attributes: attr: value; or attr = value;" << std::endl;
        std::cout << "  - Local style blocks: style { } with auto class/id addition" << std::endl;
        std::cout << "  - Property condition expressions: prop: cond ? opt1 : opt2;" << std::endl;
        std::cout << "  - Templates: [Template] @Style/@Element/@Var" << std::endl;
        std::cout << "  - Custom: [Custom] with specialization support" << std::endl;
        std::cout << "  - Origin embedding: [Origin] @Html/@Style/@JavaScript" << std::endl;
        std::cout << "  - Import system: [Import] with various types" << std::endl;
        std::cout << "  - Namespaces: [Namespace] for module pollution prevention" << std::endl;
        std::cout << "  - Constraints: except keyword for domain restrictions" << std::endl;
        std::cout << "  - Configuration: [Configuration] with Name remapping" << std::endl;
        std::cout << "  - CHTL JS: Independent DSL that compiles to JavaScript" << std::endl;
        std::cout << "  - Module system: CMOD and CJMOD support" << std::endl;
        return 0;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = argv[2];
    
    try {
        CHTL::CHTLCompiler compiler;
        
        std::cout << "Compiling " << inputFile << " to " << outputFile << "..." << std::endl;
        
        bool success = compiler.compile(inputFile, outputFile);
        
        if (success) {
            std::cout << "Compilation successful!" << std::endl;
            
            auto errors = compiler.getErrors();
            auto warnings = compiler.getWarnings();
            
            if (!warnings.empty()) {
                std::cout << "Warnings:" << std::endl;
                for (const auto& warning : warnings) {
                    std::cout << "  " << warning << std::endl;
                }
            }
            
            return 0;
        } else {
            std::cout << "Compilation failed!" << std::endl;
            
            auto errors = compiler.getErrors();
            for (const auto& error : errors) {
                std::cout << "Error: " << error << std::endl;
            }
            
            return 1;
        }
    } catch (const std::exception& e) {
        std::cout << "Fatal error: " << e.what() << std::endl;
        return 1;
    }
}