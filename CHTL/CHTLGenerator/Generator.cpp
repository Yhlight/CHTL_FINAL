#include "Generator.h"
#include <stdexcept>

namespace CHTL {

Generator::Generator(CHTLContext& context) : m_context(context) {}

std::string Generator::generate(Node* node) {
    m_output.str("");
    m_output.clear();
    visit(node);
    return m_output.str();
}

void Generator::visit(Node* node) {
    if (!node) return;

    if (auto elementNode = dynamic_cast<ElementNode*>(node)) {
        visitElementNode(elementNode);
    } else if (auto textNode = dynamic_cast<TextNode*>(node)) {
        visitTextNode(textNode);
    } else if (auto instantiationNode = dynamic_cast<TemplateInstantiationNode*>(node)) {
        visitTemplateInstantiationNode(instantiationNode);
    }
    // TemplateNode definitions are ignored by the generator
}

void Generator::visitTemplateInstantiationNode(TemplateInstantiationNode* node) {
    TemplateNode* templateDef = m_context.findTemplate(node->m_name.literal);
    if (templateDef) {
        // This is a simple implementation. It doesn't handle template arguments yet.
        for (const auto& child : templateDef->children) {
            visit(child.get());
        }
    } else {
        // Handle error: template not found
    }
}


void Generator::visitElementNode(ElementNode* node) {
    if (node->m_tagName == "root") {
        for (const auto& child : node->children) {
            visit(child.get());
        }
        return;
    }

    m_output << "<" << node->m_tagName;

    for (const auto& attr : node->attributes) {
        m_output << " " << attr->m_key << "=\"" << attr->m_value->toString(0) << "\"";
    }

    if (node->styleBlock && !node->styleBlock->properties.empty()) {
        m_output << " style=\"";
        for (size_t i = 0; i < node->styleBlock->properties.size(); ++i) {
            const auto& prop = node->styleBlock->properties[i];
            m_output << prop->m_key << ":" << prop->m_value->toString(0);
            if (i < node->styleBlock->properties.size() - 1) {
                m_output << ";";
            }
        }
        m_output << "\"";
    }

    m_output << ">";

    for (const auto& child : node->children) {
        visit(child.get());
    }

    m_output << "</" << node->m_tagName << ">";
}

void Generator::visitTextNode(TextNode* node) {
    m_output << node->m_value;
}

} // namespace CHTL
