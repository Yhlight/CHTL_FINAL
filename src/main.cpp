#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

#include "CHTL/CHTLLexer/CHTLLexer.h"
#include "CHTL/CHTLParser/CHTLParser.h"
#include "CHTL/CHTLGenerator/CHTLGenerator.h"
#include "CHTL/CHTLLoader/CHTLLoader.h"
#include "CHTL/CHTLDebug/ASTPrinter.h"
#include "Scanner/UnifiedScanner.h"
#include "CompilerDispatcher/CompilerDispatcher.h"


int main(int argc, char* argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <input_file.chtl> [-o <output_file.html>] [--debug]" << std::endl;
        return 1;
    }

    std::string input_path;
    std::string output_path;
    bool debug_mode = false;

    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "-o") {
            if (i + 1 < argc) {
                output_path = argv[++i];
            } else {
                std::cerr << "-o option requires one argument." << std::endl;
                return 1;
            }
        } else if (arg == "--debug") {
            debug_mode = true;
        } else if (arg == "-h" || arg == "--help") {
            std::cout << "Usage: " << argv[0] << " <input_file.chtl> [-o <output_file.html>] [--debug]" << std::endl;
            return 0;
        } else {
            if (input_path.empty()) {
                input_path = arg;
            } else {
                std::cerr << "Unrecognized argument: " << arg << std::endl;
                return 1;
            }
        }
    }

    if (input_path.empty()) {
        std::cerr << "No input file specified." << std::endl;
        return 1;
    }

    std::ifstream file(input_path);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open file " << input_path << std::endl;
        return 1;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string source = buffer.str();

    // 1. Scan for placeholders
    CHTL::UnifiedScanner scanner(source);
    CHTL::ScanResult scanResult = scanner.scan();

    // 2. Setup compilation context and dispatcher
    CHTL::CHTLLoader loader;
    auto context = std::make_shared<CHTL::ParserContext>();
    auto dispatcher = std::make_shared<CHTL::CompilerDispatcher>(scanResult.placeholders);

    // 3. Lex the modified source
    CHTL::CHTLLexer lexer(scanResult.modified_source);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();

    // 4. Parse all files (starting with the main one)
    CHTL::CHTLParser parser(scanResult.modified_source, tokens, loader, input_path, context);
    std::unique_ptr<CHTL::RootNode> ast = parser.parse();

    // 5. Print debug info if requested
    if (debug_mode) {
        std::cerr << "--- Modified Source ---\n" << scanResult.modified_source << "\n-----------------------\n";
        std::cerr << "--- Placeholders ---\n";
        for (const auto& pair : scanResult.placeholders) {
            std::cerr << pair.first << " -> " << pair.second << "\n";
        }
        std::cerr << "--------------------\n";

        CHTL::ASTPrinter astPrinter;
        std::string astString = astPrinter.print(*ast, context);
        std::cerr << astString << "-----------\n";
    }

    // 6. Generator
    CHTL::CHTLGenerator generator(dispatcher);
    std::string html = generator.generate(*ast);

    if (!output_path.empty()) {
        std::ofstream outFile(output_path);
        if (!outFile.is_open()) {
            std::cerr << "Error: Could not open output file " << output_path << std::endl;
            return 1;
        }
        outFile << html;
    } else {
        std::cout << html << std::endl;
    }

    return 0;
}
