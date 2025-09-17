#include "Generator.h"
#include <stdexcept>

namespace CHTL {

Generator::Generator(std::shared_ptr<BaseNode> root) : m_root(std::move(root)) {}

void Generator::generate() {
    visit(m_root);
}

std::string Generator::getHtml() const {
    return m_html_stream.str();
}

std::string Generator::getCss() const {
    return m_css_stream.str();
}

void Generator::visit(const std::shared_ptr<BaseNode>& node) {
    if (auto rootNode = std::dynamic_pointer_cast<RootNode>(node)) {
        visitRootNode(rootNode);
    } else if (auto elementNode = std::dynamic_pointer_cast<ElementNode>(node)) {
        visitElementNode(elementNode);
    } else if (auto textNode = std::dynamic_pointer_cast<TextNode>(node)) {
        visitTextNode(textNode);
    } else if (auto styleNode = std::dynamic_pointer_cast<StyleNode>(node)) {
        // Style nodes are handled within the context of their parent element,
        // so we don't do anything here at the top-level visit.
    } else {
        throw std::runtime_error("Unknown node type in generator.");
    }
}

void Generator::visitRootNode(const std::shared_ptr<RootNode>& node) {
    for (const auto& child : node->children) {
        visit(child);
    }
}

void Generator::visitElementNode(const std::shared_ptr<ElementNode>& node) {
    std::string indent(m_indent_level * 2, ' ');
    m_html_stream << indent << "<" << node->tagName;

    // Handle attributes
    for (const auto& attr : node->attributes) {
        m_html_stream << " " << attr->key << "=\"" << attr->value << "\"";
    }

    // Find style node and generate inline styles
    std::stringstream inline_style_stream;
    for (const auto& child : node->children) {
        if (auto styleNode = std::dynamic_pointer_cast<StyleNode>(child)) {
            for (const auto& prop : styleNode->properties) {
                inline_style_stream << prop.key << ": " << prop.value << ";";
            }
        }
    }
    if (inline_style_stream.str().length() > 0) {
        m_html_stream << " style=\"" << inline_style_stream.str() << "\"";
    }


    m_html_stream << ">\n";

    m_indent_level++;
    // Visit children that are not style nodes
    for (const auto& child : node->children) {
        if (!std::dynamic_pointer_cast<StyleNode>(child)) {
            visit(child);
        }
    }
    m_indent_level--;

    m_html_stream << indent << "</" << node->tagName << ">\n";
}

void Generator::visitTextNode(const std::shared_ptr<TextNode>& node) {
    std::string indent(m_indent_level * 2, ' ');
    m_html_stream << indent << node->content << "\n";
}

} // namespace CHTL
