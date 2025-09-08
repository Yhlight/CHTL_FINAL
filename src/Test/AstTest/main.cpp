#include "../../CHTL/CHTLLexer/Lexer.h"
#include "../../CHTL/CHTLParser/Parser.h"
#include "../../CHTL/CHTLGenerator/Generator.h"
#include <iostream>

// The printAST function remains the same for debugging purposes
void printAST(const BaseNode* node, int indent = 0) {
    if (!node) {
        return;
    }

    std::string indentation(indent * 2, ' ');

    switch (node->getType()) {
        case NodeType::Element: {
            const auto* element = static_cast<const ElementNode*>(node);
            std::cout << indentation << "<" << element->tagName << ">" << std::endl;
            if (!element->attributes.empty()) {
                std::cout << indentation << "  Attributes:" << std::endl;
                for (const auto& attr : element->attributes) {
                    std::cout << indentation << "    " << attr.key << ": \"" << attr.value << "\"" << std::endl;
                }
            }
            if (!element->children.empty()) {
                std::cout << indentation << "  Children:" << std::endl;
                for (const auto& child : element->children) {
                    printAST(child.get(), indent + 2);
                }
            }
            break;
        }
        case NodeType::Text: {
            const auto* text = static_cast<const TextNode*>(node);
            std::cout << indentation << "Text: \"" << text->content << "\"" << std::endl;
            break;
        }
        case NodeType::Comment: {
            const auto* comment = static_cast<const CommentNode*>(node);
            std::cout << indentation << "Comment: \"" << comment->content << "\"" << std::endl;
            break;
        }
        default:
            std::cout << indentation << "Unknown Node" << std::endl;
            break;
    }
}

int main() {
    std::string chtl_source = R"(
// Main document structure
html {
    body {
        // A div with attributes and content
        div {
            id: main-content;
            class = "container";

            text {
                Welcome to CHTL
            }

            span {
                text { "A nested element." }
            }
        }
    }
}
)";

    // 1. Lexing
    Lexer lexer(chtl_source);

    // 2. Parsing
    Parser parser(lexer);
    NodeList ast = parser.parse();

    // Print the AST for verification
    std::cout << "--- Abstract Syntax Tree ---" << std::endl;
    for (const auto& node : ast) {
        printAST(node.get());
    }
    std::cout << "--------------------------" << std::endl << std::endl;

    // 3. Generation
    Generator generator;
    std::string html_output = generator.generate(ast);

    // Print the final HTML
    std::cout << "--- Generated HTML ---" << std::endl;
    std::cout << html_output << std::endl;
    std::cout << "----------------------" << std::endl;


    return 0;
}
