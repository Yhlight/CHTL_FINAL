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
    std::string entry_point = "Test/StylePropertyReferenceTest/main.chtl";
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

        // Find the span's style attribute
        size_t span_pos = generated_html.find("<span");
        assert(span_pos != std::string::npos);
        size_t style_start = generated_html.find("style=\"", span_pos) + 7;
        size_t style_end = generated_html.find("\"", style_start);
        std::string style_attr = generated_html.substr(style_start, style_end - style_start);

        std::map<std::string, std::string> generated_styles = parseStyle(style_attr);

        assert(generated_styles.count("width"));
        // Using stof for float comparison, expecting 200px
        assert(std::stof(generated_styles.at("width")) == 200.0f);

    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime_error exception: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "Property Reference Test Passed!" << std::endl;

    return 0;
}
