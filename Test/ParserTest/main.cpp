#include "../../CHTL/CHTLLexer/Lexer.h"
#include "../../CHTL/CHTLParser/Parser.h"
#include "../../CHTL/CHTLNode/AST.h"
#include <iostream>
#include <vector>
#include <memory>

void printAST(const CHTL::BaseNode* node, int indent = 0);

void printElementNode(const CHTL::ElementNode* node, int indent) {
    std::cout << std::string(indent, ' ') << "<" << node->tagName << ">" << std::endl;
    if (!node->attributes.empty()) {
        std::cout << std::string(indent + 2, ' ') << "Attributes:" << std::endl;
        for (const auto& attr : node->attributes) {
            std::cout << std::string(indent + 4, ' ') << attr.key << ": \"" << attr.value << "\"" << std::endl;
        }
    }
    if (!node->children.empty()) {
        std::cout << std::string(indent + 2, ' ') << "Children:" << std::endl;
        for (const auto& child : node->children) {
            printAST(child.get(), indent + 4);
        }
    }
}

void printTextNode(const CHTL::TextNode* node, int indent) {
    std::cout << std::string(indent, ' ') << "Text: \"" << node->content << "\"" << std::endl;
}

void printAST(const CHTL::BaseNode* node, int indent) {
    if (auto elementNode = dynamic_cast<const CHTL::ElementNode*>(node)) {
        printElementNode(elementNode, indent);
    } else if (auto textNode = dynamic_cast<const CHTL::TextNode*>(node)) {
        printTextNode(textNode, indent);
    } else {
        std::cout << std::string(indent, ' ') << "Unknown Node" << std::endl;
    }
}

void runParserTest(const std::string& source) {
    std::cout << "--- Parsing Source ---" << std::endl;
    std::cout << source << std::endl;
    std::cout << "----------------------" << std::endl;

    try {
        CHTL::Lexer lexer(source);
        auto tokens = lexer.tokenize();
        CHTL::Parser parser(tokens);
        CHTL::DocumentNode ast = parser.parse();

        std::cout << "--- AST ---" << std::endl;
        for (const auto& node : ast) {
            printAST(node.get());
        }
        std::cout << "-----------" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
    }
    std::cout << "\n\n";
}

int main() {
    std::string sample = R"(
        div {
            id: "main";
            class = container;

            h1 {
                text { "Welcome to CHTL" }
            }

            [Template] // This should be skipped gracefully

            p {
                // some text here
            }
        }
    )";

    runParserTest(sample);

    return 0;
}
