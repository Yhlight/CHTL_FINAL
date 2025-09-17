#include "../CHTL/Lexer/Lexer.h"
#include "../CHTL/Parser/Parser.h"
#include "../CHTL/Generator/Generator.h"
#include "AstPrinter.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>

// Helper to remove whitespace for easier comparison
std::string removeWhitespace(const std::string& str) {
    std::string out;
    out.reserve(str.size());
    for(char c : str) {
        if (!std::isspace(c)) {
            out += c;
        }
    }
    return out;
}

void runTest(const std::string& testName, const std::string& source, const std::string& expected_html) {
    std::cout << "--- Testing: " << testName << " ---\n";
    CHTL::TemplateRegistry::getInstance().clear();

    CHTL::Lexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.getAllTokens();

    CHTL::Parser parser(tokens);
    std::shared_ptr<CHTL::RootNode> ast = parser.parse();
    CHTL::Generator generator(ast);
    generator.generate();
    std::string html = generator.getHtml();

    std::cout << "Generated HTML:\n" << html << std::endl;

    std::string clean_html = removeWhitespace(html);
    std::string clean_expected = removeWhitespace(expected_html);

    assert(clean_html == clean_expected);

    std::cout << "Test passed.\n";
    std::cout << "--------------------------------\n\n";
}

int main() {
    runTest("Insert Before Indexed", R"(
        [Template] @Element Card {
            div { class: "header"; }
            div { class: "footer"; }
        }
        body { @Element Card { insert before div[1] { p{} } } }
    )", "<body><divclass=\"header\"></div><p></p><divclass=\"footer\"></div></body>");

    return 0;
}
