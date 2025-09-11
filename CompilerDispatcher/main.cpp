#include <iostream>
#include <vector>
#include <memory>
#include "Scanner/UnifiedScanner.h"
#include "CHTLLexer/CHTLLexer.h"
#include "CHTLCompiler/CHTLCompiler.h"
#include <filesystem>

int main(int argc, char* argv[]) {
    std::string source = R"(
        [Import] @Chtl from "Test/E2ETest/imported.chtl";

        @Element MyImportedTemplate;
    )";

    std::cout << "--- CHTL Compilation (Import System Test) ---" << std::endl;

    try {
        CHTL::CHTLCompiler compiler;
        std::filesystem::path executable_path = std::filesystem::canonical(argv[0]);
        std::filesystem::path project_root = executable_path.parent_path().parent_path().parent_path();
        std::string html_output = compiler.Compile(source, project_root.string());

        std::cout << "\nOutput:\n" << html_output << std::endl;
        std::cout << "\nCompilation successful." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Compilation error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
