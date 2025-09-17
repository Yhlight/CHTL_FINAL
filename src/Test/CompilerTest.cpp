#include "../CHTL/Lexer/Lexer.h"
#include "../CHTL/Parser/Parser.h"
#include "../CHTL/Generator/Generator.h"
#include "AstPrinter.h"
#include <iostream>
#include <vector>
#include <cassert>
#include <algorithm>

void testCustomDefinitionAndExpansion() {
    std::cout << "--- Testing [Custom] Definition and Expansion ---\n";
    std::string source = R"(
        [Custom] @Style BaseCustomStyle
        {
            color: blue;
            font-weight: bold;
            font-size: 16px;
        }

        div {
            style {
                @Style BaseCustomStyle {
                    delete font-size;
                }
            }
        }
    )";

    CHTL::Lexer lexer(source);
    std::vector<CHTL::Token> tokens = lexer.getAllTokens();
    CHTL::Parser parser(tokens);
    std::shared_ptr<CHTL::RootNode> ast = parser.parse();

    // Test Parser
    auto customNode = std::dynamic_pointer_cast<CHTL::CustomNode>(ast->children[0]);
    assert(customNode != nullptr);
    assert(customNode->name == "BaseCustomStyle");
    auto styleNode = std::dynamic_pointer_cast<CHTL::StyleNode>(customNode->body[0]);
    assert(styleNode != nullptr);
    assert(styleNode->children.size() == 3);
    std::cout << "Parser correctly parsed [Custom] definition.\n";

    // Test Generator
    CHTL::Generator generator(ast);
    generator.generate();
    std::string html = generator.getHtml();

    std::cout << "Generated HTML:\n" << html << std::endl;

    std::string clean_html;
    clean_html.resize(html.size());
    auto it = std::remove_copy_if(html.begin(), html.end(), clean_html.begin(), ::isspace);
    clean_html.erase(it, clean_html.end());

    assert(clean_html.find("font-weight:bold;") != std::string::npos);
    assert(clean_html.find("color:blue;") != std::string::npos);
    assert(clean_html.find("font-size:16px;") == std::string::npos);

    std::cout << "[Custom] expansion test passed.\n";
    std::cout << "--------------------------------\n\n";
}


int main() {
    testCustomDefinitionAndExpansion();
    return 0;
}
