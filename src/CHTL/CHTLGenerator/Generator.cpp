#include "Generator.h"

std::string Generator::generate(const NodeList& nodes) {
    output.str(""); // Clear the stream
    for (const auto& node : nodes) {
        generateNode(node.get());
    }
    return output.str();
}

void Generator::generateNode(const BaseNode* node) {
    if (!node) return;

    switch (node->getType()) {
        case NodeType::Element:
            generateElement(static_cast<const ElementNode*>(node));
            break;
        case NodeType::Text:
            generateText(static_cast<const TextNode*>(node));
            break;
        case NodeType::Comment:
            generateComment(static_cast<const CommentNode*>(node));
            break;
        default:
            // Should not happen with a valid AST
            break;
    }
}

void Generator::generateElement(const ElementNode* node) {
    // Opening tag and attributes
    output << "<" << node->tagName;
    for (const auto& attr : node->attributes) {
        output << " " << attr.key << "=\"" << attr.value << "\"";
    }
    output << ">";

    // Children
    for (const auto& child : node->children) {
        generateNode(child.get());
    }

    // Closing tag
    output << "</" << node->tagName << ">";
}

// Helper function to escape HTML special characters
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

void Generator::generateText(const TextNode* node) {
    output << escapeHTML(node->content);
}

void Generator::generateComment(const CommentNode* node) {
    // As per the spec, '--' comments are passed to the generator.
    // Generating a standard HTML comment is a sensible default.
    output << "<!-- " << node->content << " -->";
}
