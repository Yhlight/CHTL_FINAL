#include "Generator.h"
#include <stdexcept>

namespace CHTL {

std::string Generator::generate(const DocumentNode* document) {
    m_output.clear();
    if (document) {
        for (const auto& child : document->getChildren()) {
            generateNode(child.get());
        }
    }
    return m_output;
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
        // Other node types are ignored for now.
        case NodeType::Document:
        case NodeType::Attribute:
        default:
            break;
    }
}

void Generator::generateElement(const ElementNode* element) {
    // Append the opening tag, e.g., "<div"
    m_output += "<" + element->getTagName();

    // Append attributes, e.g., ' id="main"'
    for (const auto& attr : element->getAttributes()) {
        // Basic sanitization could be added here later (e.g., for quotes in value)
        m_output += " " + attr->getKey() + "=\"" + attr->getValue() + "\"";
    }

    // Close the opening tag
    m_output += ">";

    // Recursively generate all child nodes
    for (const auto& child : element->getChildren()) {
        generateNode(child.get());
    }

    // Append the closing tag, e.g., "</div>"
    m_output += "</" + element->getTagName() + ">";
}

void Generator::generateText(const TextNode* text) {
    // TODO: Add HTML escaping for special characters like <, >, &
    m_output += text->getText();
}

} // namespace CHTL
