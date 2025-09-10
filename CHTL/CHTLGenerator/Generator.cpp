#include "Generator.h"
#include <stdexcept>

// Helper function for HTML escaping
std::string escapeHTML(const std::string& data) {
    std::string buffer;
    buffer.reserve(data.size());
    for(size_t pos = 0; pos != data.size(); ++pos) {
        switch(data[pos]) {
            case '&':  buffer.append("&amp;");       break;
            case '\"': buffer.append("&quot;");      break;
            case '\'': buffer.append("&apos;");      break;
            case '<':  buffer.append("&lt;");        break;
            case '>':  buffer.append("&gt;");        break;
            default:   buffer.append(&data[pos], 1); break;
        }
    }
    return buffer;
}

std::string Generator::generate(const BaseNode* root) {
    if (!root) {
        return "";
    }
    output.str(""); // Clear the stream
    visit(root);
    return output.str();
}

void Generator::visit(const BaseNode* node) {
    // Dispatch to the correct visitor based on the node's type
    if (const auto* elem = dynamic_cast<const ElementNode*>(node)) {
        visitElementNode(elem);
    } else if (const auto* text = dynamic_cast<const TextNode*>(node)) {
        visitTextNode(text);
    } else if (const auto* comment = dynamic_cast<const CommentNode*>(node)) {
        visitCommentNode(comment);
    } else {
        // Should not happen with our current node types
        throw std::runtime_error("Unknown node type in AST.");
    }
}

void Generator::visitElementNode(const ElementNode* node) {
    // Opening tag
    output << "<" << node->tagName;

    // Attributes
    for (const auto& attr : node->attributes) {
        output << " " << attr.first << "=\"" << escapeHTML(attr.second) << "\"";
    }

    output << ">";

    // Children
    for (const auto& child : node->children) {
        visit(child.get());
    }

    // Closing tag
    output << "</" << node->tagName << ">";
}

void Generator::visitTextNode(const TextNode* node) {
    output << escapeHTML(node->content);
}

void Generator::visitCommentNode(const CommentNode* node) {
    output << "<!-- " << node->content << " -->";
}
