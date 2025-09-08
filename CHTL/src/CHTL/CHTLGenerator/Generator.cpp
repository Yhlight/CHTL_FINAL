#include "Generator.h"
#include "CHTL/CHTLNode/StyleBlockNode.h"
#include "CHTL/CHTLNode/StylePropertyNode.h"
#include "CHTL/CHTLNode/StyleSelectorNode.h"
#include "CHTL/CHTLNode/CustomUsageNode.h"
#include "CHTL/CHTLNode/DeleteRuleNode.h"
#include "CHTL/CHTLNode/InsertRuleNode.h"
#include "CHTL/CHTLNode/OriginNode.h"
#include "CHTL/CHTLNode/OriginUsageNode.h"
#include "CHTL/ExpressionNode/LiteralExpr.h"
#include "CHTL/Evaluator/Evaluator.h"
#include <stdexcept>
#include <iostream>
#include <map>
#include <vector>
#include <algorithm>
#include <regex>

namespace CHTL {

bool isLiteral(const Expr* expr) { if (!expr) return false; return dynamic_cast<const LiteralExpr*>(expr) != nullptr; }

std::string Generator::generate(const DocumentNode* document, const CHTLContext& context) {
    m_context = &context;
    m_root_node = document;
    m_output.clear();
    m_global_css.clear();
    if (document) { for (const auto& child : document->getChildren()) { generateNode(child.get()); } }
    if (!m_global_css.empty()) {
        size_t head_pos = m_output.find("</head>");
        if (head_pos != std::string::npos) { m_output.insert(head_pos, "<style>" + m_global_css + "</style>"); }
        else { m_output = "<head><style>" + m_global_css + "</style></head>" + m_output; }
    }
    return m_output;
}

void Generator::generateNode(const BaseNode* node) {
    if (!node) return;
    switch (node->getType()) {
        case NodeType::Element: generateElement(static_cast<const ElementNode*>(node)); break;
        case NodeType::Text: generateText(static_cast<const TextNode*>(node)); break;
        case NodeType::TemplateUsage: generateTemplateUsage(static_cast<const TemplateUsageNode*>(node), nullptr); break;
        case NodeType::CustomUsage: generateCustomUsage(static_cast<const CustomUsageNode*>(node)); break;
        case NodeType::Origin: generateOrigin(static_cast<const OriginNode*>(node)); break;
        case NodeType::OriginUsage: generateOriginUsage(static_cast<const OriginUsageNode*>(node)); break;
        default: break;
    }
}

void Generator::generateOriginUsage(const OriginUsageNode* node) {
    const OriginNode* def = m_context->getOrigin(CHTLContext::GLOBAL_NAMESPACE, node->getName());
     if (def) {
        generateOrigin(def);
    } else {
        m_output += "<!-- Named origin block not found: " + node->getName() + " -->";
    }
}

void Generator::generateOrigin(const OriginNode* node) {
    m_output += node->getContent();
}

void Generator::expandStyleProperties(const BaseNode* container, std::map<std::string, const StylePropertyNode*>& properties) {
    if (!container) return;
    const std::vector<std::unique_ptr<BaseNode>>* children = nullptr;
    if (container->getType() == NodeType::StyleBlock) children = &(static_cast<const StyleBlockNode*>(container)->getChildren());
    else if (container->getType() == NodeType::TemplateDefinition) children = &(static_cast<const TemplateDefinitionNode*>(container)->getChildren());
    else if (container->getType() == NodeType::CustomDefinition) children = &(static_cast<const CustomDefinitionNode*>(container)->getChildren());
    if (!children) return;
    for (const auto& child : *children) {
        if (child->getType() == NodeType::StyleProperty) {
            const auto* propNode = static_cast<const StylePropertyNode*>(child.get());
            properties[propNode->getKey()] = propNode;
        } else if (child->getType() == NodeType::TemplateUsage) {
            const auto* usageNode = static_cast<const TemplateUsageNode*>(child.get());
            if (usageNode->getTemplateType() == "Style") {
                std::string ns = usageNode->getNamespace().empty() ? CHTLContext::GLOBAL_NAMESPACE : usageNode->getNamespace();
                const TemplateDefinitionNode* baseTemplate = m_context->getTemplate(ns, usageNode->getTemplateName());
                expandStyleProperties(baseTemplate, properties);
            }
        } else if (child->getType() == NodeType::CustomUsage) {
            const auto* usageNode = static_cast<const CustomUsageNode*>(child.get());
            if (usageNode->getCustomType() == "Style") {
                std::string ns = usageNode->getNamespace().empty() ? CHTLContext::GLOBAL_NAMESPACE : usageNode->getNamespace();
                const CustomDefinitionNode* baseDef = m_context->getCustom(ns, usageNode->getCustomName());
                expandStyleProperties(baseDef, properties);
                for (const auto& rule : usageNode->getSpecializations()) {
                    if (rule->getType() == NodeType::DeleteRule) {
                        properties.erase(static_cast<const DeleteRuleNode*>(rule.get())->getTarget());
                    }
                }
            }
        }
    }
}

void Generator::generateTemplateUsage(const TemplateUsageNode* node, const ElementNode* parentElement) {
    std::string ns = node->getNamespace().empty() ? CHTLContext::GLOBAL_NAMESPACE : node->getNamespace();
    const TemplateDefinitionNode* def = m_context->getTemplate(ns, node->getTemplateName());
    if (!def) { m_output += "<!-- Template not found: " + node->getNamespace() + "::" + node->getTemplateName() + " -->"; return; }
    if (node->getTemplateType() == "Element") {
        for (const auto& child : def->getChildren()) { generateNode(child.get()); }
    }
}

std::vector<std::unique_ptr<BaseNode>>::iterator
Generator::findInsertionPoint(const std::string& selector, std::vector<std::unique_ptr<BaseNode>>& nodes) {
    std::regex re("(\\w+)\\[(\\d+)\\]"); std::smatch match;
    if (std::regex_match(selector, match, re) && match.size() == 3) {
        std::string tag = match[1].str(); int index = std::stoi(match[2].str()); int count = 0;
        for (auto it = nodes.begin(); it != nodes.end(); ++it) {
            if ((*it)->getType() == NodeType::Element && static_cast<ElementNode*>((*it).get())->getTagName() == tag) {
                if (count == index) return it;
                count++;
            }
        }
    } else {
         for (auto it = nodes.begin(); it != nodes.end(); ++it) {
            if ((*it)->getType() == NodeType::Element && static_cast<ElementNode*>((*it).get())->getTagName() == selector) { return it; }
        }
    }
    return nodes.end();
}

void Generator::generateCustomUsage(const CustomUsageNode* usageNode) {
    std::string ns = usageNode->getNamespace().empty() ? CHTLContext::GLOBAL_NAMESPACE : usageNode->getNamespace();
    const CustomDefinitionNode* def = m_context->getCustom(ns, usageNode->getCustomName());
    if (!def) { m_output += "<!-- Custom definition not found: " + usageNode->getNamespace() + "::" + usageNode->getCustomName() + " -->"; return; }
    std::vector<std::unique_ptr<BaseNode>> mutable_ast;
    for (const auto& child : def->getChildren()) { mutable_ast.push_back(child->clone()); }
    for (const auto& rule : usageNode->getSpecializations()) {
        if (rule->getType() == NodeType::DeleteRule) {
            const auto* deleteRule = static_cast<const DeleteRuleNode*>(rule.get());
            mutable_ast.erase(std::remove_if(mutable_ast.begin(), mutable_ast.end(),
                [&](const auto& n){ return n->getType() == NodeType::Element && static_cast<ElementNode*>(n.get())->getTagName() == deleteRule->getTarget(); }), mutable_ast.end());
        }
        else if (rule->getType() == NodeType::InsertRule) {
            const auto* insertRule = static_cast<const InsertRuleNode*>(rule.get());
            auto target_it = findInsertionPoint(insertRule->getTarget(), mutable_ast);
            std::vector<std::unique_ptr<BaseNode>> content_to_insert;
            for(const auto& n : insertRule->getContent()) { content_to_insert.push_back(n->clone()); }
            if (insertRule->getMode() == "at top") mutable_ast.insert(mutable_ast.begin(), std::make_move_iterator(content_to_insert.begin()), std::make_move_iterator(content_to_insert.end()));
            else if (insertRule->getMode() == "at bottom") mutable_ast.insert(mutable_ast.end(), std::make_move_iterator(content_to_insert.begin()), std::make_move_iterator(content_to_insert.end()));
            else if (target_it != mutable_ast.end()) {
                if (insertRule->getMode() == "before") mutable_ast.insert(target_it, std::make_move_iterator(content_to_insert.begin()), std::make_move_iterator(content_to_insert.end()));
                else if (insertRule->getMode() == "after") mutable_ast.insert(std::next(target_it), std::make_move_iterator(content_to_insert.begin()), std::make_move_iterator(content_to_insert.end()));
                else if (insertRule->getMode() == "replace") { target_it = mutable_ast.erase(target_it); mutable_ast.insert(target_it, std::make_move_iterator(content_to_insert.begin()), std::make_move_iterator(content_to_insert.end())); }
            }
        }
    }
    for (const auto& node : mutable_ast) { generateNode(node.get()); }
}

const ElementNode* Generator::findNodeBySelector(const BaseNode* searchRoot, const std::string& selector) const {
    if (!searchRoot) return nullptr;

    if (searchRoot->getType() == NodeType::Element) {
        const auto* el = static_cast<const ElementNode*>(searchRoot);
        if (selector[0] == '#') {
            std::string id = selector.substr(1);
            for(const auto& attr : el->getAttributes()) {
                if(attr->getKey() == "id" && attr->getValue() == id) return el;
            }
        }
    }

    const std::vector<std::unique_ptr<BaseNode>>* children = nullptr;
    if (searchRoot->getType() == NodeType::Document) children = &static_cast<const DocumentNode*>(searchRoot)->getChildren();
    else if (searchRoot->getType() == NodeType::Element) children = &static_cast<const ElementNode*>(searchRoot)->getChildren();

    if (children) {
        for (const auto& child : *children) {
            const ElementNode* found = findNodeBySelector(child.get(), selector);
            if (found) return found;
        }
    }

    return nullptr;
}


void Generator::generateElement(const ElementNode* element) {
    std::map<std::string, std::string> attributes;
    for (const auto& attr : element->getAttributes()) { attributes[attr->getKey()] = attr->getValue(); }
    if (element->getStyleBlock()) {
        std::map<std::string, const StylePropertyNode*> final_properties;
        expandStyleProperties(element->getStyleBlock(), final_properties);

        EvaluationContext eval_context;
        NodeResolver resolver = [this](const std::string& selector) {
            return this->findNodeBySelector(this->m_root_node, selector);
        };

        for(const auto& pair : final_properties) { if(isLiteral(pair.second->getValue())) { Evaluator eval; eval_context[pair.first] = eval.evaluate(pair.second->getValue(), *m_context, eval_context, resolver); } }
        std::string inline_style;
        for(const auto& pair : final_properties) { Evaluator eval; ChtlValue value = eval.evaluate(pair.second->getValue(), *m_context, eval_context, resolver); inline_style += pair.first + ": " + value.toString() + ";"; }
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
                for (const auto& prop : selectorNode->getProperties()) { Evaluator eval; ChtlValue value = eval.evaluate(prop->getValue(), *m_context, eval_context, resolver); m_global_css += prop->getKey() + ": " + value.toString() + "; "; }
                m_global_css += "}\n";
            }
        }
    }
    m_output += "<" + element->getTagName();
    for(const auto& pair : attributes) { m_output += " " + pair.first + "=\"" + pair.second + "\""; }
    m_output += ">";
    for (const auto& child : element->getChildren()) {
        generateNode(child.get());
    }
    m_output += "</" + element->getTagName() + ">";
}

void Generator::generateText(const TextNode* text) { m_output += text->getText(); }

} // namespace CHTL
