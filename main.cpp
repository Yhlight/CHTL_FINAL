#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>
#include "CHTLLexer/Lexer.h"
#include "CHTLParser/Parser.h"
#include "CHTLGenerator/Generator.h"
#include "CHTL_JS/CHTLJSLexer/Token.h"
#include "CHTL_JS/CHTLJSLexer/Lexer.h"
#include "CHTL_JS/CHTLJSParser/Parser.h"

void testCHTLJS() {
    std::cout << "\n--- Testing CHTL JS Parser ---\n";
    std::string source = R"~(
        vir test = listen {
            click: () => {
                {{box}}->textContent = "Clicked!";
            }
        };
    )~";

    CHTLJSLexer lexer(source);
    std::vector<CHTLJSToken> tokens = lexer.tokenize();

    try {
        CHTLJSParser parser(tokens);
        CHTLJSNodeList ast = parser.parse();
        std::cout << "CHTL JS Parser ran successfully!" << std::endl;
    } catch (const CHTLJSParser::ParseError& e) {
        std::cerr << e.what() << std::endl;
    }

    std::cout << "----------------------------\n";
}


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
        testCHTLJS(); // Run CHTL JS parser test if no file is provided
        return 1;
    }

    std::string source = readFile(argv[1]);

    // 1. Lexing
    Lexer lexer(source);
    std::vector<Token> tokens = lexer.tokenize();

    // 2. Parsing
    NodeList ast;
    try {
        Parser parser(tokens);
        ast = parser.parse();
    } catch (const Parser::ParseError& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    // 3. Generating
    Generator generator;
    std::string html_output = generator.generate(ast);

    std::cout << html_output << std::endl;

    return 0;
}
