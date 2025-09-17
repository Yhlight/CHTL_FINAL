#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include "CHTL/CHTLLexer/Lexer.h"
#include "CHTL/CHTLParser/Parser.h"
#include "CHTL/CHTLGenerator/Generator.h"
#include "CHTL/CHTLNode/Node.h"

int main() {
    std::string input = R"----(
        [Custom] @Style TextSet {
            color;
            font-size;
            font-weight: normal;
        }

        body {
            p {
                style {
                    @Style TextSet {
                        color: blue;
                        font-size: 22px;
                    }
                    border: 1px solid black;
                }
                text: "This text is customized!";
            }
        }
    )----";

    CHTL::Lexer lexer(input);
    CHTL::Parser parser(lexer);

    std::shared_ptr<CHTL::Node> program = parser.parseProgram();

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
    std::string html_output = generator.generate(program);

    std::cout << html_output << std::endl;

    return 0;
}
