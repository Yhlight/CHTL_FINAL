#include "../../CHTL/CHTLCompiler.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include <iostream>

int main() {
    std::string entry_file = "Test/ElementTemplateTest/test.chtl";

    std::cout << "--- Starting Compilation from Entry File: " << entry_file << " ---\n" << std::endl;

    try {
        CHTL::CHTLCompiler compiler;
        std::unique_ptr<CHTL::BaseNode> ast = compiler.compile(entry_file);

        CHTL::CHTLGenerator generator(compiler.getTemplateDefinitions());
        CHTL::CompilationResult result = generator.generate(ast.get());

        std::cout << "--- Generated HTML ---\n" << result.html << "\n----------------------\n" << std::endl;
        std::cout << "--- Generated CSS ---\n" << result.css << "\n---------------------\n" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Caught an exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
