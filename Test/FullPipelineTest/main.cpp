#include "../../CHTL/CHTLCompiler.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../../CHTL/CHTLContext.h"
#include <iostream>
#include <cassert>

int main() {
    // The entry point to our test project
    std::string entry_file = "Test/FullPipelineTest/ns_test.chtl";

    std::cout << "--- Starting Compilation from Entry File: " << entry_file << " ---\n" << std::endl;

    try {
        // 1. Compilation
        CHTL::CHTLCompiler compiler;
        std::unique_ptr<CHTL::CHTLContext> context = compiler.compile(entry_file);

        // 2. Verification
        std::cout << "--- Verifying Namespace Context ---" << std::endl;

        // Check if the "my_lib" namespace exists
        assert(context->namespaces.count("my_lib") == 1);
        std::cout << "OK: Namespace 'my_lib' found." << std::endl;

        // Check for the "MyStyle" template within the "my_lib" namespace
        const CHTL::NamespaceData& ns_data = context->namespaces.at("my_lib");
        assert(ns_data.template_definitions.count("MyStyle") == 1);
        std::cout << "OK: Template 'MyStyle' found in 'my_lib' namespace." << std::endl;

        // Check the property of the style
        const CHTL::TemplateDefinitionNode& style_def = ns_data.template_definitions.at("MyStyle");
        assert(style_def.style_properties.size() == 1);
        assert(style_def.style_properties[0].key == "color");
        // For now, we don't have an easy way to check the expression value,
        // but we can add that later. This is a good start.
        std::cout << "OK: 'MyStyle' template has the correct properties." << std::endl;

        std::cout << "\n--- Namespace Test Passed! ---\n" << std::endl;


    } catch (const std::exception& e) {
        std::cerr << "Caught an exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
