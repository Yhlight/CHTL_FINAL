#include <iostream>
#include <vector>
#include <memory>
#include "Scanner/UnifiedScanner.h"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLParser/CHTLParser.h"
#include "CHTLGenerator/CHTLGenerator.h"
#include "CHTLNode/BaseNode.h"

int main() {
    std::string source = R"(
        [Template] @Style BaseStyle {
            color: red;
            font-size: 16px;
        }

        [Template] @Style DerivedStyle {
            @Style BaseStyle;
            font-size: 20px; // Override base
            font-weight: bold;
        }

        div {
            style {
                @Style DerivedStyle;
            }
        }
    )";

    std::cout << "--- CHTL Compilation (Template Inheritance Test) ---" << std::endl;

    try {
        CHTL::UnifiedScanner scanner;
        auto fragments = scanner.Scan(source);
        const std::string& chtl_source = fragments[0].content;

        CHTL::CHTLLexer lexer(chtl_source);
        auto tokens = lexer.Tokenize();

        CHTL::CHTLParser parser(tokens);
        auto ast = parser.Parse();

        if (ast.empty()) {
            std::cerr << "Parsing failed. Aborting." << std::endl;
            return 1;
        }

        CHTL::CHTLGenerator generator;
        std::string html_output = generator.Generate(ast);

        std::cout << "\nOutput:\n" << html_output << std::endl;
        std::cout << "\nCompilation successful." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Compilation error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
