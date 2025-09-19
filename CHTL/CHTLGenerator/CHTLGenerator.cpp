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
        root->accept(*this);
    }
    return {html_output.str(), css_output.str()};
}

void CHTLGenerator::visit(ElementNode& node) {
    // --- Attribute and Style Processing ---
    std::map<std::string, std::string> attributes;
    for (const auto& attr : node.attributes) {
        attributes[attr.key] = attr.value;
    }

    std::vector<std::string> classes_to_add;
    std::string id_to_add;
    std::string style_str;

    for (const auto& child : node.children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            // 1. Process global rules for CSS output and collect classes/IDs
            for (const auto& rule : styleNode->global_rules) {
                css_output << rule.selector << " {";
                for (const auto& prop : rule.properties) {
                    ExpressionEvaluator evaluator(this->templates, this->doc_root);
                    EvaluatedValue result = evaluator.evaluate(prop.value_expr.get(), &node);
                    css_output << prop.key << ":";
                     if (result.value == 0 && !result.unit.empty()) {
                        css_output << result.unit;
                    } else {
                        if (result.value == static_cast<long long>(result.value)) {
                            css_output << std::to_string(static_cast<long long>(result.value));
                        } else {
                            css_output << std::to_string(result.value);
                        }
                        css_output << result.unit;
                    }
                    css_output << ";";
                }
                css_output << "}";

                if (!rule.selector.empty()) {
                    if (rule.selector[0] == '.') {
                        classes_to_add.push_back(rule.selector.substr(1));
                    } else if (rule.selector[0] == '#') {
                        if (id_to_add.empty()) {
                            id_to_add = rule.selector.substr(1);
                        }
                    }
                }
            }

            // 2. Process inline styles
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
            for (const auto& prop : styleNode->direct_properties) {
                final_props[prop.key] = prop.clone();
            }
            for (const auto& pair : final_props) {
                ExpressionEvaluator evaluator(this->templates, this->doc_root);
                EvaluatedValue result = evaluator.evaluate(pair.second.value_expr.get(), &node);
                style_str += pair.first + ":";
                if (result.value == 0 && !result.unit.empty()) {
                    style_str += result.unit;
                } else {
                    if (result.value == static_cast<long long>(result.value)) {
                        style_str += std::to_string(static_cast<long long>(result.value));
                    } else {
                        style_str += std::to_string(result.value);
                    }
                    style_str += result.unit;
                }
                style_str += ";";
            }
        }
    }

    // --- Merge and Finalize Attributes ---
    if (!id_to_add.empty() && attributes.find("id") == attributes.end()) {
        attributes["id"] = id_to_add;
    }

    if (!classes_to_add.empty()) {
        if (attributes.count("class")) {
            for (const auto& new_class : classes_to_add) {
                attributes["class"] += " " + new_class;
            }
        } else {
            std::string class_str;
            for (size_t i = 0; i < classes_to_add.size(); ++i) {
                class_str += classes_to_add[i] + (i == classes_to_add.size() - 1 ? "" : " ");
            }
            attributes["class"] = class_str;
        }
    }

    if (!style_str.empty()) {
        if (attributes.count("style")) {
            attributes["style"] += style_str;
        } else {
            attributes["style"] = style_str;
        }
    }

    // --- HTML Tag Generation ---
    html_output << "<" << node.tagName;
    std::string text_content;
    if (attributes.count("text")) {
        text_content = attributes["text"];
        attributes.erase("text");
    }

    for (const auto& attr_pair : attributes) {
        html_output << " " << attr_pair.first << "=\"" << attr_pair.second << "\"";
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

} // namespace CHTL
