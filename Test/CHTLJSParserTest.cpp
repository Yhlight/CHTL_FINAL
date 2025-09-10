#include "../CHTL_JS/CHTLJSLexer/CHTLJSLexer.h"
#include "../CHTL_JS/CHTLJSParser/CHTLJSParser.h"
#include "../CHTL_JS/CHTLJSNode/CHTLJSNode.h"
#include <iostream>
#include <vector>

// Forward declaration
void printChtljsAst(const CHTLJSNodePtr& node, int indent = 0);

void printChtljsNodeList(const std::vector<CHTLJSNodePtr>& nodes, int indent) {
    for (const auto& node : nodes) {
        printChtljsAst(node, indent);
    }
}

void printChtljsAst(const CHTLJSNodePtr& node, int indent) {
    if (!node) return;
    std::string indentation(indent * 2, ' ');

    if (auto* selector = dynamic_cast<SelectorNode*>(node.get())) {
        std::cout << indentation << "Selector: " << selector->selector_text << std::endl;
    } else if (auto* listen = dynamic_cast<ListenNode*>(node.get())) {
        std::cout << indentation << "ListenStatement:" << std::endl;
        std::cout << indentation << "  Target:" << std::endl;
        printChtljsAst(listen->target, indent + 2);
        std::cout << indentation << "  Body: \"" << listen->body << "...\"" << std::endl;
    } else {
        std::cout << indentation << "Unknown CHTLJS Node" << std::endl;
    }
}

int main() {
    std::string source = R"(
        {{.my-button}}->listen {
            click: someClickHandler,
            mouseenter: someFunction
        }
    )";

    std::cout << "--- Starting CHTL JS Parser Test ---" << std::endl;

    try {
        CHTLJSLexer lexer(source);
        CHTLJSParser parser(lexer);
        auto ast = parser.parse();

        std::cout << "--- CHTL JS Abstract Syntax Tree ---" << std::endl;
        printChtljsNodeList(ast, 0);

    } catch (const std::runtime_error& e) {
        std::cerr << "Caught a runtime error: " << e.what() << std::endl;
        return 1;
    }

    std::cout << "\n--- CHTL JS Parser Test Finished ---" << std::endl;
    return 0;
}
