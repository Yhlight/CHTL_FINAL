#include "Generator.h"
#include <stdexcept>

namespace CHTL {

// Forward declaration for recursive call
void generateStyleString(Node* node, CHTLContext& context, std::stringstream& ss);


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
}

void Generator::visitTemplateInstantiationNode(TemplateInstantiationNode* node) {
    TemplateNode* templateDef = m_context.findTemplate(node->m_name.literal);
    if (templateDef) {
        if (node->m_templateType.type == TokenType::AT_ELEMENT) {
            for (const auto& child : templateDef->children) {
                visit(child.get());
            }
        }
        // @Style instantiations are handled within visitElementNode
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

    if (node->styleBlock && !node->styleBlock->children.empty()) {
        m_output << " style=\"";
        for (const auto& child : node->styleBlock->children) {
            generateStyleString(child.get(), m_context, m_output);
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


// Helper function to recursively generate style strings
void generateStyleString(Node* node, CHTLContext& context, std::stringstream& ss) {
    if (auto attr = dynamic_cast<AttributeNode*>(node)) {
        ss << attr->m_key << ":" << attr->m_value->toString(0) << ";";
    } else if (auto inst = dynamic_cast<TemplateInstantiationNode*>(node)) {
        if (inst->m_templateType.type == TokenType::AT_STYLE) {
            TemplateNode* templateDef = context.findTemplate(inst->m_name.literal);
            if (templateDef) {
                for (const auto& child : templateDef->children) {
                    // Recurse in case a style template includes another style template
                    generateStyleString(child.get(), context, ss);
                }
            }
        }
    }
}

} // namespace CHTL
