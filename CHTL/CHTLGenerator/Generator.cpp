#include "Generator.h"

std::string Generator::generate(ElementNode& root) {
    // The root node from the parser is a dummy node.
    // We should generate its children.
    for (auto& child : root.children) {
        child->accept(*this);
    }
    return output.str();
}

void Generator::visit(ElementNode& node) {
    output << "<" << node.tagName;

    for (const auto& attr : node.attributes) {
        output << " " << attr.first << "=\"" << attr.second << "\"";
    }

    // Check for self-closing tags, a simple list for now.
    std::vector<std::string> selfClosingTags = {"area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "param", "source", "track", "wbr"};
    bool isSelfClosing = false;
    for(const auto& tag : selfClosingTags) {
        if (node.tagName == tag) {
            isSelfClosing = true;
            break;
        }
    }

    if (node.children.empty() && isSelfClosing) {
        output << " />";
        return;
    }

    output << ">";

    for (auto& child : node.children) {
        child->accept(*this);
    }

    output << "</" << node.tagName << ">";
}

void Generator::visit(TextNode& node) {
    output << node.content;
}
