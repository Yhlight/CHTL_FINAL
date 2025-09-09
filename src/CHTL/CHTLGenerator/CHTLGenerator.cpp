#include "CHTLGenerator.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/OriginNode.h"

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
    // Special handling for the dummy root node created by the parser
    if (node.getTagName() == "root") {
        for (const auto& child : node.getChildren()) {
            child->accept(*this);
        }
        return;
    }

    doIndent();
    output += "<" + node.getTagName();

    for (const auto& attr : node.getAttributes()) {
        output += " " + attr.key + "=\"" + attr.value + "\"";
    }

    if (node.getChildren().empty()) {
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

void CHTLGenerator::visit(OriginNode& node) {
    // As per spec, origin blocks are output directly without processing.
    // We might add indentation based on the generator's state, but for now, raw is raw.
    output += node.getContent();
    // Add a newline for cleaner separation in the output, if the content doesn't have one.
    if (!output.empty() && output.back() != '\n') {
        output += '\n';
    }
}
