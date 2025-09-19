#include "CHTLGenerator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../Expression/ExpressionEvaluator.h"
#include <unordered_set>
#include <algorithm>
#include <map>

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
        // We use a two-pass approach.
        // 1. First pass collects all non-inline CSS rules (e.g. from class selectors)
        //    and generates the global stylesheet. This is necessary because style
        //    blocks can appear anywhere in the document.
        ExpressionEvaluator evaluator(this->templates, this->doc_root);
        collectAndGenerateCss(root, evaluator);

        // 2. Second pass generates the HTML structure and inline styles.
        root->accept(*this);
    }
    return {html_output.str(), css_output.str()};
}

void CHTLGenerator::collectAndGenerateCss(BaseNode* node, ExpressionEvaluator& evaluator) {
    if (!node) return;

    if (StyleNode* styleNode = dynamic_cast<StyleNode*>(node)) {
        // Process global rules defined in this style block
        for (const auto& rule : styleNode->global_rules) {
            css_output << rule.selector << " {\n";
            for (const auto& prop : rule.properties) {
                // We pass nullptr for context because global rules can't reference element properties
                PropertyValue result = evaluator.evaluate(prop.value_expr.get(), nullptr);
                css_output << "    " << prop.key << ": " << result.toString() << ";\n";
            }
            css_output << "}\n";
        }
    }

    if (ElementNode* elementNode = dynamic_cast<ElementNode*>(node)) {
        for (const auto& child : elementNode->children) {
            collectAndGenerateCss(child.get(), evaluator);
        }
    }
}


void CHTLGenerator::visit(ElementNode& node) {
    html_output << "<" << node.tagName;
    std::string text_content;

    // Process standard attributes
    ExpressionEvaluator attr_evaluator(this->templates, this->doc_root);
    for (const auto& attr : node.attributes) {
        PropertyValue result = attr_evaluator.evaluate(attr.value_expr.get(), &node);
        if (attr.key == "text") {
            text_content = result.toString();
        } else {
            html_output << " " << attr.key << "=\"" << result.toString() << "\"";
        }
    }

    // Process inline styles
    std::string style_str;
    ExpressionEvaluator evaluator(this->templates, this->doc_root);
    for (const auto& child : node.children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            // This logic for applying templates is simplified and may need review
            std::map<std::string, AttributeNode> final_props;
            for (const auto& app : styleNode->template_applications) {
                const TemplateDefinitionNode* def = nullptr;
                for (const auto& ns_pair : this->templates) {
                    if (ns_pair.second.count(app.template_name)) {
                        def = &ns_pair.second.at(app.template_name);
                        break;
                    }
                }
                if (def && def->type == TemplateType::STYLE) {
                    for (const auto& prop : def->style_properties) { final_props[prop.key] = prop.clone(); }
                    for (const auto& key_to_delete : app.deleted_properties) { final_props.erase(key_to_delete); }
                    for (const auto& prop : app.new_or_overridden_properties) { final_props[prop.key] = prop.clone(); }
                }
            }
            // Direct properties override any template properties
            for (const auto& prop : styleNode->direct_properties) {
                final_props[prop.key] = prop.clone();
            }

            // Evaluate and append the final properties
            for (const auto& pair : final_props) {
                PropertyValue result = evaluator.evaluate(pair.second.value_expr.get(), &node);
                style_str += pair.first + ": " + result.toString() + ";";
            }
        }
    }
    if (!style_str.empty()) {
        html_output << " style=\"" << style_str << "\"";
    }

    // Handle void elements
    if (voidElements.count(node.tagName)) {
        html_output << ">";
        return;
    }

    // Process children and closing tag
    html_output << ">";
    if (!text_content.empty()) {
        html_output << text_content;
    }
    for (const auto& child : node.children) {
        // Style nodes are processed for CSS, not rendered as HTML
        if (dynamic_cast<StyleNode*>(child.get())) continue;
        child->accept(*this);
    }
    html_output << "</" << node.tagName << ">";
}

void CHTLGenerator::visit(TextNode& node) {
    html_output << node.text;
}

void CHTLGenerator::visit(StyleNode& node) {
    // This is now handled by the initial `collectAndGenerateCss` pass.
    // This visit function is for the HTML-generation pass, where styles are ignored.
}

void CHTLGenerator::visit(OriginNode& node) {
    if (node.type == OriginType::HTML) {
        html_output << node.content;
    } else if (node.type == OriginType::STYLE) {
        // Origin CSS is now also collected in the first pass
        css_output << node.content;
    }
}

} // namespace CHTL
