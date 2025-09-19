#include "../../CHTL/Scanner/CHTLUnifiedScanner.h"
#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>

int main() {
    // Create library file
    CHTL::FileSystem::writeFile("Test/CustomTest/lib.chtl",
        "[Custom] @Element Card {\n"
        "    div { class: \"card-header\"; }\n"
        "    div { class: \"card-body\"; }\n"
        "    div { class: \"card-footer\"; }\n"
        "}"
    );

    std::string entry_point = "Test/CustomTest/main.chtl";
    std::string source =
        "[Import] @Chtl from \"lib.chtl\";\n"
        "body {\n"
        "    @Element Card {\n"
        "        delete div[2];\n"
        "    }\n"
        "    hr{}\n"
        "    @Element Card {\n"
        "        insert after div[1] {\n"
        "            p { text: \"Inserted!\"; }\n"
        "        }\n"
        "    }\n"
        "}";

    CHTL::FileSystem::writeFile(entry_point, source);

    std::cout << "--- Input CHTL from " << entry_point << " ---\n" << source << "\n------------------\n" << std::endl;

    try {
        // 1. Scan the source for different language blocks
        CHTL::CHTLUnifiedScanner scanner(source);
        CHTL::ScanResult scan_result = scanner.scan();

        // 2. Lex the sanitized CHTL source
        CHTL::CHTLLexer lexer(scan_result.sanitized_source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();

        // 3. Parse the tokens to build the AST
        CHTL::CHTLParser parser(scan_result.sanitized_source, tokens, entry_point);
        std::unique_ptr<CHTL::BaseNode> ast = parser.parse();

        // 4. Generate the final output, passing the placeholders to the generator
        CHTL::CHTLGenerator generator(parser.getTemplateDefinitions(), scan_result.placeholders);
        CHTL::CompilationResult result = generator.generate(ast.get());

        std::cout << "--- Generated HTML ---\n" << result.html << "\n----------------------\n" << std::endl;
        std::cout << "--- Generated CSS ---\n" << result.css << "\n---------------------\n" << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime_error exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
