#include "../CHTL/CHTLLexer/Lexer.h"
#include "../CHTL/CHTLParser/Parser.h"
#include "../CHTL/CHTLNode/BaseNode.h"
#include "../CHTL/CHTLNode/ElementNode.h"
#include "../CHTL/CHTLNode/TextNode.h"
#include "../CHTL/CHTLNode/AttributeNode.h"
#include "../CHTL/CHTLNode/RootNode.h"
#include <iostream>
#include <string>

// An implementation of the AstVisitor to print the tree
class AstPrinter : public AstVisitor {
public:
    std::string print(std::shared_ptr<RootNode> root) {
        root->accept(*this);
        return result;
    }

    void visit(RootNode& node) override {
        result += "(Document\n";
        indent++;
        for (auto& child : node.children) {
            child->accept(*this);
        }
        indent--;
        result += ")\n";
    }

    void visit(ElementNode& node) override {
        result += std::string(indent * 2, ' ');
        result += "(Element<" + node.tagName + ">\n";
        indent++;

        // Print attributes
        for (auto& attr : node.attributes) {
            attr->accept(*this);
        }

        // Print children
        for (auto& child : node.children) {
            child->accept(*this);
        }

        indent--;
        result += std::string(indent * 2, ' ');
        result += ")\n";
    }

    void visit(AttributeNode& node) override {
        result += std::string(indent * 2, ' ');
        result += "(Attribute " + node.key + "=\"" + node.value + "\")\n";
    }

    void visit(TextNode& node) override {
        result += std::string(indent * 2, ' ');
        result += "(Text \"" + node.text + "\")\n";
    }

private:
    std::string result;
    int indent = 0;
};


int main() {
    std::string source = R"(
html {
    head {
        title { text: "My Page"; }
    }
    body {
        div {
            id = main;
            class: "container";
            text {
                "Hello World"
            }
        }
    }
}
)";

    try {
        std::cout << "--- Parsing Source ---" << std::endl;
        Lexer lexer(source);
        std::vector<Token> tokens = lexer.tokenize();

        Parser parser(tokens);
        std::shared_ptr<RootNode> ast = parser.parse();

        std::cout << "--- AST ---" << std::endl;
        AstPrinter printer;
        std::cout << printer.print(ast) << std::endl;

    } catch (const std::runtime_error& e) {
        std::cerr << "Parsing failed: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
