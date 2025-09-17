#include <iostream>
#include <string>
#include <vector>
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLNode/Node.h"

int main() {
    std::string input = R"----(
        [Template] @Var Theme {
            brandColor: "rgb(255, 0, 0)";
            secondaryColor: "#EEE";
        }

        [Template] @Style ThemedText {
            color: Theme(brandColor);
            background-color: Theme(secondaryColor);
        }

        body {
            h1 {
                style {
                    @Style ThemedText;
                    font-weight: bold;
                }
                text: "This text is themed!";
            }
        }
    )----";

    CHTL::Lexer lexer(input);
    CHTL::Parser parser(lexer);

    auto program = parser.parseProgram();

    if (!parser.getErrors().empty()) {
        std::cerr << "--- Parser Errors ---" <<
        std::endl;
        for (const auto& error : parser.getErrors()) {
            std::cerr << error << std::endl;
        }
        return 1;
    }

    if (!program) {
        std::cerr << "Parsing failed, returned null program." << std::endl;
        return 1;
    }

    CHTL::Generator generator(parser.getContext());
    std::string html_output = generator.generate(program.get());

    std::cout << html_output << std::endl;

    return 0;
}
