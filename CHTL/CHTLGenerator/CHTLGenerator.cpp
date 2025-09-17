#include "CHTLGenerator.h"
#include <stdexcept>

namespace CHTL {

CHTLGenerator::CHTLGenerator() : m_indent_level(0) {}

std::string CHTLGenerator::Generate(Program* program) {
    m_output.str(""); // Clear the stream
    m_indent_level = 0;
    for (const auto& stmt : program->statements) {
        generateNode(stmt.get());
    }
    return m_output.str();
}

void CHTLGenerator::indent() {
    for (int i = 0; i < m_indent_level; ++i) {
        m_output << "  "; // 2 spaces for indentation
    }
}

void CHTLGenerator::generateNode(BaseNode* node) {
    if (auto* element = dynamic_cast<ElementNode*>(node)) {
        generateElementNode(element);
    } else if (auto* text = dynamic_cast<TextNode*>(node)) {
        generateTextNode(text);
    } else if (auto* comment = dynamic_cast<CommentNode*>(node)) {
        generateCommentNode(comment);
    }
    // AttributeNode and StyleNode are handled within generateElementNode,
    // so they are not dispatched from here.
}

void CHTLGenerator::generateAttributeNode(AttributeNode* node, std::vector<std::string>& attributes) {
    if (node && node->value) {
        // "key=\"value\""
        attributes.push_back(node->key.literal + "=\"" + node->value->GetTokenLiteral() + "\"");
    }
}

void CHTLGenerator::generateStyleNode(StyleNode* node, std::vector<std::string>& style_properties) {
    for (const auto& prop : node->properties) {
        if (prop && prop->value) {
            // "key:value"
            style_properties.push_back(prop->key.literal + ":" + prop->value->GetTokenLiteral());
        }
    }
}

void CHTLGenerator::generateElementNode(ElementNode* node) {
    std::vector<std::string> attributes;
    std::vector<std::string> style_properties;

    // First pass: collect attributes and styles
    for (const auto& child : node->children) {
        if (auto* attr = dynamic_cast<AttributeNode*>(child.get())) {
            generateAttributeNode(attr, attributes);
        } else if (auto* style = dynamic_cast<StyleNode*>(child.get())) {
            generateStyleNode(style, style_properties);
        }
    }

    indent();
    m_output << "<" << node->GetTokenLiteral();

    // Add attributes to the tag
    if (!attributes.empty()) {
        for (const auto& attr : attributes) {
            m_output << " " << attr;
        }
    }

    // Add inline styles to the tag
    if (!style_properties.empty()) {
        m_output << " style=\"";
        for (size_t i = 0; i < style_properties.size(); ++i) {
            m_output << style_properties[i] << (i == style_properties.size() - 1 ? "" : ";");
        }
        m_output << "\"";
    }

    m_output << ">" << "\n";

    // Second pass: generate content
    m_indent_level++;
    for (const auto& child : node->children) {
        // Only generate nodes that are not attributes or styles
        if (!dynamic_cast<AttributeNode*>(child.get()) && !dynamic_cast<StyleNode*>(child.get())) {
            generateNode(child.get());
        }
    }
    m_indent_level--;

    indent();
    m_output << "</" << node->GetTokenLiteral() << ">" << "\n";
}

void CHTLGenerator::generateTextNode(TextNode* node) {
    indent();
    m_output << node->GetTokenLiteral() << "\n";
}

void CHTLGenerator::generateCommentNode(CommentNode* node) {
    indent();
    // Assuming the token literal is like "# some comment"
    // We want to extract " some comment"
    std::string content = node->GetTokenLiteral();
    if (content.rfind("# ", 0) == 0) {
        content = content.substr(2);
    } else if (content.rfind("#", 0) == 0) {
        content = content.substr(1);
    }
    m_output << "<!-- " << content << " -->" << "\n";
}

} // namespace CHTL
