#include "Generator.h"
#include "../CHTLNode/StyleBlockNode.h"
#include "../CHTLNode/StylePropertyNode.h"
#include "../CHTLNode/StyleSelectorNode.h"
#include "../ExpressionNode/LiteralExpr.h"
#include "../Evaluator/Evaluator.h"
#include <stdexcept>
#include <map>

namespace CHTL {

// Helper to check if an expression is a simple literal that can be evaluated early.
bool isLiteral(const Expr* expr) {
    if (!expr) return false;
    // This is a bit of a hack. A better way would be a visitor.
    return dynamic_cast<const LiteralExpr*>(expr) != nullptr;
}

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
    EvaluationContext context;

    for (const auto& attr : element->getAttributes()) {
        attributes[attr->getKey()] = attr->getValue();
    }

    if (element->getStyleBlock()) {
        const auto* styleBlock = element->getStyleBlock();

        // First pass: evaluate all simple literal properties to build the context
        for (const auto& styleChild : styleBlock->getChildren()) {
            if (styleChild->getType() == NodeType::StyleProperty) {
                const auto* prop = static_cast<const StylePropertyNode*>(styleChild.get());
                if (isLiteral(prop->getValue())) {
                    Evaluator eval;
                    context[prop->getKey()] = eval.evaluate(prop->getValue(), context);
                }
            }
        }

        // Second pass: evaluate all properties (including complex ones) and generate CSS
        for (const auto& styleChild : styleBlock->getChildren()) {
            if (styleChild->getType() == NodeType::StyleProperty) {
                const auto* prop = static_cast<const StylePropertyNode*>(styleChild.get());
                Evaluator eval;
                ChtlValue value = eval.evaluate(prop->getValue(), context);
                inline_style += prop->getKey() + ": " + value.toString() + ";";
            } else if (styleChild->getType() == NodeType::StyleSelector) {
                // ... (This logic remains largely the same, but needs to use the evaluator too)
                const auto* selectorNode = static_cast<const StyleSelectorNode*>(styleChild.get());
                std::string selector_text = selectorNode->getSelector();

                if (selector_text[0] == '.') {
                    attributes["class"] += (attributes["class"].empty() ? "" : " ") + selector_text.substr(1);
                } else if (selector_text[0] == '#') {
                    attributes["id"] = selector_text.substr(1);
                }

                size_t amp_pos = selector_text.find('&');
                if (amp_pos != std::string::npos) {
                     std::string base_selector;
                    if (attributes.count("id") > 0 && !attributes.at("id").empty()) base_selector = "#" + attributes.at("id");
                    else if (attributes.count("class") > 0 && !attributes.at("class").empty()) base_selector = "." + attributes.at("class").substr(0, attributes.at("class").find(' '));
                    else base_selector = element->getTagName();
                    selector_text.replace(amp_pos, 1, base_selector);
                }

                m_global_css += selector_text + " { ";
                for (const auto& prop : selectorNode->getProperties()) {
                     Evaluator eval;
                     ChtlValue value = eval.evaluate(prop->getValue(), context);
                     m_global_css += prop->getKey() + ": " + value.toString() + "; ";
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
