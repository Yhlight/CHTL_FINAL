#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>

// Helper function to check if a string contains a substring
bool contains(const std::string& str, const std::string& substr) {
    return str.find(substr) != std::string::npos;
}

int main() {
    std::string file_path = "Test/ResponsiveTest/main.chtl";
    std::string source_code = CHTL::FileSystem::readFile(file_path);

    auto config = std::make_shared<CHTL::Configuration>();
    CHTL::CHTLLexer lexer(source_code, config);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();

    CHTL::CHTLParser parser(source_code, tokens, file_path, config);
    auto root = parser.parse();
    bool use_doctype = parser.getUseHtml5Doctype();

    CHTL::CHTLGenerator generator(parser.getTemplateDefinitions(), config);
    CHTL::CompilationResult result = generator.generate(root.get(), use_doctype);

    std::cout << "--- Generated JS ---\n" << result.js << "\n------------------\n";

    // 1. Assert that the original script content is there
    assert(contains(result.js, "var myClass = \"initial-class\";"));

    // 2. Assert that the reactivity manager is generated
    assert(contains(result.js, "const __chtl_reactivity_manager = {"));

    // 3. Assert that the binding is created correctly.
    // The generated ID will be __chtl_reactive_id_0
    assert(contains(result.js, "__chtl_reactivity_manager.createReactive(window, 'myClass', (newValue) => { document.getElementById('__chtl_reactive_id_0').className = newValue; });"));

    std::cout << "ResponsiveTest PASSED!" << std::endl;

    return 0;
}
