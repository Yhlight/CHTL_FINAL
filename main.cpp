#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>

void printUsage() {
    std::cout << "Usage: chtl <inputfile> [-o <outputfile>]" << std::endl;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printUsage();
        return 1;
    }

    std::string inputFile;
    std::string outputFile;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-o") {
            if (i + 1 < argc) {
                outputFile = argv[++i];
            } else {
                std::cerr << "Error: -o option requires an output file." << std::endl;
                printUsage();
                return 1;
            }
        } else {
            if (inputFile.empty()) {
                inputFile = arg;
            } else {
                std::cerr << "Error: Multiple input files not supported." << std::endl;
                printUsage();
                return 1;
            }
        }
    }

    if (inputFile.empty()) {
        std::cerr << "Error: No input file specified." << std::endl;
        printUsage();
        return 1;
    }

    std::ifstream file(inputFile);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open input file '" << inputFile << "'" << std::endl;
        return 1;
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    try {
        Lexer lexer(source);
        Parser parser(lexer);
        NodeList ast = parser.parse();
        Generator generator(ast);
        std::string htmlOutput = generator.generate();

        if (!outputFile.empty()) {
            std::ofstream outFile(outputFile);
            if (outFile.is_open()) {
                outFile << htmlOutput;
                outFile.close();
                std::cout << "Successfully compiled " << inputFile << " to " << outputFile << std::endl;
            } else {
                std::cerr << "Error: Could not open output file '" << outputFile << "'" << std::endl;
                return 1;
            }
        } else {
            // Write to standard output
            std::cout << htmlOutput;
        }

    } catch (const std::runtime_error& e) {
        std::cerr << "Compilation Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
