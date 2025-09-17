#include "Generator.h"
#include "../CHTLNode/Node.h"
#include <stdexcept>
#include <unordered_set>

// Set of HTML void elements that should not have a closing tag.
const std::unordered_set<std::string> voidElements = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};


Generator::Generator() : indentLevel(0) {}

void Generator::printIndent() {
    for (int i = 0; i < indentLevel; ++i) {
        output << "  ";
    }
}

std::string Generator::generate(const std::shared_ptr<RootNode>& ast) {
    output.str(""); // Clear the stream
    output.clear();
    for (const auto& statement : ast->statements) {
        visit(statement);
    }
    return output.str();
}

void Generator::visit(const std::shared_ptr<Node>& node) {
    if (auto element = std::dynamic_pointer_cast<ElementNode>(node)) {
        visitElement(element);
    } else if (auto text = std::dynamic_pointer_cast<TextNode>(node)) {
        visitText(text);
    }
}

void Generator::visitElement(const std::shared_ptr<ElementNode>& node) {
    printIndent();
    output << "<" << node->tagName;

    for (const auto& attr : node->attributes) {
        output << " " << attr->name << "=\"" << attr->value << "\"";
    }
    output << ">";

    bool isVoid = voidElements.count(node->tagName) > 0;
    if (isVoid) {
        output << "\n";
        return;
    }

    // If the element is not void, proceed with children and closing tag.
    if (!node->children.empty()) {
        output << "\n";
        indentLevel++;
        for (const auto& child : node->children) {
            visit(child);
        }
        indentLevel--;
        printIndent();
    }

    output << "</" << node->tagName << ">\n";
}

void Generator::visitText(const std::shared_ptr<TextNode>& node) {
    printIndent();
    output << node->content << "\n";
}
