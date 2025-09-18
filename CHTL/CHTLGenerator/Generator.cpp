#include "Generator.hpp"

namespace CHTL {

Generator::Generator() {}

std::string Generator::generate(const Program* program) {
    std::string html;
    for (const auto& node : program->children) {
        html += generateNode(node.get());
    }
    return html;
}

std::string Generator::generateNode(const Node* node) {
    if (const auto element = dynamic_cast<const ElementNode*>(node)) {
        return generateElementNode(element);
    }
    if (const auto text = dynamic_cast<const TextNode*>(node)) {
        return generateTextNode(text);
    }
    if (const auto comment = dynamic_cast<const CommentNode*>(node)) {
        return comment->toString();
    }
    return "<!-- Unknown Node Type -->";
}

std::string Generator::generateElementNode(const ElementNode* node) {
    // The 'text' tag is special; it doesn't create a <text> element.
    // It's just a container for a TextNode in the AST.
    if (node->tagName == "text") {
        if (!node->children.empty()) {
            // A well-formed 'text' element has one TextNode child.
            return generateNode(node->children[0].get());
        }
        return "";
    }

    std::string html;
    html += "<" + node->tagName;

    for (const auto& attr : node->attributes) {
        html += " " + attr.first + "=\"" + attr.second + "\"";
    }

    if (!node->styles.empty()) {
        html += " style=\"";
        for (const auto& style : node->styles) {
            html += style.first + ":" + style.second + ";";
        }
        html += "\"";
    }

    html += ">";

    for (const auto& child : node->children) {
        html += generateNode(child.get());
    }

    html += "</" + node->tagName + ">";
    return html;
}

std::string Generator::generateTextNode(const TextNode* node) {
    return node->value;
}

} // namespace CHTL
