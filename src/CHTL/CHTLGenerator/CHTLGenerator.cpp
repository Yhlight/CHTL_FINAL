#include "CHTLGenerator.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"

std::string CHTLGenerator::generate(BaseNode& root) {
    root.accept(*this);
    return output;
}

void CHTLGenerator::doIndent() {
    for (int i = 0; i < indentLevel; ++i) {
        output += "  ";
    }
}

void CHTLGenerator::visit(ElementNode& node) {
    doIndent();
    output += "<" + node.getTagName();

    for (const auto& attr : node.getAttributes()) {
        output += " " + attr.key + "=\"" + attr.value + "\"";
    }

    if (node.getChildren().empty()) {
        // Handle self-closing tags later if needed. For now, all tags have a closing tag.
        // A simple list of self-closing tags could be checked here.
        output += "></" + node.getTagName() + ">\n";
        return;
    }

    output += ">\n";

    indentLevel++;
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }
    indentLevel--;

    doIndent();
    output += "</" + node.getTagName() + ">\n";
}

void CHTLGenerator::visit(TextNode& node) {
    doIndent();
    output += node.getText() + "\n";
}

void CHTLGenerator::visit(CommentNode& node) {
    doIndent();
    output += "<!-- " + node.getText() + " -->\n";
}
