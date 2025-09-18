#include "CHTLGenerator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
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

    // Append attributes
    for (const auto& attr : node.attributes) {
        output << " " << attr.key << "=\"" << attr.value << "\"";
    }

    // A self-closing tag for void elements.
    if (voidElements.find(node.tagName) != voidElements.end()) {
        output << ">";
        return;
    }

    output << ">";

    // Process children and add a closing tag.
    for (const auto& child : node.children) {
        child->accept(*this);
    }
    output << "</" << node.tagName << ">";
}

void CHTLGenerator::visit(TextNode& node) {
    // Simply append the text content.
    output << node.text;
}

} // namespace CHTL
