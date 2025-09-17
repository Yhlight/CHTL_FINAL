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
    } else if (auto templateNode = std::dynamic_pointer_cast<TemplateNode>(node)) {
        // Template definitions produce no output themselves, so we do nothing.
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
            for (const auto& style_child : styleNode->children) {
                if (auto propNode = std::dynamic_pointer_cast<CssPropertyNode>(style_child)) {
                    inline_style_stream << propNode->key << ": " << propNode->value << ";";
                } else if (auto usageNode = std::dynamic_pointer_cast<TemplateUsageNode>(style_child)) {
                    // It's a template usage, expand it.
                    auto& registry = TemplateRegistry::getInstance();
                    auto templateNode = registry.lookupTemplate(usageNode->name);
                    if (templateNode && templateNode->templateType == usageNode->templateType) {
                        // The body of a style template is another StyleNode
                        for (const auto& template_body_node : templateNode->body) {
                            if (auto templateStyleNode = std::dynamic_pointer_cast<StyleNode>(template_body_node)) {
                                for (const auto& template_prop_node : templateStyleNode->children) {
                                    if (auto prop = std::dynamic_pointer_cast<CssPropertyNode>(template_prop_node)) {
                                        inline_style_stream << prop->key << ": " << prop->value << ";";
                                    }
                                }
                            }
                        }
                    }
                }
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
