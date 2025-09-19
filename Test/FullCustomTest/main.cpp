#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>

int main() {
    std::string chtl_source = R"~(
[Template] @Var Theme {
    specialColor: "purple";
}

[Custom] @Style Valueless {
    color,
    font-size;
    border;
}

[Template] @Style ToBeDeleted {
    text-decoration: "underline";
}

[Custom] @Element Card {
    div {
        class: "header";
        @Element ToBeDeleted;
    }
    div {
        class: "content";
        p { text: "Original content"; }
    }
}

body {
    @Element Card {
        // Add style to the first div
        div {
            style {
                @Style Valueless {
                    color: Theme(specialColor = "green"); // Specialize variable
                    font-size: 20px;
                    border: "2px solid red";
                }
            }
        }

        // Insert an element
        insert after div[0] {
            hr {}
        }

        // Delete an inherited template
        delete @Element ToBeDeleted;
    }
}
)~";

    std::cout << "--- Input CHTL ---\n" << chtl_source << "\n------------------\n" << std::endl;

    try {
        CHTL::CHTLLexer lexer(chtl_source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();
        CHTL::CHTLParser parser(chtl_source, tokens, "Test/FullCustomTest/main.chtl");
        std::unique_ptr<CHTL::BaseNode> ast = parser.parse();
        CHTL::CHTLGenerator generator(parser.getTemplateDefinitions());
        CHTL::CompilationResult result = generator.generate(ast.get());

        std::cout << "--- Generated HTML ---\n" << result.html << "\n----------------------\n" << std::endl;
        std::cout << "--- Generated CSS ---\n" << result.css << "\n---------------------\n" << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime_error exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
