#include "CHTLLexer.h"
#include "CHTLParser.h"
#include "ElementNode.h"
#include "TextNode.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

// --- AST Printing for Debugging ---
void printAst(const std::shared_ptr<CHTL::BaseNode>& node, int indent = 0) {
    if (auto element = std::dynamic_pointer_cast<CHTL::ElementNode>(node)) {
        for (int i = 0; i < indent; ++i) std::cout << "  ";
        std::cout << "<" << element->tagName << ">" << std::endl;

        // Print attributes
        for (const auto& attr : element->attributes) {
            for (int i = 0; i < indent + 1; ++i) std::cout << "  ";
            std::cout << "(ATTR) " << attr->key << " = \"" << attr->value << "\"" << std::endl;
        }

        // Print global styles (only for the root node)
        if (element->tagName == "root" && !element->global_styles.empty()) {
            for (int i = 0; i < indent + 1; ++i) std::cout << "  ";
            std::cout << "(GLOBAL_STYLES COLLECTED)" << std::endl;
            for (const auto& style_pair : element->global_styles) {
                for (int i = 0; i < indent + 2; ++i) std::cout << "  ";
                std::cout << style_pair.first << " { " << style_pair.second << "}" << std::endl;
            }
        }

        // Print children
        for (const auto& child : element->children) {
            printAst(child, indent + 1);
        }
    } else if (auto text = std::dynamic_pointer_cast<CHTL::TextNode>(node)) {
        for (int i = 0; i < indent; ++i) std::cout << "  ";
        std::cout << "(TEXT) \"" << text->content << "\"" << std::endl;
    }
}

// --- Main Driver ---
int main() {
    std::string source = R"(
        div {
            class: "box";
            style {
                &:hover {
                    background-color: #eee;
                }
            }
        }
    )";

    std::cout << "--- CHTL Source ---" << std::endl;
    std::cout << source << std::endl << std::endl;

    std::shared_ptr<CHTL::ElementNode> ast;

    try {
        CHTL::CHTLLexer lexer(source);
        std::vector<CHTL::Token> tokens = lexer.scanTokens();
        CHTL::CHTLParser parser(tokens);
        ast = parser.parse();
    } catch (const std::exception& e) {
        std::cerr << "Compilation failed: " << e.what() << std::endl;
        return 1;
    }

    // Print the AST to verify the parser's work
    std::cout << "--- Parsed AST ---" << std::endl;
    printAst(ast);
    std::cout << "------------------" << std::endl;

    return 0;
}
