#include "Generator.h"
#include <vector>
#include <numeric>

namespace CHTL {

std::string Generator::generate(const DocumentNode& doc) {
    m_out.str("");
    for (const auto& node : doc) {
        node->accept(*this);
    }
    return m_out.str();
}

void Generator::visit(ElementNode& node) {
    m_out << "<" << node.tagName;

    // Handle normal attributes
    if (!node.attributes.empty()) {
        m_out << " ";
        for (size_t i = 0; i < node.attributes.size(); ++i) {
            m_out << node.attributes[i].key << "=\"" << node.attributes[i].value << "\"";
            if (i < node.attributes.size() - 1) {
                m_out << " ";
            }
        }
    }

    // Find and process a StyleBlockNode child for inline styles
    StyleBlockNode* style_block = nullptr;
    for (const auto& child : node.children) {
        if (auto sb = dynamic_cast<StyleBlockNode*>(child.get())) {
            style_block = sb;
            break;
        }
    }

    if (style_block && !style_block->properties.empty()) {
        m_out << " style=\"";
        for (size_t i = 0; i < style_block->properties.size(); ++i) {
            const auto& prop = style_block->properties[i];
            m_out << prop.key << ": " << prop.value << ";";
            if (i < style_block->properties.size() - 1) {
                m_out << " ";
            }
        }
        m_out << "\"";
    }

    m_out << ">";

    // Visit children, skipping the style block since it has been processed
    for (const auto& child : node.children) {
        if (dynamic_cast<StyleBlockNode*>(child.get())) {
            continue; // Already processed
        }
        child->accept(*this);
    }

    m_out << "</" << node.tagName << ">";
}

void Generator::visit(TextNode& node) {
    m_out << node.content;
}

void Generator::visit(StyleBlockNode& node) {
    // This visitor method is intentionally left empty.
    // Style blocks are handled by the parent ElementNode's visitor
    // to correctly generate the inline style attribute.
}

} // namespace CHTL
