#include "CHTLGenerator.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/CommentNode.h"
#include "CHTL/CHTLNode/StyleNode.h"
#include "CHTL/CHTLNode/TemplateUsageNode.h"
#include "CHTL/CHTLNode/ModificationNode.h"
#include <stdexcept>
#include <unordered_set>
#include <algorithm>
#include <map>
#include <vector>

namespace CHTL {

const std::unordered_set<std::string> voidElements = { "area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "param", "source", "track", "wbr" };

// --- visitStyleNode and its helpers ---
void resolveStyleTemplate(const std::string& name, const CHTLContext& context, std::map<std::string, std::string>& properties) {
    const StyleTemplate* tpl = context.getStyleTemplate(name);
    if (!tpl) return;
    for (const auto& prop : tpl->properties) { properties[prop.first] = prop.second; }
}

void CHTLGenerator::visitStyleNode(const StyleNode& node, ElementNode& parent) {
    std::map<std::string, std::string> resolved_properties;
    for (const auto& rule : node.getRules()) {
        if (rule.selector == "@Style") {
            const std::string& tplName = rule.properties.at(0).second;
            resolveStyleTemplate(tplName, m_context, resolved_properties);
        } else if (rule.selector.empty()) {
            for (const auto& prop : rule.properties) {
                resolved_properties[prop.first] = prop.second;
            }
        }
    }
    std::string inline_style;
    for (const auto& prop : resolved_properties) {
        inline_style += prop.first + ":" + prop.second + ";";
    }
    if (!inline_style.empty()) {
        parent.setAttribute("style", (parent.getAttributes().count("style") ? parent.getAttributes().at("style") : "") + inline_style);
    }
}


CHTLGenerator::CHTLGenerator(CHTLContext& context) : m_context(context), m_indentLevel(0) {}

std::string CHTLGenerator::generate(BaseNode& root) {
    m_output.clear();
    m_globalCss.clear();
    m_indentLevel = -1;
    visit(root);
    if (!m_globalCss.empty()) {
        size_t head_pos = m_output.find("</head>");
        if (head_pos != std::string::npos) {
            m_output.insert(head_pos, "  <style>\n" + m_globalCss + "  </style>\n");
        }
    }
    return m_output;
}

std::string CHTLGenerator::getIndent() {
    if (m_indentLevel < 0) return "";
    return std::string(m_indentLevel * 2, ' ');
}

void CHTLGenerator::visit(BaseNode& node) {
    switch (node.getType()) {
        case NodeType::Element: visitElement(static_cast<ElementNode&>(node)); break;
        case NodeType::Text: visitText(static_cast<const TextNode&>(node)); break;
        case NodeType::Comment: visitComment(static_cast<const CommentNode&>(node)); break;
        case NodeType::TemplateUsage: visitTemplateUsage(static_cast<TemplateUsageNode&>(node)); break;
        default: break;
    }
}

void CHTLGenerator::visitTemplateUsage(TemplateUsageNode& node) {
    if (node.getTemplateType() == TemplateType::Element) {
        const ElementTemplate* tpl = m_context.getElementTemplate(node.getTemplateName());
        if (tpl) {
            for (auto& child : tpl->nodes) {
                auto cloned_child = child->clone();
                visit(*cloned_child);
            }
        }
    }
}

void CHTLGenerator::visitElement(ElementNode& node) {
    if (node.getTagName() == "_root_") {
        m_indentLevel++;
        for (auto& child : const_cast<std::vector<std::unique_ptr<BaseNode>>&>(node.getChildren())) { visit(*child); }
        m_indentLevel--;
        return;
    }
    for (const auto& child : node.getChildren()) {
        if (child->getType() == NodeType::Style) {
            visitStyleNode(static_cast<const StyleNode&>(*child), node);
        }
    }
    m_output += getIndent() + "<" + node.getTagName();
    for (const auto& attr : node.getAttributes()) {
        m_output += " " + attr.first + "=\"" + attr.second + "\"";
    }
    bool has_content = std::any_of(node.getChildren().begin(), node.getChildren().end(), [](const auto& child){ return child->getType() != NodeType::Style; });
    if (!has_content && voidElements.count(node.getTagName())) {
        m_output += ">\n";
        return;
    }
    m_output += ">\n";
    m_indentLevel++;
    for (auto& child : const_cast<std::vector<std::unique_ptr<BaseNode>>&>(node.getChildren())) {
        if (child->getType() != NodeType::Style) {
            visit(*child);
        }
    }
    m_indentLevel--;
    m_output += getIndent() + "</" + node.getTagName() + ">\n";
}

void CHTLGenerator::visitText(const TextNode& node) {
    m_output += getIndent() + node.getContent() + "\n";
}

void CHTLGenerator::visitComment(const CommentNode& node) {
    if (node.isGeneratorComment()) {
        m_output += getIndent() + "<!-- " + node.getContent() + " -->\n";
    }
}

} // namespace CHTL
