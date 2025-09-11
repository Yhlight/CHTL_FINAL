#include "Generator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"

std::string Generator::generate(std::shared_ptr<BaseNode> root) {
    output.clear();
    indentLevel = 0;
    if (root) {
        root->accept(*this);
    }
    return output;
}

void Generator::indent() {
    for (int i = 0; i < indentLevel; ++i) {
        output += "  "; // 2 spaces for indentation
    }
}

void Generator::visit(ElementNode& node) {
    indent();
    output += "<" + node.tagName;

    // Append attributes
    for (const auto& attr : node.attributes) {
        output += " " + attr.first + "=\"" + attr.second + "\"";
    }

    if (node.children.empty()) {
        // Self-closing tag for simplicity, though not all tags work this way.
        // A more robust implementation would have a list of void elements.
        output += " />\n";
    } else {
        output += ">\n";
        indentLevel++;
        for (const auto& child : node.children) {
            child->accept(*this);
        }
        indentLevel--;
        indent();
        output += "</" + node.tagName + ">\n";
    }
}

void Generator::visit(TextNode& node) {
    indent();
    output += node.text + "\n";
}

void Generator::visit(CommentNode& node) {
    indent();
    output += "<!-- " + node.comment + " -->\n";
}
