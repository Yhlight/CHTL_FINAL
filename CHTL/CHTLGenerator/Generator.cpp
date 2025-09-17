#include "Generator.h"

namespace CHTL {

Generator::Generator() {}

std::string Generator::generate(const Node* rootNode) {
    m_ss.str(""); // Clear the stream
    m_indent_level = 0;
    visit(rootNode);
    return m_ss.str();
}

void Generator::writeIndent() {
    for (int i = 0; i < m_indent_level; ++i) {
        m_ss << "  "; // 2 spaces for indentation
    }
}

void Generator::visit(const Node* node) {
    if (!node) return;

    switch (node->getType()) {
        case NodeType::Root:
            visitRootNode(static_cast<const RootNode*>(node));
            break;
        case NodeType::Element:
            visitElementNode(static_cast<const ElementNode*>(node));
            break;
        case NodeType::Text:
            visitTextNode(static_cast<const TextNode*>(node));
            break;
        case NodeType::Comment:
            // Not implemented in Phase 1
            break;
    }
}

void Generator::visitRootNode(const RootNode* node) {
    for (const auto& child : node->getChildren()) {
        visit(child.get());
    }
}

void Generator::visitElementNode(const ElementNode* node) {
    writeIndent();
    m_ss << "<" << node->getTagName();

    // Write attributes
    for (const auto& attr : node->getAttributes()) {
        m_ss << " " << attr.first << "=\"" << attr.second << "\"";
    }

    if (node->getChildren().empty()) {
        // Self-closing tag for simplicity if no children
        // A more robust solution would check for HTML void elements
        m_ss << " />\n";
    } else {
        m_ss << ">\n";
        m_indent_level++;

        // Visit children
        for (const auto& child : node->getChildren()) {
            visit(child.get());
        }

        m_indent_level--;
        writeIndent();
        m_ss << "</" << node->getTagName() << ">\n";
    }
}

void Generator::visitTextNode(const TextNode* node) {
    writeIndent();
    m_ss << node->getText() << "\n";
}

} // namespace CHTL
