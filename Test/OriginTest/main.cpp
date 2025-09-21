#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>
#include <stdexcept>

// Helper to remove all whitespace from a string
void removeWhitespace(std::string& str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
}

int main() {
    std::string entry_point = "Test/OriginTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(entry_point);

    std::cout << "--- Input CHTL from " << entry_point << " ---\n" << source << "\n------------------\n" << std::endl;

    try {
        auto config = std::make_shared<CHTL::Configuration>();
        CHTL::CHTLLexer lexer(source, config);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();

        CHTL::CHTLParser parser(source, tokens, entry_point, config);
        std::unique_ptr<CHTL::BaseNode> ast = parser.parse();

        CHTL::CHTLGenerator generator(parser.getTemplateDefinitions(), config);
        CHTL::CompilationResult result = generator.generate(ast.get(), parser.getUseHtml5Doctype());

        std::cout << "--- Generated HTML ---\n" << result.html << "\n----------------------\n" << std::endl;
        std::cout << "--- Generated CSS ---\n" << result.css << "\n---------------------\n" << std::endl;
        std::cout << "--- Generated JS ---\n" << result.js << "\n--------------------\n" << std::endl;

        std::string expected_html = "<html><head></head><body><div>Content</div><p>This is some raw HTML.</p><div id=\"app\">{{ message }}</div></body></html>";
        std::string expected_css = ".raw-css { color: purple; }";
        std::string expected_js = "function raw_js() { if (true) { console.log(\"raw js with braces\"); } }";

        std::string actual_html = result.html;
        std::string actual_css = result.css;
        std::string actual_js = result.js;

        removeWhitespace(expected_html);
        removeWhitespace(expected_css);
        removeWhitespace(expected_js);
        removeWhitespace(actual_html);
        removeWhitespace(actual_css);
        removeWhitespace(actual_js);

        assert(actual_html == expected_html);
        assert(actual_css == expected_css);
        assert(actual_js == expected_js);

        std::cout << "OriginTest Passed!" << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime_error exception in OriginTest: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
