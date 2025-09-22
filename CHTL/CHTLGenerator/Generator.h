#pragma once

#include "../CHTLNode/BaseNode.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include <vector>
#include <memory>
#include <string>
#include <sstream>
#include <stdexcept>

namespace CHTL {

class Generator {
public:
    // Main entry point. Takes the program AST and returns the generated HTML string.
    std::string Generate(const std::vector<std::unique_ptr<BaseNode>>& program) {
        std::stringstream output;
        for (const auto& node : program) {
            generateNode(node.get(), output);
        }
        return output.str();
    }

private:
    // Dispatches generation to the appropriate function based on the node's type.
    void generateNode(const BaseNode* node, std::stringstream& output) {
        if (!node) return;

        // Use dynamic_cast to determine the concrete type of the node at runtime.
        if (const auto* elementNode = dynamic_cast<const ElementNode*>(node)) {
            generateElementNode(elementNode, output);
        } else if (const auto* textNode = dynamic_cast<const TextNode*>(node)) {
            generateTextNode(textNode, output);
        } else {
            throw std::runtime_error("Unknown node type encountered in generator.");
        }
    }

    // Generates HTML for an ElementNode.
    void generateElementNode(const ElementNode* node, std::stringstream& output) {
        // Output the opening tag.
        output << "<" << node->tagName << ">";

        // Recursively generate all child nodes.
        for (const auto& child : node->children) {
            generateNode(child.get(), output);
        }

        // Output the closing tag.
        output << "</" << node->tagName << ">";
    }

    // Generates the content for a TextNode.
    void generateTextNode(const TextNode* node, std::stringstream& output) {
        // For this basic version, we output the value directly.
        // A production-ready generator would need to escape HTML special characters.
        output << node->value;
    }
};

} // namespace CHTL
