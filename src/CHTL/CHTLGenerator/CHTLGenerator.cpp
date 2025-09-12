#include "CHTLGenerator.h"

namespace CHTL {

std::string CHTLGenerator::generate(const RootNode& root) {
    for (const auto& child : root.children_) {
        visit(child.get());
    }
    return output_.str();
}

void CHTLGenerator::visit(const Node* node) {
    if (!node) return;
    switch (node->getType()) {
        case NodeType::Element:
            visitElement(static_cast<const ElementNode*>(node));
            break;
        case NodeType::Text:
            visitText(static_cast<const TextNode*>(node));
            break;
        case NodeType::Comment:
            visitComment(static_cast<const CommentNode*>(node));
            break;
        default:
            // Handle other node types or throw an error
            break;
    }
}

void CHTLGenerator::visitElement(const ElementNode* node) {
    indent();
    output_ << "<" << node->tagName_;

    for (const auto& attr : node->attributes_) {
        output_ << " " << attr->key_ << "=\"" << attr->value_ << "\"";
    }

    if (node->children_.empty()) {
        // Self-closing tag for simplicity, though not all tags work this way in HTML
        output_ << " />\n";
    } else {
        output_ << ">\n";
        indentLevel_++;
        for (const auto& child : node->children_) {
            visit(child.get());
        }
        indentLevel_--;
        indent();
        output_ << "</" << node->tagName_ << ">\n";
    }
}

void CHTLGenerator::visitText(const TextNode* node) {
    indent();
    output_ << node->text_ << "\n";
}

void CHTLGenerator::visitComment(const CommentNode* node) {
    indent();
    // CHTL generator comments '--' are converted to HTML comments
    output_ << "<!-- " << node->comment_ << " -->\n";
}

void CHTLGenerator::indent() {
    for (int i = 0; i < indentLevel_; ++i) {
        output_ << "  "; // 2 spaces for indentation
    }
}

} // namespace CHTL
