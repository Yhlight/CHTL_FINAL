#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>

int main() {
    std::string chtl_source = R"~(
[Namespace] space1 {
    [Template] @Element Box {
        div { text: "This is a box from space1"; }
    }
}

[Namespace] space2 {
    [Namespace] nested {
        [Template] @Element Box {
            div { text: "This is a box from space2.nested"; }
        }
    }
}

body {
    @Element Box from space1;
    @Element Box from space2.nested;
}
)~";

    std::cout << "--- Input CHTL ---\n" << chtl_source << "\n------------------\n" << std::endl;

    try {
        CHTL::CHTLLexer lexer(chtl_source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();
        CHTL::CHTLParser parser(chtl_source, tokens, "Test/NamespaceTest/main.chtl");
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
