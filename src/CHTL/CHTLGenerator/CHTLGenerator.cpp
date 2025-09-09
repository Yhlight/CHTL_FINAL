#include "CHTLGenerator.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/TextNode.h"
#include "CHTLNode/CommentNode.h"
#include "CHTLNode/OriginNode.h"
#include <iostream>

CHTLGenerator::CHTLGenerator(CHTLContext* context) : context(context) {}

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

    if (node.getTagName() == "head") {
        output += ">\n";
        indentLevel++;
        // Inject global CSS into head
        const std::string& globalCSS = context->getGlobalCSS();
        if (!globalCSS.empty()) {
            doIndent();
            output += "<style>\n";
            output += globalCSS; // Assuming CSS is already formatted
            doIndent();
            output += "</style>\n";
        }
        for (const auto& child : node.getChildren()) {
            child->accept(*this);
        }
        indentLevel--;
        doIndent();
        output += "</" + node.getTagName() + ">\n";
        return;
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
    output += node.getContent();
    if (!output.empty() && output.back() != '\n') {
        output += '\n';
    }
}
