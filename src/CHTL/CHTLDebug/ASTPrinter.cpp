#include "ASTPrinter.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/OriginNode.h"

std::string ASTPrinter::print(BaseNode& node) {
    node.accept(*this);
    return result;
}

void ASTPrinter::indent() {
    for (int i = 0; i < indentLevel; ++i) {
        result += "  ";
    }
}

void ASTPrinter::visit(ElementNode& node) {
    indent();
    result += "Element(" + node.getTagName() + ")\n";

    for (const auto& attr : node.getAttributes()) {
        indent();
        result += "  Attribute(" + attr.key + " = " + attr.value + ")\n";
    }

    indentLevel++;
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }
    indentLevel--;
}

void ASTPrinter::visit(TextNode& node) {
    indent();
    result += "Text(\"" + node.getText() + "\")\n";
}

void ASTPrinter::visit(CommentNode& node) {
    indent();
    result += "Comment(\"" + node.getText() + "\")\n";
}

void ASTPrinter::visit(OriginNode& node) {
    indent();
    result += "Origin(Type: " + node.getType() + ", Content: \"" + node.getContent() + "\")\n";
}
