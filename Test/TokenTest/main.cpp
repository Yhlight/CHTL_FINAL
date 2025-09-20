#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include <iostream>
#include <vector>

int main() {
    std::string source = R"(
        # this is a generator comment
        [Namespace] MyStuff
        [Configuration] {
            DEBUG_MODE = true;
        }
        div {
            class: box container;
            text { unquoted text here }
        }
    )";

    CHTL::CHTLLexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();

    for (const auto& token : tokens) {
        std::cout << "Type: " << CHTL::tokenTypeToString(token.type)
                  << ", Lexeme: '" << token.lexeme
                  << "', Line: " << token.line << std::endl;
    }

    return 0;
}
