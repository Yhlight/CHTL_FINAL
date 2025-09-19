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

// Helper function to remove all whitespace from a string for robust comparison
std::string remove_whitespace(const std::string& str) {
    std::string out;
    out.reserve(str.size());
    for (char c : str) {
        if (!std::isspace(static_cast<unsigned char>(c))) {
            out += c;
        }
    }
    return out;
}

int main() {
    std::string file_path = "Test/ImportTest/main.chtl";
    std::string source_code = CHTL::FileSystem::readFile(file_path);

    auto config = std::make_shared<CHTL::Configuration>();
    CHTL::CHTLLexer lexer(source_code, config);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();

    CHTL::CHTLParser parser(source_code, tokens, file_path, config);
    auto root = parser.parse();
    bool use_doctype = parser.getUseHtml5Doctype();

    CHTL::CHTLGenerator generator(parser.getTemplateDefinitions(), config);
    CHTL::CompilationResult result = generator.generate(root.get(), use_doctype);

    std::string processed_html = remove_whitespace(result.html);
    std::cout << "--- Generated HTML ---\n" << result.html << "\n----------------------\n";

    // 1. Test aliased import: @Element LibButton
    assert(contains(processed_html, "<buttonstyle=\"border-radius:5px;padding:10px;\">DefaultButton</button>"));

    // 2. Test precise import: @Style Important
    assert(contains(processed_html, "<pstyle=\"color:red;font-weight:bold;\">Importanttext!</p>"));

    // 3. Test type import: @Style Base
    assert(contains(processed_html, "<spanstyle=\"font-family:Helvetica;\">Basetext.</span>"));

    // 4. Test that un-imported symbols are not available.
    assert(!contains(processed_html, "class=\"card\""));

    std::cout << "ImportTest PASSED!" << std::endl;

    return 0;
}
