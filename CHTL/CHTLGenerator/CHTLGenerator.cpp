#include "CHTLGenerator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h" // Add full definition
#include <unordered_set>

namespace CHTL {

// Set of HTML tags that are "void elements" (i.e., they don't have a closing tag).
const std::unordered_set<std::string> voidElements = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

std::string CHTLGenerator::generate(BaseNode* root) {
    output.str(""); // Clear the stream for fresh generation
    if (root) {
        root->accept(*this);
    }
    return output.str();
}

void CHTLGenerator::visit(ElementNode& node) {
    output << "<" << node.tagName;

    // Append attributes from the element itself
    for (const auto& attr : node.attributes) {
        output << " " << attr.key << "=\"" << attr.value << "\"";
    }

    // Find and process style nodes to create an inline style attribute
    std::string style_str;
    for (const auto& child : node.children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            for (const auto& prop : styleNode->properties) {
                style_str += prop.key + ": " + prop.value + ";";
            }
        }
    }
    if (!style_str.empty()) {
        output << " style=\"" << style_str << "\"";
    }

    // A self-closing tag for void elements.
    if (voidElements.find(node.tagName) != voidElements.end()) {
        output << ">";
        return;
    }

    output << ">";

    // Process children, but skip StyleNodes as they have been processed.
    for (const auto& child : node.children) {
        if (dynamic_cast<StyleNode*>(child.get())) {
            continue;
        }
        child->accept(*this);
    }
    output << "</" << node.tagName << ">";
}

void CHTLGenerator::visit(TextNode& node) {
    // Simply append the text content.
    output << node.text;
}

void CHTLGenerator::visit(StyleNode& node) {
    // This method must be implemented to make the class concrete.
    // However, style nodes are handled within visit(ElementNode&),
    // so this method does nothing.
}

} // namespace CHTL
