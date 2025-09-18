#include "CHTLGenerator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/CssRuleNode.h"
#include "../CHTLNode/DocumentNode.h"
#include <unordered_set>
#include <algorithm>
#include <iostream>

namespace CHTL {

// Helper set for HTML void elements
const std::unordered_set<std::string> voidElements = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

CHTLGenerator::CHTLGenerator(const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates)
    : m_templates(templates), m_doc_root(nullptr) {
    // The evaluator is initialized later, once we have the doc_root
}

CompilationResult CHTLGenerator::generate(BaseNode* root) {
    m_html_output.str("");
    m_css_output.str("");
    m_doc_root = root;
    m_evaluator = std::make_unique<ExpressionEvaluator>(m_templates, m_doc_root);

    if (root) {
        root->accept(*this);
    }
    return {m_html_output.str(), m_css_output.str()};
}

void CHTLGenerator::indent(int level) {
    for (int i = 0; i < level; ++i) {
        m_html_output << "  ";
    }
}

void CHTLGenerator::visit(ElementNode& node) {
    // --- Pre-processing Step: Generate CSS from child StyleNodes ---
    // The visitor pattern is tricky for context. We process styles for the current
    // element *before* we generate its HTML.
    std::string inline_style_str;
    for (const auto& child : node.children) {
        if (auto* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            // Generate inline styles
            for (const auto& prop : styleNode->inline_properties) {
                Value val = m_evaluator->evaluate(prop.value_expr.get(), &node);
                inline_style_str += prop.key + ": " + val.toString() + "; ";
            }
            // Generate global rules
            for (const auto& rule : styleNode->global_rules) {
                // This is a simplified approach. A real implementation would
                // need to handle selector transformations (&, etc.)
                m_css_output << rule.selector << " {\n";
                for (const auto& prop : rule.properties) {
                    Value val = m_evaluator->evaluate(prop.value_expr.get(), &node);
                    m_css_output << "    " << prop.key << ": " << val.toString() << ";\n";
                }
                m_css_output << "}\n\n";
            }
        }
    }

    // --- HTML Generation Step ---
    indent(m_indent_level);
    m_html_output << "<" << node.tagName;

    // Add attributes
    for (const auto& attr : node.attributes) {
        m_html_output << " " << attr.key << "=\"" << attr.value << "\"";
    }
    // Add the collected inline styles
    if (!inline_style_str.empty()) {
        m_html_output << " style=\"" << inline_style_str << "\"";
    }

    if (voidElements.count(node.tagName)) {
        m_html_output << ">\n";
        return;
    }

    m_html_output << ">\n";

    // Recursively visit children that are not style nodes
    m_indent_level++;
    for (const auto& child : node.children) {
        if (dynamic_cast<StyleNode*>(child.get()) == nullptr) {
            child->accept(*this);
        }
    }
    m_indent_level--;

    indent(m_indent_level);
    m_html_output << "</" << node.tagName << ">\n";
}

void CHTLGenerator::visit(TextNode& node) {
    indent(m_indent_level);
    m_html_output << node.text << "\n";
}

void CHTLGenerator::visit(StyleNode& node) {
    // This is intentionally left empty.
    // The parent ElementNode's visit method handles processing its child StyleNodes
    // to correctly apply styles to the parent element.
}

void CHTLGenerator::visit(CssRuleNode& node) {
    // This is also intentionally left empty for the same reason as StyleNode.
    // The logic is handled in the parent's context.
}

void CHTLGenerator::visit(OriginNode& node) {
    indent(m_indent_level);
    switch (node.type) {
        case OriginType::HTML: m_html_output << node.content << "\n"; break;
        case OriginType::STYLE: m_css_output << node.content << "\n"; break;
        case OriginType::JAVASCRIPT: /* Not handled yet */ break;
    }
}

void CHTLGenerator::visit(DocumentNode& node) {
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }
}

} // namespace CHTL
