#include <iostream>
#include <vector>
#include <memory>
#include "Scanner/UnifiedScanner.h"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "CHTLNode/BaseNode.h"

int main() {
    // This source showcases templates, variables, and expressions.
    std::string source = R"(
        [Template] @Var Theme {
            primaryColor: blue;
            baseMargin: 10px;
        }

        div {
            style {
                color: Theme(primaryColor);
                margin: Theme(baseMargin) * 2;
            }
        }
    )";

    std::cout << "--- CHTL Compiler Showcase ---" << std::endl;

    try {
        // 1. Scanning (currently passes the whole source as CHTL)
        CHTL::UnifiedScanner scanner;
        std::vector<CHTL::CodeFragment> fragments = scanner.Scan(source);

        if (fragments.empty()) {
            return 0; // Nothing to compile
        }

        const std::string& chtl_source = fragments[0].content;

        // 2. Lexing
        CHTL::CHTLLexer lexer(chtl_source);
        std::vector<CHTL::Token> tokens = lexer.Tokenize();

        // 3. Parsing
        CHTL::CHTLParser parser(tokens);
        CHTL::NodeList ast = parser.Parse();

        if (ast.empty()) {
            std::cerr << "Parsing failed. Aborting." << std::endl;
            return 1;
        }

        // 4. Generation
        CHTL::CHTLGenerator generator;
        std::string html_output = generator.Generate(ast);

        std::cout << "\nInput CHTL:\n" << source << std::endl;
        std::cout << "\nOutput HTML:\n" << html_output << std::endl;
        std::cout << "\nCompilation successful." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Compilation error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
