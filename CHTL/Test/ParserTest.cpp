#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include <iostream>

// Forward declaration
void printAst(const std::shared_ptr<Node>& node, int indent = 0);

void printIndent(int indent) {
    for (int i = 0; i < indent; ++i) {
        std::cout << "  ";
    }
}

void printElementNode(const std::shared_ptr<ElementNode>& node, int indent) {
    printIndent(indent);
    std::cout << "<" << node->tagName << ">" << std::endl;

    for (const auto& attr : node->attributes) {
        printIndent(indent + 1);
        std::cout << "@attr " << attr->name << " = \"" << attr->value << "\"" << std::endl;
    }

    for (const auto& child : node->children) {
        printAst(child, indent + 1);
    }
}

void printTextNode(const std::shared_ptr<TextNode>& node, int indent) {
    printIndent(indent);
    std::cout << "TEXT: \"" << node->content << "\"" << std::endl;
}

void printAst(const std::shared_ptr<Node>& node, int indent) {
    if (!node) return;

    if (auto element = std::dynamic_pointer_cast<ElementNode>(node)) {
        printElementNode(element, indent);
    } else if (auto text = std::dynamic_pointer_cast<TextNode>(node)) {
        printTextNode(text, indent);
    } else if (auto root = std::dynamic_pointer_cast<RootNode>(node)) {
        printIndent(indent);
        std::cout << "[ROOT]" << std::endl;
        for (const auto& stmt : root->statements) {
            printAst(stmt, indent + 1);
        }
    } else {
        printIndent(indent);
        std::cout << "Unknown Node Type" << std::endl;
    }
}


int main() {
    const char* sample_code = R"CHTL(
html {
    head {
        title {
            text: "My CHTL Page";
        }
    }
    body {
        div {
            id = "main";
            class: "container";

            p {
                text {
                    "Welcome to CHTL!"
                }
            }
        }
    }
}
)CHTL";

    std::cout << "--- Parsing Sample CHTL Code ---" << std::endl;

    try {
        Lexer lexer(sample_code);
        Parser parser(lexer);
        std::shared_ptr<RootNode> ast = parser.parse();

        std::cout << "\n--- Abstract Syntax Tree ---" << std::endl;
        printAst(ast);
        std::cout << "--------------------------" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Caught exception: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
