#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <regex>
#include <map>

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

// Helper to parse and compare CSS rulesets
bool compare_css(const std::string& generated_css, const std::map<std::string, std::string>& expected_rules) {
    std::string processed_css = remove_whitespace(generated_css);

    for (const auto& pair : expected_rules) {
        std::string rule_str = remove_whitespace(pair.first + ":" + pair.second + ";");
        if (processed_css.find(rule_str) == std::string::npos) {
            std::cerr << "FAILED to find rule: " << rule_str << " in generated CSS." << std::endl;
            return false;
        }
    }
    return true;
}


int main() {
    std::string file_path = "Test/ArithmeticTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    auto config = std::make_shared<CHTL::Configuration>();

    CHTL::CHTLLexer lexer(source, config);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();

    CHTL::CHTLParser parser(source, tokens, file_path, config);
    auto root = parser.parse();

    CHTL::CHTLGenerator generator(parser.getTemplateDefinitions(), config);
    CHTL::CompilationResult result = generator.generate(root.get(), false);

    // The generator will create a style attribute. We need to parse it.
    std::regex style_regex("style=\"([^\"]+)\"");
    std::smatch match;
    if (!std::regex_search(result.html, match, style_regex)) {
        std::cerr << "ArithmeticTest FAILED! Could not find style attribute in HTML." << std::endl;
        return 1;
    }

    std::string style_content = match[1].str();

    std::map<std::string, std::string> expected_styles;
    expected_styles["width"] = "150px";
    expected_styles["height"] = "175px";
    expected_styles["padding"] = "20";
    expected_styles["margin"] = "25";
    expected_styles["font-size"] = "16pt";
    expected_styles["border-width"] = "20px";
    expected_styles["line-height"] = "1.5";
    expected_styles["opacity"] = "1";
    expected_styles["z-index"] = "1024";

    if (!compare_css(style_content, expected_styles)) {
         std::cerr << "ArithmeticTest FAILED!" << std::endl;
         std::cerr << "Generated style content was: " << style_content << std::endl;
         return 1;
    }

    std::cout << "ArithmeticTest PASSED!" << std::endl;

    return 0;
}
