#include "Generator.h"
#include <sstream>

// Initialize the static set of self-closing tags
const std::set<std::string> Generator::s_selfClosingTags = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

std::string Generator::generate(const std::vector<std::unique_ptr<BaseNode>>& program) {
    std::stringstream ss;
    for (const auto& node : program) {
        ss << generateNode(node.get());
    }
    return ss.str();
}

std::string Generator::generateNode(const BaseNode* node) {
    if (!node) {
        return "";
    }

    // Use dynamic_cast to determine the concrete type of the node
    if (auto elementNode = dynamic_cast<const ElementNode*>(node)) {
        return generateElement(elementNode);
    }
    if (auto textNode = dynamic_cast<const TextNode*>(node)) {
        return generateText(textNode);
    }
    if (auto commentNode = dynamic_cast<const CommentNode*>(node)) {
        return generateComment(commentNode);
    }

    // If we have an unknown node type, we return an empty string.
    return "";
}

std::string Generator::generateElement(const ElementNode* node) {
    std::stringstream ss;
    ss << "<" << node->tagName;

    // Append attributes
    for (const auto& attr : node->attributes) {
        ss << " " << attr->name << "=\"" << attr->value << "\"";
    }

    // Handle self-closing tags
    if (s_selfClosingTags.count(node->tagName)) {
        ss << " />";
    } else {
        ss << ">";
        // Append children
        for (const auto& child : node->children) {
            ss << generateNode(child.get());
        }
        ss << "</" << node->tagName << ">";
    }

    return ss.str();
}

// Helper function to escape special HTML characters
std::string escapeHTML(const std::string& data) {
    std::string buffer;
    buffer.reserve(data.size());
    for(size_t pos = 0; pos != data.size(); ++pos) {
        switch(data[pos]) {
            case '&':  buffer.append("&amp;");       break;
            case '\"': buffer.append("&quot;");      break;
            case '\'': buffer.append("&#39;");       break;
            case '<':  buffer.append("&lt;");        break;
            case '>':  buffer.append("&gt;");        break;
            default:   buffer.append(&data[pos], 1); break;
        }
    }
    return buffer;
}

std::string Generator::generateText(const TextNode* node) {
    // HTML-escape the content to prevent XSS
    return escapeHTML(node->content);
}

std::string Generator::generateComment(const CommentNode* node) {
    // Only generator comments (--) are rendered into HTML
    if (node->token.type == TokenType::GENERATOR_COMMENT) {
        // Strip the leading "--" from the lexeme
        std::string content = node->content.substr(2);
        // Trim leading space if it exists
        if (!content.empty() && content[0] == ' ') {
            content = content.substr(1);
        }
        return "<!-- " + content + " -->";
    }
    // Other comment types are ignored
    return "";
}
