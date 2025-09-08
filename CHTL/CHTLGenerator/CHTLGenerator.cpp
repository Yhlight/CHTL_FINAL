#include "CHTLGenerator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/ModificationNode.h"
#include <stdexcept>
#include <unordered_set>
#include <algorithm>
#include <map>
#include <vector>

namespace CHTL {

const std::unordered_set<std::string> voidElements = { "area", "base", "br", "col", "embed", "hr", "img", "input", "link", "meta", "param", "source", "track", "wbr" };

void resolveStyleTemplate(const std::string& name, const CHTLContext& context, std::map<std::string, std::string>& properties) { /* ... */ }
void resolveCustomStyleTemplate(const std::string& name, const CHTLContext& context, std::map<std::string, std::string>& properties) { /* ... */ }
ElementNode* findNodeBySelector(std::vector<std::unique_ptr<BaseNode>>& nodes, const std::string& selector) { return nullptr; }


// Corrected the typo CHTLLContext -> CHTLContext
CHTLGenerator::CHTLGenerator(CHTLContext& context) : m_context(context), m_indentLevel(0) {}

std::string CHTLGenerator::generate(BaseNode& root) {
    m_output.clear();
    m_globalCss.clear();
    m_indentLevel = -1;
    visit(root);
    size_t head_pos = m_output.find("</head>");
    if (head_pos != std::string::npos && !m_globalCss.empty()) {
        m_output.insert(head_pos, "  <style>\n" + m_globalCss + "  </style>\n");
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
        case NodeType::Style: break;
        default: break;
    }
}

void CHTLGenerator::visitTemplateUsage(TemplateUsageNode& node) {
    // ... implementation ...
}

void CHTLGenerator::visitStyleNode(const StyleNode& node, ElementNode& parent) {
    // ... implementation ...
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
