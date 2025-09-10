#include "ASTPrinter.h"

void ASTPrinter::print(const NodeList& nodes) {
    std::cout << "--- AST ---" << std::endl;
    for (const auto& node : nodes) {
        node->accept(*this);
    }
    std::cout << "-----------" << std::endl;
}

void ASTPrinter::visit(ElementNode* node) {
    printIndent();
    std::cout << "<" << node->tagName << ">" << std::endl;
    indent();
    for (const auto& child : node->children) {
        child->accept(*this);
    }
    dedent();
    printIndent();
    std::cout << "</" << node->tagName << ">" << std::endl;
}

void ASTPrinter::visit(TextNode* node) {
    printIndent();
    std::cout << "Text: \"" << node->text << "\"" << std::endl;
}

void ASTPrinter::visit(CommentNode* node) {
    printIndent();
    if (node->isGeneratorComment) {
        std::cout << "Generator Comment: " << node->text << std::endl;
    } else {
        std::cout << "Comment: " << node->text << std::endl;
    }
}

void ASTPrinter::visit(PropertyNode* node) {
    printIndent();
    std::cout << "Property: " << node->name << " = \"" << node->value << "\"" << std::endl;
}

void ASTPrinter::indent() {
    indent_level++;
}

void ASTPrinter::dedent() {
    indent_level--;
}

void ASTPrinter::printIndent() {
    for (int i = 0; i < indent_level; ++i) {
        std::cout << "  ";
    }
}
