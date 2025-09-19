#include "../../CHTL/CHTLLexer/CHTLLexer.h"
#include "../../CHTL/CHTLParser/CHTLParser.h"
#include "../../CHTL/CHTLGenerator/CHTLGenerator.h"
#include "../../Util/FileSystem/FileSystem.h"
#include <iostream>
#include <cassert>
#include <string>
#include <algorithm>
#include <regex>

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

// Helper function to sort attributes for comparison
std::string sort_attributes(std::string html) {
    std::regex tag_regex("<([a-zA-Z0-9]+)\\s+([^>]+)>");
    std::smatch match;
    if (std::regex_search(html, match, tag_regex)) {
        std::string tag_name = match[1];
        std::string attrs_str = match[2];

        std::regex attr_regex("([a-zA-Z\\-]+)=\"([^\"]+)\"");
        std::sregex_iterator it(attrs_str.begin(), attrs_str.end(), attr_regex);
        std::sregex_iterator end;

        std::map<std::string, std::string> sorted_attrs;
        while (it != end) {
            sorted_attrs[(*it)[1]] = (*it)[2];
            ++it;
        }

        std::string new_attrs;
        for (const auto& pair : sorted_attrs) {
            new_attrs += pair.first + "=\"" + pair.second + "\" ";
        }
        if (!new_attrs.empty()) {
            new_attrs.pop_back();
        }

        html.replace(match.position(2), match.length(2), new_attrs);
    }
    return html;
}


int main() {
    std::string file_path = "Test/ColonEqualTest/main.chtl";
    std::string source = CHTL::FileSystem::readFile(file_path);

    auto config = std::make_shared<CHTL::Configuration>();

    CHTL::CHTLLexer lexer(source, config);
    std::vector<CHTL::Token> tokens = lexer.scanTokens();

    CHTL::CHTLParser parser(source, tokens, file_path, config);
    auto root = parser.parse();

    CHTL::CHTLGenerator generator(parser.getTemplateDefinitions(), config);
    CHTL::CompilationResult result = generator.generate(root.get(), false);

    std::string expected_html = R"(<div id="main-box" class="sub" style="width:100px;">Hello</div>)";
    std::string expected_css = R"(.sub{color:blue;})";

    std::string processed_html_result = remove_whitespace(sort_attributes(result.html));
    std::string processed_html_expected = remove_whitespace(sort_attributes(expected_html));

    if (processed_html_result != processed_html_expected) {
        std::cerr << "ColonEqualTest FAILED on HTML!" << std::endl;
        std::cerr << "Expected: " << processed_html_expected << std::endl;
        std::cerr << "Got:      " << processed_html_result << std::endl;
        return 1;
    }

    std::string processed_css_result = remove_whitespace(result.css);
    std::string processed_css_expected = remove_whitespace(expected_css);

    if (processed_css_result != processed_css_expected) {
        std::cerr << "ColonEqualTest FAILED on CSS!" << std::endl;
        std::cerr << "Expected: " << processed_css_expected << std::endl;
        std::cerr << "Got:      " << processed_css_result << std::endl;
        return 1;
    }

    assert(result.js.empty());

    std::cout << "ColonEqualTest PASSED!" << std::endl;

    return 0;
}
