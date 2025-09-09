#include "Generator.h"
#include "../CHTLNode/Node.h"
#include <stdexcept>
#include <unordered_set>
#include <map>
#include <string>
#include <vector>

namespace CHTL {

// Set of HTML tags that are "void elements" (i.e., they don't have a closing tag).
const std::unordered_set<std::string> VOID_ELEMENTS = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

Generator::Generator() : m_indent_level(0) {}

std::string Generator::getIndentation() const {
    return std::string(m_indent_level * 2, ' ');
}

std::string Generator::generate(const std::vector<std::unique_ptr<Node>>& roots) {
    // Clear previous outputs
    m_output.str("");
    m_global_css.str("");

    // First pass: generate the main HTML body and collect global CSS rules
    for (const auto& root : roots) {
        visit(root.get());
    }

    std::string body_html = m_output.str();
    std::string global_css = m_global_css.str();

    // If there is global CSS to inject, find the head tag and insert it.
    if (!global_css.empty()) {
        std::string style_tag = "\n" + getIndentation() + "<style>\n" + global_css + getIndentation() + "</style>\n";

        size_t head_pos = body_html.find("</head>");
        if (head_pos != std::string::npos) {
            body_html.insert(head_pos, style_tag);
        } else {
            // If no head tag, prepend one. This is a fallback.
            body_html = "<html>\n<head>" + style_tag + "</head>\n<body>\n" + body_html + "</body>\n</html>";
        }
    }

    return body_html;
}

void Generator::visit(const Node* node) {
    if (!node) return;

    switch (node->getType()) {
        case NodeType::Element:
            visitElement(static_cast<const ElementNode*>(node));
            break;
        case NodeType::Text:
            visitText(static_cast<const TextNode*>(node));
            break;
        case NodeType::Attribute:
            throw std::runtime_error("Encountered an AttributeNode at the top level of visitation.");
        default:
            // Other node types like style nodes are handled within visitElement
            break;
    }
}

void Generator::visitElement(const ElementNode* node) {
    m_output << getIndentation() << "<" << node->getTagName();

    // --- Attribute Aggregation ---
    std::map<std::string, std::string> attributes;

    // 1. Get attributes from the AST
    for (const auto& attr : node->getAttributes()) {
        attributes[attr->getKey()] = attr->getValue();
    }

    // 2. Process style block for inline styles and automated attributes
    if (node->getStyleBlock()) {
        const auto* styleBlock = node->getStyleBlock();
        std::string inline_style_str;

        for (const auto& prop : styleBlock->getInlineProperties()) {
            inline_style_str += prop->getKey() + ":" + prop->getValue() + ";";
        }
        if (!inline_style_str.empty()) {
            attributes["style"] = inline_style_str;
        }

        for (const auto& rule : styleBlock->getCssRules()) {
            std::string selector = rule->getSelector();
            if (selector.rfind(".", 0) == 0) { // starts with .
                std::string className = selector.substr(1);
                if (attributes.count("class")) {
                    attributes["class"] += " " + className;
                } else {
                    attributes["class"] = className;
                }
            } else if (selector.rfind("#", 0) == 0) { // starts with #
                attributes["id"] = selector.substr(1); // Overwrite existing ID
            }

            m_global_css << getIndentation() << "  " << selector << " {\n";
            for (const auto& prop : rule->getProperties()) {
                m_global_css << getIndentation() << "    " << prop->getKey() << ": " << prop->getValue() << ";\n";
            }
            m_global_css << getIndentation() << "  }\n";
        }
    }

    // --- Render all aggregated attributes ---
    for (const auto& pair : attributes) {
        m_output << " " << pair.first << "=\"" << pair.second << "\"";
    }

    bool is_void = VOID_ELEMENTS.count(node->getTagName()) > 0;
    if (is_void && node->getChildren().empty()) {
        m_output << ">\n";
        return;
    }

    m_output << ">\n";

    m_indent_level++;
    for (const auto& child : node->getChildren()) {
        visit(child.get());
    }
    m_indent_level--;

    if (!is_void) {
        m_output << getIndentation() << "</" << node->getTagName() << ">\n";
    }
}

void Generator::visitText(const TextNode* node) {
    // Basic HTML escaping could be added here if needed.
    m_output << getIndentation() << node->getText() << "\n";
}

} // namespace CHTL
