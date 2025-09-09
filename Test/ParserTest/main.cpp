#include "../../CHTL/CHTLLexer/Lexer.h"
#include "../../CHTL/CHTLParser/Parser.h"
#include "../../CHTL/CHTLGenerator/Generator.h"
#include "../../CHTL/CHTLContext/Context.h"
#include "../../CHTL/CHTLLoader/Loader.h"
#include <iostream>
#include <vector>

void runPipelineTest(const std::string& entry_file_path) {
    std::cout << "--- CHTL Entry File: " << entry_file_path << " ---" << std::endl;

    try {
        std::string source = CHTL::Loader::loadFile(entry_file_path);
        std::cout << source << std::endl;
        std::cout << "--------------------" << std::endl;

        CHTL::CHTLContext context;
        context.addImportedPath(entry_file_path); // Add the entry file to prevent self-import loops

        // 1. Lexer
        CHTL::Lexer lexer(source);
        auto tokens = lexer.tokenize();

        // 2. Parser
        CHTL::Parser parser(tokens, context);
        CHTL::DocumentNode ast = parser.parse();

        // 3. Generator
        CHTL::Generator generator(context);
        std::string html_output = generator.generate(ast);

        std::cout << "--- Generated HTML ---" << std::endl;
        std::cout << html_output << std::endl;
        std::cout << "----------------------" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }
    std::cout << "\n\n";
}

int main() {
    runPipelineTest("main.chtl");
    return 0;
}
