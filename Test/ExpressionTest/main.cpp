#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>

// Helper to remove all whitespace from a string
void removeWhitespace(std::string& str) {
    str.erase(std::remove_if(str.begin(), str.end(), ::isspace), str.end());
}

int main() {
    std::string source =
        "[Template] @Style Base { width: 100px; height: 200px; }\n"
        "body {\n"
        "    div { \n"
        "        id: \"container\";\n"
        "        style { width: 800px; }\n"
        "        p { \n"
        "            class: \"item\";\n"
        "            style { height: 50px; }\n"
        "        }\n"
        "        p { \n"
        "            class: \"item\";\n"
        "            style { height: #container.width / 8; } // 100px\n"
        "        }\n"
        "    }\n"
        "    span {\n"
        "       style {\n"
        "           width: .item[1].height + 10px; // 110px\n"
        "           height: 10 > 5 ? 20px : 30px; // 20px\n"
        "       }\n"
        "    }\n"
        "    a {\n"
        "       style {\n"
        "           @Style Base;\n"
        "       }\n"
        "    }\n"
        "    button {\n"
        "        style { width: a.width; } // 100px\n"
        "    }\n"
        "}";

    std::cout << "--- Input CHTL ---\n" << source << "\n------------------\n" << std::endl;

    try {
        CHTL::CHTLLexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();
        CHTL::CHTLParser parser(source, tokens, "test.chtl");
        std::unique_ptr<CHTL::BaseNode> ast = parser.parse();
        CHTL::CHTLGenerator generator(parser.getTemplateDefinitions());
        CHTL::CompilationResult result = generator.generate(ast.get());

        std::cout << "--- Generated HTML ---\n" << result.html << "\n----------------------\n" << std::endl;

        std::string expected_html =
            "<body>"
            "<div id=\"container\" style=\"width:800px;\">"
            "<p class=\"item\" style=\"height:50px;\"></p>"
            "<p class=\"item\" style=\"height:100px;\"></p>"
            "</div>"
            "<span style=\"height:20px;width:110px;\"></span>"
            "<a style=\"height:200px;width:100px;\"></a>"
            "<button style=\"width:100px;\"></button>"
            "</body>";

        std::string actual_html = result.html;

        removeWhitespace(expected_html);
        removeWhitespace(actual_html);

        assert(actual_html == expected_html);

        std::cout << "Expression Test Passed!" << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime_error exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
