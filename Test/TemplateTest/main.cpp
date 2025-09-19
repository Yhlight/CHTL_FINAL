#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>

int main() {
    std::string chtl_source = R"~(
[Template] @Var Theme {
    primaryColor: "blue";
    secondaryColor: "white";
    basePadding: 10px;
}

[Template] @Style BaseButton {
    padding: Theme(basePadding);
    border: "1px solid black";
}

[Template] @Style PrimaryButton {
    inherit @Style BaseButton;
    background-color: Theme(primaryColor);
    color: Theme(secondaryColor);
}

[Template] @Element MyButton {
    button {
        style {
            @Style PrimaryButton;
        }
        text: "Click Me";
    }
}

body {
    @Element MyButton;
}
)~";

    std::cout << "--- Input CHTL ---\n" << chtl_source << "\n------------------\n" << std::endl;

    try {
        CHTL::CHTLLexer lexer(chtl_source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();
        CHTL::CHTLParser parser(chtl_source, tokens, "Test/TemplateTest/main.chtl");
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
