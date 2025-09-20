#include "../../CHTL/CHTLCompiler.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include <iostream>

int main() {
    // The entry point to our test project
    std::string entry_file = "Test/FullPipelineTest/test.chtl";

    std::cout << "--- Starting Compilation from Entry File: " << entry_file << " ---\n" << std::endl;

    try {
        // 1. Compilation (including loading, lexing, parsing, and handling imports)
        CHTL::CHTLCompiler compiler;
        std::unique_ptr<CHTL::CHTLContext> context = compiler.compile(entry_file);

        // 2. Generation
        // For this test, we generate from the entry file's AST.
        CHTL::CHTLGenerator generator(*context);
        CHTL::BaseNode* entry_ast = context->files[entry_file].get();
        CHTL::CompilationResult result = generator.generate(entry_ast);

        // 3. Verification
        std::cout << "--- Generated HTML ---\n" << result.html << "\n----------------------\n" << std::endl;
        std::cout << "--- Generated CSS ---\n" << result.css << "\n---------------------\n" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Caught an exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
