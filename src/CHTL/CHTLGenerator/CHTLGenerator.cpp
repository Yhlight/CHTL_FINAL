#include "CHTLGenerator.h"
#include "AttributeNode.h"
#include <stdexcept>

namespace CHTL {

CHTLGenerator::CHTLGenerator(std::shared_ptr<ElementNode> root) : root(root) {}

std::string CHTLGenerator::generate() {
    // The root node from the parser is a synthetic "root" element.
    // We should generate its children directly without generating the "root" tag.
    for (const auto& child : root->children) {
        generateNode(child);
    }
    return output.str();
}

void CHTLGenerator::indent() {
    for (int i = 0; i < indentLevel; ++i) {
        output << "  "; // Using 2 spaces for indentation
    }
}

void CHTLGenerator::generateNode(const std::shared_ptr<BaseNode>& node) {
    if (auto element = std::dynamic_pointer_cast<ElementNode>(node)) {
        generateElement(element);
    } else if (auto text = std::dynamic_pointer_cast<TextNode>(node)) {
        generateText(text);
    }
    // Silently ignore unknown node types for now.
}

void CHTLGenerator::generateElement(const std::shared_ptr<ElementNode>& node) {
    indent();
    output << "<" << node->tagName;

    // Add regular attributes
    for (const auto& attr : node->attributes) {
        output << " " << attr->key << "=\"" << attr->value << "\"";
    }

    // Add style attribute from parsed styles
    if (!node->styles.empty()) {
        output << " style=\"";
        bool first = true;
        for (const auto& style_pair : node->styles) {
            if (!first) {
                output << " ";
            }
            output << style_pair.first << ": " << style_pair.second << ";";
            first = false;
        }
        output << "\"";
    }

    if (node->children.empty()) {
        // A more robust implementation would check a set of known void elements
        // (like <img>, <br>, etc.) and render them as self-closing.
        // For now, we will just close the tag immediately.
        output << "></" << node->tagName << ">\n";
    } else {
        output << ">\n";
        indentLevel++;
        for (const auto& child : node->children) {
            generateNode(child);
        }
        indentLevel--;
        indent();
        output << "</" << node->tagName << ">\n";
    }
}

void CHTLGenerator::generateText(const std::shared_ptr<TextNode>& node) {
    indent();
    output << node->content << "\n";
}

} // namespace CHTL
