#include "Generator.h"
#include <unordered_set>
#include <iterator>
#include <regex>

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
    visit(rootNode, nullptr);
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

void Generator::visit(const Node* node, const ElementNode* parent_context) {
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
            visitStyleNode(static_cast<const StyleNode*>(node), parent_context);
            break;
        case NodeType::CssRule:
            visitCssRuleNode(static_cast<const CssRuleNode*>(node), parent_context);
            break;
        case NodeType::Comment:
            break;
    }
}

void Generator::visitRootNode(const RootNode* node) {
    for (const auto& child : node->getChildren()) {
        visit(child.get(), nullptr);
    }
}

void Generator::visitElementNode(const ElementNode* node) {
    writeIndent();
    m_ss << "<" << node->getTagName();

    for (const auto& attr : node->getAttributes()) {
        m_ss << " " << attr.first << "=\"" << attr.second << "\"";
    }

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
            if (std::next(it) != inline_styles.end()) m_ss << " ";
        }
        m_ss << "\"";
    }

    if (node->getChildren().empty() && void_elements.count(node->getTagName())) {
        m_ss << " />\n";
        return;
    }

    m_ss << ">\n";
    m_indent_level++;

    for (const auto& child : node->getChildren()) {
        visit(child.get(), node);
    }

    m_indent_level--;
    writeIndent();
    m_ss << "</" << node->getTagName() << ">\n";
}

void Generator::visitTextNode(const TextNode* node) {
    writeIndent();
    m_ss << node->getText() << "\n";
}

void Generator::visitStyleNode(const StyleNode* node, const ElementNode* parent_context) {
    for (const auto& child : node->getChildren()) {
        visit(child.get(), parent_context);
    }
}

void Generator::visitCssRuleNode(const CssRuleNode* node, const ElementNode* parent_context) {
    std::string selector = node->getSelector();

    // Resolve '&' if it exists
    if (selector.find('&') != std::string::npos) {
        std::string resolved_base;
        if (parent_context) {
            const auto& attrs = parent_context->getAttributes();
            if (attrs.count("class")) {
                // For simplicity, we use the first class name if there are multiple.
                std::string first_class = attrs.at("class").substr(0, attrs.at("class").find(' '));
                resolved_base = "." + first_class;
            } else if (attrs.count("id")) {
                resolved_base = "#" + attrs.at("id");
            }
        }

        if (!resolved_base.empty()) {
            selector = std::regex_replace(selector, std::regex("&"), resolved_base);
        }
    }

    m_global_css_stream << selector << " {\n";
    for (const auto& prop : node->getProperties()) {
        m_global_css_stream << "  " << prop.first << ": " << prop.second << ";\n";
    }
    m_global_css_stream << "}\n";
}

} // namespace CHTL
