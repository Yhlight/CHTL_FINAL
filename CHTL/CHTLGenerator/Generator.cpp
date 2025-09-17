#include "Generator.h"
#include <unordered_set>
#include <iterator>

namespace {
// A helper set of HTML void elements that should not have a closing tag.
const std::unordered_set<std::string> void_elements = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};
}

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

    // Write styles
    const auto& styles = node->getStyles();
    if (!styles.empty()) {
        m_ss << " style=\"";
        for (auto it = styles.begin(); it != styles.end(); ++it) {
            m_ss << it->first << ": " << it->second << ";";
            if (std::next(it) != styles.end()) {
                m_ss << " ";
            }
        }
        m_ss << "\"";
    }

    if (node->getChildren().empty() && void_elements.count(node->getTagName())) {
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
