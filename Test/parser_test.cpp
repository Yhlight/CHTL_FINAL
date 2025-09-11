#include "../CHTL/CHTLLexer/CHTLLexer.h"
#include "../CHTL/CHTLParser/CHTLParser.h"
#include "../CHTL/CHTLNode/ProgramNode.h"
#include <iostream>
#include <vector>
#include <string>

int main() {
    std::cout << "--- Parser Debug Test ---" << std::endl;

    std::string source = R"([Template] @Style MyTestStyle {
    color: red;
}

div {
    style {
        @Style MyTestStyle;
    }
}
)";

    std::cout << "--- Source ---" << std::endl;
    std::cout << source << std::endl;
    std::cout << "--------------" << std::endl;

    CHTL::CHTLLexer lexer(source);
    // Enable debug mode
    CHTL::CHTLParser parser(lexer, true);

    auto program = parser.ParseProgram();

    const auto& errors = parser.Errors();
    if (!errors.empty()) {
        std::cout << "\n--- Parser Errors ---" << std::endl;
        for (const auto& err : errors) {
            std::cout << err << std::endl;
        }
    } else {
        std::cout << "\n--- No Parser Errors ---" << std::endl;
    }

    if (program) {
        std::cout << "\n--- Final AST ---" << std::endl;
        std::cout << program->ToString() << std::endl;
    } else {
        std::cout << "\n--- Parsing Failed: Null Program Node ---" << std::endl;
    }

    std::cout << "-----------------------" << std::endl;

    return 0;
}
