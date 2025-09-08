#include "Generator.h"
#include "CHTL/CHTLNode/StyleBlockNode.h"
#include "CHTL/CHTLNode/StylePropertyNode.h"
#include "CHTL/CHTLNode/StyleSelectorNode.h"
#include "CHTL/ExpressionNode/LiteralExpr.h"
#include "CHTL/Evaluator/Evaluator.h"
#include <stdexcept>
#include <map>
#include <vector>

namespace CHTL {

bool isLiteral(const Expr* expr) {
    if (!expr) return false;
    return dynamic_cast<const LiteralExpr*>(expr) != nullptr;
}

std::string Generator::generate(const DocumentNode* document, const CHTLContext& context) {
    m_context = &context;
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
            m_output.insert(head_pos, "<style>" + m_global_css + "</style>");
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
        case NodeType::TemplateUsage:
            generateTemplateUsage(static_cast<const TemplateUsageNode*>(node), nullptr);
            break;
        default:
            break;
    }
}

void Generator::expandStyleProperties(const BaseNode* container, std::map<std::string, const StylePropertyNode*>& properties) {
    if (!container) return;

    const std::vector<std::unique_ptr<BaseNode>>* children = nullptr;
    if (container->getType() == NodeType::StyleBlock) {
        children = &(static_cast<const StyleBlockNode*>(container)->getChildren());
    } else if (container->getType() == NodeType::TemplateDefinition) {
        children = &(static_cast<const TemplateDefinitionNode*>(container)->getChildren());
    }
    if (!children) return;

    for (const auto& child : *children) {
        if (child->getType() == NodeType::StyleProperty) {
            const auto* propNode = static_cast<const StylePropertyNode*>(child.get());
            properties[propNode->getKey()] = propNode;
        } else if (child->getType() == NodeType::TemplateUsage) {
            const auto* usageNode = static_cast<const TemplateUsageNode*>(child.get());
            if (usageNode->getTemplateType() == "Style") {
                const TemplateDefinitionNode* baseTemplate = m_context->getTemplate(usageNode->getTemplateName());
                expandStyleProperties(baseTemplate, properties);
            }
        }
    }
}

void Generator::generateTemplateUsage(const TemplateUsageNode* node, const ElementNode* parentElement) {
    const TemplateDefinitionNode* def = m_context->getTemplate(node->getTemplateName());
    if (!def) {
        m_output += "<!-- Template not found: " + node->getTemplateName() + " -->";
        return;
    }
    if (node->getTemplateType() == "Element") {
        for (const auto& child : def->getChildren()) {
            generateNode(child.get());
        }
    }
}

void Generator::generateElement(const ElementNode* element) {
    std::map<std::string, std::string> attributes;
    for (const auto& attr : element->getAttributes()) {
        attributes[attr->getKey()] = attr->getValue();
    }

    if (element->getStyleBlock()) {
        std::map<std::string, const StylePropertyNode*> final_properties;
        expandStyleProperties(element->getStyleBlock(), final_properties);

        EvaluationContext context;
        for(const auto& pair : final_properties) {
            if(isLiteral(pair.second->getValue())) {
                Evaluator eval;
                context[pair.first] = eval.evaluate(pair.second->getValue(), context);
            }
        }

        std::string inline_style;
        for(const auto& pair : final_properties) {
            Evaluator eval;
            ChtlValue value = eval.evaluate(pair.second->getValue(), context);
            inline_style += pair.first + ": " + value.toString() + ";";
        }
        if (!inline_style.empty()) attributes["style"] = inline_style;

        for (const auto& styleChild : element->getStyleBlock()->getChildren()) {
            if (styleChild->getType() == NodeType::StyleSelector) {
                const auto* selectorNode = static_cast<const StyleSelectorNode*>(styleChild.get());
                std::string selector_text = selectorNode->getSelector();

                if (selector_text[0] == '.') attributes["class"] += (attributes["class"].empty() ? "" : " ") + selector_text.substr(1);
                else if (selector_text[0] == '#') attributes["id"] = selector_text.substr(1);

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

    m_output += "<" + element->getTagName();
    for(const auto& pair : attributes) { m_output += " " + pair.first + "=\"" + pair.second + "\""; }
    m_output += ">";

    for (const auto& child : element->getChildren()) {
        if (child->getType() == NodeType::TemplateUsage) {
            generateTemplateUsage(static_cast<const TemplateUsageNode*>(child.get()), element);
        } else {
            generateNode(child.get());
        }
    }

    m_output += "</" + element->getTagName() + ">";
}

void Generator::generateText(const TextNode* text) {
    m_output += text->getText();
}

} // namespace CHTL
