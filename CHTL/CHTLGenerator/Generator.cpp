#include "Generator.h"

namespace CHTL {

Generator::Generator(CHTLContext& context) : m_context(context) {}

std::string Generator::generate(const DocumentNode& doc) {
    m_out.str("");
    for (const auto& node : doc) {
        // Don't generate output for top-level template definitions
        if (dynamic_cast<StyleTemplateDefinitionNode*>(node.get())) {
            continue;
        }
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

    if (style_block) {
        std::stringstream style_ss;
        // Temporarily redirect output to the style stringstream
        std::swap(m_out, style_ss);
        m_is_in_style_block = true;

        style_block->accept(*this);

        m_is_in_style_block = false;
        std::swap(m_out, style_ss); // Swap back

        std::string style_content = style_ss.str();
        if (!style_content.empty()) {
            m_out << " style=\"" << style_content << "\"";
        }
    }

    m_out << ">";

    // Visit children, skipping the style block and template definitions
    for (const auto& child : node.children) {
        if (dynamic_cast<StyleBlockNode*>(child.get()) || dynamic_cast<StyleTemplateDefinitionNode*>(child.get())) {
            continue;
        }
        child->accept(*this);
    }

    m_out << "</" << node.tagName << ">";
}

void Generator::visit(TextNode& node) {
    m_out << node.content;
}

void Generator::visit(StyleBlockNode& node) {
    for (size_t i = 0; i < node.items.size(); ++i) {
        node.items[i]->accept(*this);
        // Add a space after each property/template for better formatting inside the attribute
        if (i < node.items.size() - 1) {
             m_out << " ";
        }
    }
}

void Generator::visit(CssPropertyNode& node) {
    if (m_is_in_style_block) {
        m_out << node.key << ": " << node.value << ";";
    }
}

void Generator::visit(StyleTemplateUsageNode& node) {
    if (m_is_in_style_block) {
        const StyleTemplate* tpl = m_context.getStyleTemplate(node.name);
        if (tpl) {
            for (size_t i = 0; i < tpl->properties.size(); ++i) {
                const auto& prop = tpl->properties[i];
                m_out << prop.key << ": " << prop.value << ";";
                 if (i < tpl->properties.size() - 1) {
                    m_out << " ";
                }
            }
        }
        // If template not found, we could throw an error or fail silently.
        // For now, we fail silently.
    }
}

void Generator::visit(StyleTemplateDefinitionNode& node) {
    // This visitor method is intentionally left empty.
    // Template definitions are handled at parse time and stored in the context.
    // They are not rendered directly.
}

void Generator::visit(ImportNode& node) {
    // This visitor method is intentionally left empty.
    // Imports are handled at parse time and do not generate any output.
}


} // namespace CHTL
