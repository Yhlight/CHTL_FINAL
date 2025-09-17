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
    m_ss.str("");
    m_global_css_stream.str("");
    m_indent_level = 0;
    visit(rootNode);
    return m_ss.str();
}

std::string Generator::getGlobalCss() const {
    return m_global_css_stream.str();
}

void Generator::writeIndent() {
    for (int i = 0; i < m_indent_level; ++i) {
        m_ss << "  ";
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
        case NodeType::Style:
            visitStyleNode(static_cast<const StyleNode*>(node));
            break;
        case NodeType::CssRule:
            visitCssRuleNode(static_cast<const CssRuleNode*>(node));
            break;
        case NodeType::Comment:
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

    for (const auto& attr : node->getAttributes()) {
        m_ss << " " << attr.first << "=\"" << attr.second << "\"";
    }

    // Collect inline styles from any StyleNode children
    StyleMap inline_styles;
    for (const auto& child : node->getChildren()) {
        if (child->getType() == NodeType::Style) {
            const auto* style_node = static_cast<const StyleNode*>(child.get());
            for (const auto& style_prop : style_node->getInlineStyles()) {
                inline_styles[style_prop.first] = style_prop.second;
            }
        }
    }

    if (!inline_styles.empty()) {
        m_ss << " style=\"";
        for (auto it = inline_styles.begin(); it != inline_styles.end(); ++it) {
            m_ss << it->first << ": " << it->second << ";";
            if (std::next(it) != inline_styles.end()) {
                m_ss << " ";
            }
        }
        m_ss << "\"";
    }

    if (node->getChildren().empty() && void_elements.count(node->getTagName())) {
        m_ss << " />\n";
        return;
    }

    m_ss << ">\n";
    m_indent_level++;

    // Visit children, but skip StyleNodes as they've been processed
    for (const auto& child : node->getChildren()) {
        if (child->getType() != NodeType::Style) {
            visit(child.get());
        } else {
            // Visit the style node to process its global rules, but don't generate HTML
            visit(child.get());
        }
    }

    m_indent_level--;
    writeIndent();
    m_ss << "</" << node->getTagName() << ">\n";
}

void Generator::visitTextNode(const TextNode* node) {
    writeIndent();
    m_ss << node->getText() << "\n";
}

void Generator::visitStyleNode(const StyleNode* node) {
    // This node's purpose is to hold inline styles and CSS rules.
    // Inline styles are handled by the parent ElementNode visitor.
    // Here, we just need to visit its children to process the global CSS rules.
    for (const auto& child : node->getChildren()) {
        visit(child.get());
    }
}

void Generator::visitCssRuleNode(const CssRuleNode* node) {
    // This node's output goes to the global CSS buffer, not the main HTML stream.
    m_global_css_stream << node->getSelector() << " {\n";
    for (const auto& prop : node->getProperties()) {
        m_global_css_stream << "  " << prop.first << ": " << prop.second << ";\n";
    }
    m_global_css_stream << "}\n";
}

} // namespace CHTL
