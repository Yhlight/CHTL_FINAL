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
    // TODO: Add attribute generation here in the future
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
