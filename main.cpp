#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "Scanner/UnifiedScanner.h"
#include "CHTLLexer/Lexer.h"
#include "CHTLParser/Parser.h"
#include "CHTLParser/ASTPrinter.h"

std::string readFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) {
        std::cerr << "Could not open file: " << path << std::endl;
        exit(1);
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <file>" << std::endl;
        return 1;
    }

    std::string source = readFile(argv[1]);

    // 1. Scan
    UnifiedScanner scanner(source);
    ScannedContent scanned_content = scanner.scan();

    // 2. Lexing the CHTL part
    Lexer lexer(scanned_content.chtl_content);
    std::vector<Token> tokens = lexer.tokenize();

    // 3. Parsing
    try {
        Parser parser(tokens);
        NodeList ast = parser.parse();

        // 4. Print AST for verification
        ASTPrinter printer;
        printer.print(ast);

    } catch (const Parser::ParseError& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;
}
