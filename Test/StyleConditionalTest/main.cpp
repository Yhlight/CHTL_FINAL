#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>
#include <sstream>
#include <map>

// Helper to parse style string into a map for easy comparison
std::map<std::string, std::string> parseStyle(const std::string& style) {
    std::map<std::string, std::string> style_map;
    std::stringstream ss(style);
    std::string property;
    while (std::getline(ss, property, ';')) {
        if (property.empty()) continue;
        size_t colon_pos = property.find(':');
        if (colon_pos != std::string::npos) {
            std::string key = property.substr(0, colon_pos);
            std::string value = property.substr(colon_pos + 1);
            style_map[key] = value;
        }
    }
    return style_map;
}

int main() {
    std::string entry_point = "Test/StyleConditionalTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(entry_point);

    std::cout << "--- Input CHTL ---\n" << source << "\n------------------\n" << std::endl;

    std::string generated_html;

    try {
        CHTL::CHTLLexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();
        CHTL::CHTLParser parser(source, tokens, entry_point);
        std::unique_ptr<CHTL::BaseNode> ast = parser.parse();
        CHTL::CHTLGenerator generator(parser.getTemplateDefinitions());
        CHTL::CompilationResult result = generator.generate(ast.get());

        generated_html = result.html;

        std::cout << "--- Generated HTML ---\n" << generated_html << "\n----------------------\n" << std::endl;

        // --- Test Blue Box ---
        size_t blue_box_pos = generated_html.find("id=\"box-blue\"");
        assert(blue_box_pos != std::string::npos);
        size_t blue_style_start = generated_html.find("style=\"", blue_box_pos) + 7;
        size_t blue_style_end = generated_html.find("\"", blue_style_start);
        std::string blue_style_attr = generated_html.substr(blue_style_start, blue_style_end - blue_style_start);
        std::map<std::string, std::string> blue_styles = parseStyle(blue_style_attr);
        assert(blue_styles.count("background-color"));
        assert(blue_styles["background-color"] == "blue");

        // --- Test Red Box ---
        size_t red_box_pos = generated_html.find("id=\"box-red\"");
        assert(red_box_pos != std::string::npos);
        size_t red_style_start = generated_html.find("style=\"", red_box_pos) + 7;
        size_t red_style_end = generated_html.find("\"", red_style_start);
        std::string red_style_attr = generated_html.substr(red_style_start, red_style_end - red_style_start);
        std::map<std::string, std::string> red_styles = parseStyle(red_style_attr);
        assert(red_styles.count("background-color"));
        assert(red_styles["background-color"] == "red");

    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime_error exception: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Conditional Style Test Passed!" << std::endl;

    return 0;
}
