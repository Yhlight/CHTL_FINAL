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
        case NodeType::StyleBlock:
            // Style blocks are handled by their parent element, so we do nothing here.
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

    // Find style blocks and generate the inline style attribute
    std::stringstream style_ss;
    for (const auto& child : node->children_) {
        if (child->getType() == NodeType::StyleBlock) {
            const auto* styleNode = static_cast<const StyleBlockNode*>(child.get());
            for (const auto& prop : styleNode->properties_) {
                style_ss << prop.first << ": " << prop.second << "; ";
            }
        }
    }

    std::string style_attr = style_ss.str();
    if (!style_attr.empty()) {
        output_ << " style=\"" << style_attr << "\"";
    }

    bool hasNonStyleChildren = false;
    for (const auto& child : node->children_) {
        if (child->getType() != NodeType::StyleBlock) {
            hasNonStyleChildren = true;
            break;
        }
    }

    if (!hasNonStyleChildren) {
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
