#include "../../CHTL/CHTLCompiler.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include <iostream>
#include <string>
#include <vector>
#include <stdexcept>

// Helper to check if a string contains another string.
bool contains(const std::string& haystack, const std::string& needle) {
    return haystack.find(needle) != std::string::npos;
}

int main() {
    std::string entry_file = "Test/AutoIdClassTest/test.chtl";
    std::cout << "--- Running Auto ID/Class Test: " << entry_file << " ---\n" << std::endl;

    try {
        CHTL::CHTLCompiler compiler;
        std::unique_ptr<CHTL::BaseNode> ast = compiler.compile(entry_file);

        CHTL::CHTLGenerator generator(compiler.getTemplateDefinitions());
        CHTL::CompilationResult result = generator.generate(ast.get());

        std::cout << "--- Generated HTML ---\n" << result.html << "\n----------------------\n" << std::endl;

        // --- Verification ---
        std::vector<std::string> expected_attributes = {
            "class=\"box\"",
            "id=\"main-content\"",
            "class=\"existing-class new-class\"",
            "class=\"first-class second-class\"",
            "id=\"new-id\""
        };

        bool all_passed = true;
        for (const auto& attr : expected_attributes) {
            if (!contains(result.html, attr)) {
                std::cerr << "TEST FAILED: Could not find attribute fragment: " << attr << std::endl;
                all_passed = false;
            }
        }

        // Also check that the original ID was replaced, not duplicated.
        if (contains(result.html, "id=\"original-id\"")) {
             std::cerr << "TEST FAILED: Found original ID which should have been replaced." << std::endl;
             all_passed = false;
        }

        if (all_passed) {
            std::cout << "\n--- Auto ID/Class Test PASSED ---" << std::endl;
        } else {
            std::cerr << "\n--- Auto ID/Class Test FAILED ---\n" << std::endl;
            return 1;
        }

    } catch (const std::exception& e) {
        std::cerr << "Caught an exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
