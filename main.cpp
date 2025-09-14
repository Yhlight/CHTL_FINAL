#include <iostream>
#include <string>
#include "CompilerDispatcher/CompilerDispatcher.h"

int main(int argc, char* argv[]) {
    std::cout << "CHTL Compiler v1.0.0" << std::endl;
    
    if (argc < 2) {
        std::cout << "Usage: chtl <input_file> [output_file]" << std::endl;
        return 1;
    }
    
    std::string inputFile = argv[1];
    std::string outputFile = (argc > 2) ? argv[2] : "output.html";
    
    try {
        CompilerDispatcher dispatcher;
        dispatcher.compile(inputFile, outputFile);
        std::cout << "Compilation successful. Output: " << outputFile << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Compilation error: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}