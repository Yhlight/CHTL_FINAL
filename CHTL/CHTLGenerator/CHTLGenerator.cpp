#include "CHTLGenerator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../Expression/ExpressionEvaluator.h"
#include "../Util/StyleResolver.h"
#include <unordered_set>
#include <algorithm>
#include <map>
#include <vector>

namespace CHTL {

const std::unordered_set<std::string> voidElements = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

CHTLGenerator::CHTLGenerator(const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates)
    : templates(templates), doc_root(nullptr) {}

CompilationResult CHTLGenerator::generate(BaseNode* root) {
    html_output.str("");
    css_output.str("");
    this->doc_root = root;
    if (root) {
        root->accept(*this);
    }
    return {html_output.str(), css_output.str()};
}

void CHTLGenerator::visit(ElementNode& node) {
    // --- Global Style Generation ---
    for (const auto& child : node.children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            for (const auto& rule : styleNode->global_rules) {
                // ... (logic for global rules)
            }
        }
    }

    // --- HTML Tag Generation ---
    html_output << "<" << node.tagName;
    std::string text_content;
    for (const auto& attr : node.attributes) {
        if (attr.key == "text") {
            text_content = attr.value;
        } else {
            html_output << " " << attr.key << "=\"" << attr.value << "\"";
        }
    }

    // --- Inline Style Generation ---
    std::string style_str;
    for (const auto& child : node.children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            std::map<std::string, AttributeNode> final_props;
            StyleResolver::resolveStyleNode(styleNode, this->templates, final_props);

            // Evaluate all properties
            for (const auto& pair : final_props) {
                ExpressionEvaluator evaluator(&final_props, this->templates, this->doc_root);
                EvaluatedValue result = evaluator.evaluate(pair.second.value_expr.get(), &node);
                style_str += pair.first + ": ";
                if (result.value == 0 && !result.unit.empty() && result.unit != "0") {
                     style_str += result.unit;
                } else if (result.value == static_cast<int>(result.value)) {
                    style_str += std::to_string(static_cast<int>(result.value)) + result.unit;
                } else {
                    style_str += std::to_string(result.value) + result.unit;
                }
                style_str += "; ";
            }
        }
    }
    if (!style_str.empty()) {
        html_output << " style=\"" << style_str << "\"";
    }

    // --- Child and Closing Tag Generation ---
    if (voidElements.count(node.tagName)) {
        html_output << ">";
        return;
    }
    html_output << ">";
    if (!text_content.empty()) {
        html_output << text_content;
    }
    for (const auto& child : node.children) {
        if (dynamic_cast<StyleNode*>(child.get())) continue;
        child->accept(*this);
    }
    html_output << "</" << node.tagName << ">";
}

void CHTLGenerator::visit(TextNode& node) { html_output << node.text; }
void CHTLGenerator::visit(StyleNode& node) {} // Handled inside ElementNode visit
void CHTLGenerator::visit(OriginNode& node) {
    if (node.type == OriginType::HTML) html_output << node.content;
    else if (node.type == OriginType::STYLE) css_output << node.content;
}

void CHTLGenerator::visit(ConfigNode& node) {
    // Configuration nodes do not produce any output, they are handled pre-parse.
}

} // namespace CHTL
