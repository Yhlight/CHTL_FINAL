#include "CHTLGenerator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../Expression/ExpressionEvaluator.h"
#include <unordered_set>
#include <algorithm>
#include <map>

namespace CHTL {

void collectStyleProperties(std::map<std::string, AttributeNode>& properties, const TemplateDefinitionNode* def, const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& all_templates, const StyleNode::StyleApplication& app) {
    if (!def) return;

    // Apply parent templates first (recursively)
    for (const auto& parent_name : def->inherited_templates) {
        // Check if this inherited template is deleted
        bool is_deleted = false;
        for (const auto& deleted_name : app.deleted_templates) {
            if (parent_name == deleted_name) {
                is_deleted = true;
                break;
            }
        }
        if (is_deleted) continue;

        const TemplateDefinitionNode* parent_def = nullptr;
        for (const auto& ns_pair : all_templates) {
            if (ns_pair.second.count(parent_name)) {
                parent_def = &ns_pair.second.at(parent_name);
                break;
            }
        }
        collectStyleProperties(properties, parent_def, all_templates, app);
    }

    // Apply this template's properties (overwriting parent's)
    for (const auto& prop : def->style_properties) {
        properties[prop.key] = prop.clone();
    }
}


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
            for (const auto& app : styleNode->template_applications) {
                const TemplateDefinitionNode* def = nullptr;
                std::string ns_name;
                std::string tpl_name = app.template_name;
                size_t dot_pos = app.template_name.find('.');
                if (dot_pos != std::string::npos) {
                    ns_name = app.template_name.substr(0, dot_pos);
                    tpl_name = app.template_name.substr(dot_pos + 1);
                }

                if (!ns_name.empty()) {
                    if (this->templates.count(ns_name) && this->templates.at(ns_name).count(tpl_name)) {
                        def = &this->templates.at(ns_name).at(tpl_name);
                    }
                } else {
                    for (const auto& ns_pair : this->templates) {
                        if (ns_pair.second.count(tpl_name)) {
                            def = &ns_pair.second.at(tpl_name);
                            break;
                        }
                    }
                }

                if (def && def->type == TemplateType::STYLE) {
                    collectStyleProperties(final_props, def, this->templates, app);
                    for (const auto& key_to_delete : app.deleted_properties) { final_props.erase(key_to_delete); }
                    for (const auto& prop : app.new_or_overridden_properties) { final_props[prop.key] = prop.clone(); }
                }
            }
            for (const auto& prop : styleNode->direct_properties) {
                final_props[prop.key] = prop.clone();
            }
            for (const auto& pair : final_props) {
                if (pair.second.value_expr) { // Check if the expression is not null
                    ExpressionEvaluator evaluator(this->templates, this->doc_root);
                    EvaluatedValue result = evaluator.evaluate(pair.second.value_expr.get(), &node);
                    style_str += pair.first + ": ";
                    if (result.value == 0 && !result.unit.empty()) { style_str += result.unit; }
                    else { style_str += std::to_string(result.value) + result.unit; }
                    style_str += ";";
                }
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

void CHTLGenerator::visit(TemplateUsageNode& node) {
    const TemplateDefinitionNode* def = nullptr;
    for (const auto& ns_pair : this->templates) {
        if (ns_pair.second.count(node.name)) {
            def = &ns_pair.second.at(node.name);
            break;
        }
    }

    if (def) {
        if (def->type == TemplateType::ELEMENT) {
            for (const auto& child : def->element_body) {
                child->accept(*this);
            }
        }
        // @Style and @Var usages are handled elsewhere (in style evaluation and expression evaluation)
    } else {
        // Should we throw an error here? The parser should have caught this.
        // For now, do nothing.
    }
}

} // namespace CHTL
