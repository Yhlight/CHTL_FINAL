#include "Parser.h"
#include "Lexer.h"
#include "Node.h"
#include "ElementNode.h"
#include "TextNode.h"

#include <iostream>
#include <cassert>

void runParserTest() {
    std::cout << "Running Parser Test..." << std::endl;

    std::string input = R"(
        html {
            body {
                text { "Hello" }
            }
        }
    )";

    CHTL::Lexer lexer(input);
    CHTL::Parser parser(lexer);

    auto program = parser.ParseProgram();

    // Check for parsing errors
    const auto& errors = parser.Errors();
    if (!errors.empty()) {
        std::cerr << "Parser has " << errors.size() << " errors:" << std::endl;
        for (const auto& msg : errors) {
            std::cerr << "\t" << msg << std::endl;
        }
        assert(false);
    }
    assert(program != nullptr);

    // Check program structure
    assert(program->statements.size() == 1);

    // Check html node
    auto* htmlNode = dynamic_cast<CHTL::ElementNode*>(program->statements[0].get());
    assert(htmlNode != nullptr);
    assert(htmlNode->tagName == "html");
    assert(htmlNode->children.size() == 1);

    // Check body node
    auto* bodyNode = dynamic_cast<CHTL::ElementNode*>(htmlNode->children[0].get());
    assert(bodyNode != nullptr);
    assert(bodyNode->tagName == "body");
    assert(bodyNode->children.size() == 1);

    // Check that the child of body is a TextNode
    auto* textNode = dynamic_cast<CHTL::TextNode*>(bodyNode->children[0].get());
    assert(textNode != nullptr);
    assert(textNode->value == "Hello");


    std::cout << "Parser Test Passed!" << std::endl;
}


int main() {
    runParserTest();
    return 0;
}
