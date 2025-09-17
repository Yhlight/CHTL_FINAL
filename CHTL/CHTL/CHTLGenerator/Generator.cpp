#include "Generator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/AttributeNode.h"
#include "../CHTLNode/TextNode.h"

std::string Generator::generate(std::shared_ptr<RootNode> ast) {
    html.clear();
    ast->accept(*this);
    return html;
}

void Generator::indent() {
    html += std::string(indentLevel * 2, ' ');
}

void Generator::visit(RootNode& node) {
    // For now, we add a simple doctype. This can be configured later.
    html += "<!DOCTYPE html>\n";
    for (auto& child : node.children) {
        child->accept(*this);
    }
}

void Generator::visit(ElementNode& node) {
    indent();
    html += "<" + node.tagName;

    isFirstAttribute = true;
    for (auto& attr : node.attributes) {
        attr->accept(*this);
    }

    html += ">\n";
    indentLevel++;

    for (auto& child : node.children) {
        child->accept(*this);
    }

    indentLevel--;
    indent();
    html += "</" + node.tagName + ">\n";
}

void Generator::visit(AttributeNode& node) {
    // The isFirstAttribute flag isn't strictly needed but can be useful for formatting.
    // Let's just add a space before each attribute.
    html += " " + node.key + "=\"" + node.value + "\"";
}

void Generator::visit(TextNode& node) {
    indent();
    html += node.text + "\n";
}
