#include "Generator.h"
#include <vector>
#include <iostream>

std::string Generator::generate(const NodeList& ast) {
    for (const auto& node : ast) {
        node->accept(*this);
    }
    return output.str();
}

void Generator::visit(ElementNode* node) {
    printIndent();
    output << "<" << node->tagName;

    std::vector<PropertyNode*> properties;
    std::vector<Node*> otherChildren;
    for (const auto& child : node->children) {
        if (auto* prop = dynamic_cast<PropertyNode*>(child.get())) {
            properties.push_back(prop);
        } else {
            otherChildren.push_back(child.get());
        }
    }

    for (const auto* prop : properties) {
        output << " " << prop->name << "=\"" << prop->value << "\"";
    }

    output << ">";

    if (!otherChildren.empty()) {
        output << "\n";
        indent();
        for (auto* child : otherChildren) {
            child->accept(*this);
        }
        dedent();
        printIndent();
    }

    output << "</" << node->tagName << ">\n";
}

void Generator::visit(TextNode* node) {
    printIndent();
    output << node->text << "\n";
}

void Generator::visit(CommentNode* node) {
    if (node->isGeneratorComment) {
        printIndent();
        output << "<!--" << node->text << "-->\n";
    }
}

void Generator::visit(PropertyNode* node) {
    // Handled in ElementNode visit.
}

void Generator::indent() {
    indent_level++;
}

void Generator::dedent() {
    indent_level--;
}

void Generator::printIndent() {
    for (int i = 0; i < indent_level; ++i) {
        output << "  ";
    }
}
