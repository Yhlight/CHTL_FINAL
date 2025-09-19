#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>

int main() {
    // 1. Create the external file to be imported
    std::string external_html = "<div><strong>This content is from an external file!</strong></div>";
    CHTL::FileSystem::writeFile("Test/ImportTest/external.html", external_html);

    // 2. Define the CHTL source that imports and uses the file
    std::string chtl_source = R"~(
[Import] @Html from "external.html" as myHtml;

body {
    h1 { text: "Import Test"; }
    [Origin] @Html myHtml;
}
)~";

    std::cout << "--- Input CHTL ---\n" << chtl_source << "\n------------------\n" << std::endl;

    try {
        CHTL::CHTLLexer lexer(chtl_source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();
        CHTL::CHTLParser parser(chtl_source, tokens, "Test/ImportTest/main.chtl");
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
