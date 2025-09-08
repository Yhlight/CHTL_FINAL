#include "Generator.h"
#include "../CHTLNode/StyleBlockNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLNode/StyleSelectorNode.h"
#include <stdexcept>
#include <map>

namespace CHTL {

std::string Generator::generate(const DocumentNode* document) {
    m_output.clear();
    m_global_css.clear();
    if (document) {
        for (const auto& child : document->getChildren()) {
            generateNode(child.get());
        }
    }

    if (!m_global_css.empty()) {
        size_t head_pos = m_output.find("</head>");
        if (head_pos != std::string::npos) {
            std::string style_tag = "<style>" + m_global_css + "</style>";
            m_output.insert(head_pos, style_tag);
        } else {
            m_output = "<head><style>" + m_global_css + "</style></head>" + m_output;
        }
    }
    return m_output;
}

void Generator::generateNode(const BaseNode* node) {
    if (!node) return;
    switch (node->getType()) {
        case NodeType::Element:
            generateElement(static_cast<const ElementNode*>(node));
            break;
        case NodeType::Text:
            generateText(static_cast<const TextNode*>(node));
            break;
        default:
            break;
    }
}

void Generator::generateElement(const ElementNode* element) {
    std::string inline_style;
    std::map<std::string, std::string> attributes;

    for (const auto& attr : element->getAttributes()) {
        attributes[attr->getKey()] = attr->getValue();
    }

    if (element->getStyleBlock()) {
        const auto* styleBlock = element->getStyleBlock();
        for (const auto& styleChild : styleBlock->getChildren()) {
            if (styleChild->getType() == NodeType::StyleProperty) {
                const auto* prop = static_cast<const StylePropertyNode*>(styleChild.get());
                inline_style += prop->getKey() + ": " + prop->getValue() + ";";
            } else if (styleChild->getType() == NodeType::StyleSelector) {
                const auto* selectorNode = static_cast<const StyleSelectorNode*>(styleChild.get());
                std::string selector_text = selectorNode->getSelector();

                if (selector_text[0] == '.') {
                    std::string className = selector_text.substr(1);
                    if (attributes.count("class") == 0 || attributes["class"].find(className) == std::string::npos) {
                         attributes["class"] += (attributes["class"].empty() ? "" : " ") + className;
                    }
                } else if (selector_text[0] == '#') {
                    attributes["id"] = selector_text.substr(1);
                }

                size_t amp_pos = selector_text.find('&');
                if (amp_pos != std::string::npos) {
                    std::string base_selector;
                    if (attributes.count("id") > 0 && !attributes.at("id").empty()) {
                        base_selector = "#" + attributes.at("id");
                    } else if (attributes.count("class") > 0 && !attributes.at("class").empty()) {
                        std::string firstClass = attributes.at("class").substr(0, attributes.at("class").find(' '));
                        base_selector = "." + firstClass;
                    } else {
                        base_selector = element->getTagName();
                    }
                    selector_text.replace(amp_pos, 1, base_selector);
                }

                m_global_css += selector_text + " { ";
                for (const auto& prop : selectorNode->getProperties()) {
                    m_global_css += prop->getKey() + ": " + prop->getValue() + "; ";
                }
                m_global_css += "}\n";
            }
        }
    }

    if (!inline_style.empty()) {
        attributes["style"] = inline_style;
    }

    m_output += "<" + element->getTagName();
    for(const auto& pair : attributes) {
        m_output += " " + pair.first + "=\"" + pair.second + "\"";
    }
    m_output += ">";

    for (const auto& child : element->getChildren()) {
        generateNode(child.get());
    }

    m_output += "</" + element->getTagName() + ">";
}

void Generator::generateText(const TextNode* text) {
    m_output += text->getText();
}

} // namespace CHTL
