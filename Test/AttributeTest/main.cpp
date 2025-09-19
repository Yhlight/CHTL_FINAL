#include "../../CHTL/Scanner/CHTLUnifiedScanner.h"
#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <string>
#include <cassert>

// Helper to check if a substring exists in a string
bool contains(const std::string& haystack, const std::string& needle) {
    return haystack.find(needle) != std::string::npos;
}

int main() {
    std::string entry_point = "Test/AttributeTest/main.chtl";
    std::string source;
    try {
        source = CHTL::FileSystem::readFile(entry_point);
    } catch (const std::runtime_error& e) {
        std::cerr << "Failed to read test file: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "--- Input CHTL ---\n" << source << "\n------------------\n" << std::endl;

    try {
        CHTL::CHTLUnifiedScanner scanner(source);
        CHTL::ScanResult scan_result = scanner.scan();

        CHTL::CHTLLexer lexer(scan_result.sanitized_source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();
        CHTL::CHTLParser parser(scan_result.sanitized_source, tokens, entry_point);
        std::unique_ptr<CHTL::BaseNode> ast = parser.parse();
        CHTL::CHTLGenerator generator(parser.getTemplateDefinitions(), scan_result.placeholders);
        CHTL::CompilationResult result = generator.generate(ast.get());

        std::cout << "--- Generated HTML ---\n" << result.html << "\n----------------------\n" << std::endl;

        // --- Verification ---
        std::cout << "--- Running Asserts ---\n";

        assert(contains(result.html, "id=\"box-1\""));
        std::cout << "Assert 1: ID attribute with concatenation correct. PASSED.\n";

        assert(contains(result.html, "class=\"item item-5\""));
        std::cout << "Assert 2: Class attribute with nested expressions correct. PASSED.\n";

        assert(contains(result.html, "dataActive=\"true\""));
        std::cout << "Assert 3: Data attribute with conditional correct. PASSED.\n";

        assert(contains(result.html, "dataValue=\"250\""));
        std::cout << "Assert 4: Data attribute with arithmetic correct. PASSED.\n";

        assert(contains(result.html, "dataText=\"Final value is 250\""));
        assert(contains(result.html, "></div>"));
        std::cout << "Assert 5: Data-text attribute with expression correct. PASSED.\n";

        std::cout << "\n--- All Asserts Passed! ---\n";

    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime_error exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
