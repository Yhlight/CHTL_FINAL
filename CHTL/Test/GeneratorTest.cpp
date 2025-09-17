#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLGenerator/Generator.h"
#include <iostream>

int main() {
    const char* sample_code = R"CHTL(
html {
    head {
        title {
            text: "My CHTL Page";
        }
        meta {
            charset: "UTF-8";
        }
    }
    body {
        div {
            id = "main";
            class: "container";

            p {
                text { "Welcome to CHTL!" }
            }

            img {
                src: "image.png";
                alt: "An image";
            }
        }
    }
}
)CHTL";

    std::cout << "--- Full Compilation Test (Lexer -> Parser -> Generator) ---" << std::endl;

    try {
        // 1. Lexing
        Lexer lexer(sample_code);

        // 2. Parsing
        Parser parser(lexer);
        std::shared_ptr<RootNode> ast = parser.parse();

        // 3. Generation
        Generator generator;
        std::string htmlOutput = generator.generate(ast);

        std::cout << "\n--- Generated HTML Output ---" << std::endl;
        std::cout << htmlOutput;
        std::cout << "---------------------------" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
