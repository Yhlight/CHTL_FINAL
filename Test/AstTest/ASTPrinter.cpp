#include "ASTPrinter.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/TextNode.h"
#include "../../CHTL/CHTLNode/StyleNode.h"
#include <iostream>

namespace CHTL {

void ASTPrinter::print(BaseNode* node) {
    if (node) {
        node->accept(*this);
    }
}

void ASTPrinter::visit(ElementNode& node) {
    std::cout << indentString() << "<" << node.tagName << ">" << std::endl;

    indent++;
    // Print attributes
    for (const auto& attr : node.attributes) {
        std::cout << indentString() << "@" << attr.key << " = \"" << attr.value << "\"" << std::endl;
    }

    // Print children
    for (const auto& child : node.children) {
        child->accept(*this);
    }
    indent--;
}

void ASTPrinter::visit(TextNode& node) {
    std::cout << indentString() << "Text: \"" << node.text << "\"" << std::endl;
}

std::string ASTPrinter::indentString() {
    return std::string(indent * 2, ' ');
}

void ASTPrinter::visit(StyleNode& node) {
    std::cout << indentString() << "<style>" << std::endl;
    indent++;
    for (const auto& prop : node.properties) {
        std::cout << indentString() << prop.key << ": " << prop.value << ";" << std::endl;
    }
    indent--;
}

} // namespace CHTL
