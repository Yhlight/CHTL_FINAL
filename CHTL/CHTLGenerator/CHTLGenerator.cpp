#include "CHTLGenerator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../../CHTL JS/CHTLJSLexer/CHTLJSLexer.h"
#include "../../CHTL JS/CHTLJSParser/CHTLJSParser.h"
#include "../../CHTL JS/CHTLJSNode/RawJSNode.h"
#include "../../CHTL JS/CHTLJSNode/EnhancedSelectorNode.h"
#include "../../CHTL JS/CHTLJSNode/ListenNode.h"
#include "../../CHTL JS/CHTLJSNode/EventHandlerNode.h"
#include "../../CHTL JS/CHTLJSNode/DelegateNode.h"
#include "../../CHTL JS/CHTLJSNode/AnimateNode.h"
#include "../Expression/ExpressionEvaluator.h"
#include <unordered_set>
#include <algorithm>
#include <iostream>
#include <map>
#include <sstream>

namespace CHTL {

// Helper to format doubles cleanly for CSS output
std::string format_css_double(double val) {
    std::ostringstream oss;
    oss << val;
    return oss.str();
}

const std::unordered_set<std::string> voidElements = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

// --- JS Generation for Dynamic Expressions ---
// This is a simplified helper. A real implementation would be much more complex.
std::string generateDynamicJS(
    const std::string& target_element_id,
    const std::string& css_property,
    const std::string& source_selector,
    const std::string& source_property,
    const std::string& condition_operator,
    const std::string& condition_value,
    const std::string& true_value,
    const std::string& false_value
) {
    std::stringstream js;
    js << "{\n";
    js << "  const source = document.querySelector('" << source_selector << "');\n";
    js << "  const target = document.getElementById('" << target_element_id << "');\n";
    js << "  const updateStyle = () => {\n";
    js << "    const sourceValue = parseFloat(window.getComputedStyle(source)." << source_property << ");\n";
    js << "    if (sourceValue " << condition_operator << " " << condition_value << ") {\n";
    js << "      target.style." << css_property << " = '" << true_value << "';\n";
    js << "    } else {\n";
    js << "      target.style." << css_property << " = '" << false_value << "';\n";
    js << "    }\n";
    js << "  };\n";
    js << "  const observer = new MutationObserver(updateStyle);\n";
    js << "  observer.observe(source, { attributes: true, attributeFilter: ['style'] });\n";
    js << "  updateStyle(); // Initial update\n";
    js << "}\n";
    return js.str();
}

std::map<std::string, AttributeNode> CHTLGenerator::resolveStyleTemplate(const StyleNode::StyleApplication& app) {
    std::map<std::string, AttributeNode> resolved_props;

    // 1. Find the definition for the current application
    const TemplateDefinitionNode* def = nullptr;
    for (const auto& ns_pair : this->templates) {
        if (ns_pair.second.count(app.template_name)) {
            def = &ns_pair.second.at(app.template_name);
            break;
        }
    }

    if (!def || def->type != TemplateType::STYLE) {
        return resolved_props;
    }

    // 2. Recursively resolve inherited templates
    for (const auto& inherited_template_name : def->inherited_style_templates) {
        StyleNode::StyleApplication inherited_app;
        inherited_app.template_name = inherited_template_name;
        std::map<std::string, AttributeNode> inherited_props = resolveStyleTemplate(inherited_app);
        for (auto const& [key, val] : inherited_props) {
            resolved_props[key] = val.clone();
        }
    }

    // 3. Add properties from the current template definition (will override inherited)
    for (const auto& prop : def->style_properties) {
        resolved_props[prop.key] = prop.clone();
    }

    // 4. Apply specializations from the application site
    for (const auto& key_to_delete : app.deleted_properties) {
        resolved_props.erase(key_to_delete);
    }
    for (const auto& prop : app.new_or_overridden_properties) {
        resolved_props[prop.key] = prop.clone();
    }

    return resolved_props;
}

CHTLGenerator::CHTLGenerator(const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates, std::shared_ptr<Configuration> config)
    : templates(templates), config(config), doc_root(nullptr) {}

CompilationResult CHTLGenerator::generate(BaseNode* root, bool use_html5_doctype) {
    html_output.str("");
    css_output.str("");
    js_output.str("");
    delegate_registry.clear();
    this->doc_root = root;
    if (root) {
        root->accept(*this);
    }

    for (const auto& pair : delegate_registry) {
        const std::string& parent_selector_str = pair.first;
        const auto& delegate_nodes = pair.second;

        js_output << "document.querySelector('" << parent_selector_str << "').addEventListener('click', (event) => {\n";
        for (const auto& delegate_node : delegate_nodes) {
            for (const auto& target : delegate_node.target_selectors) {
                 js_output << "  if (event.target.matches('" << target.selector_string << "')) {\n";
                 for (const auto& event : delegate_node.events) {
                     js_output << "    (" << event.second << ")(event);\n";
                 }
                 js_output << "  }\n";
            }
        }
        js_output << "});\n";
    }

    std::string final_html = use_html5_doctype ? "<!DOCTYPE html>\n" : "";
    final_html += html_output.str();

    return {final_html, css_output.str(), js_output.str()};
}

void CHTLGenerator::visit(ElementNode& node) {
    std::string primary_selector_for_ampersand;
    for (const auto& child : node.children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            bool class_attr_exists = std::any_of(node.attributes.begin(), node.attributes.end(),
                                                 [](const HtmlAttribute& attr){ return attr.key == "class"; });
            bool id_attr_exists = std::any_of(node.attributes.begin(), node.attributes.end(),
                                              [](const HtmlAttribute& attr){ return attr.key == "id"; });

            if (!class_attr_exists) {
                for (const auto& rule : styleNode->global_rules) {
                    if (!rule.selector.empty() && rule.selector[0] == '.') {
                        node.addAttribute({"class", rule.selector.substr(1)});
                        break;
                    }
                }
            }
            if (!id_attr_exists) {
                 for (const auto& rule : styleNode->global_rules) {
                    if (!rule.selector.empty() && rule.selector[0] == '#') {
                        node.addAttribute({"id", rule.selector.substr(1)});
                        break;
                    }
                }
            }

            for (const auto& attr : node.attributes) {
                if (attr.key == "class") { primary_selector_for_ampersand = "." + attr.value; break; }
                if (attr.key == "id") { primary_selector_for_ampersand = "#" + attr.value; }
            }
            if (primary_selector_for_ampersand.empty()) {
                 for (const auto& rule : styleNode->global_rules) {
                     if(!rule.selector.empty() && rule.selector[0] == '.') { primary_selector_for_ampersand = rule.selector; break;}
                     if(!rule.selector.empty() && rule.selector[0] == '#') { primary_selector_for_ampersand = rule.selector; }
                 }
            }

            for (const auto& rule : styleNode->global_rules) {
                std::string final_selector = rule.selector;
                if (!rule.selector.empty() && rule.selector[0] == '&') {
                    if (!primary_selector_for_ampersand.empty()) {
                        final_selector = primary_selector_for_ampersand + rule.selector.substr(1);
                    }
                }
                css_output << final_selector << " {\n";
                for (const auto& prop : rule.properties) {
                    ExpressionEvaluator evaluator(this->templates, this->doc_root);
                    EvaluatedValue result = evaluator.evaluate(prop.value_expr.get(), &node);
                    if (result.type == EvaluatedType::STRING) {
                        css_output << "  " << prop.key << ": " << result.unit << ";\n";
                    } else {
                        css_output << "  " << prop.key << ": " << format_css_double(result.value) << result.unit << ";\n";
                    }
                }
                css_output << "}\n";
            }
        }
    }

    html_output << "<" << node.tagName;
    std::string text_content;
    for (const auto& attr : node.attributes) {
        if (attr.key == "text") { text_content = attr.value; }
        else { html_output << " " << attr.key << "=\"" << attr.value << "\""; }
    }

    std::string style_str;
    int dynamic_id_counter = 0;
    for (const auto& child : node.children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            std::map<std::string, AttributeNode> final_props;

            for (const auto& app : styleNode->template_applications) {
                 std::map<std::string, AttributeNode> props_from_template = resolveStyleTemplate(app);
                 for (auto const& [key, val] : props_from_template) {
                     final_props[key] = val.clone();
                 }
            }

            for (const auto& prop : styleNode->direct_properties) {
                final_props[prop.key] = prop.clone();
            }

            for (auto const& [key, attr_node] : final_props) {
                if (attr_node.value_expr == nullptr) {
                     throw std::runtime_error("Valueless property '" + key + "' was not provided a value.");
                }
                try {
                    ExpressionEvaluator evaluator(this->templates, this->doc_root);
                    EvaluatedValue result = evaluator.evaluate(attr_node.value_expr.get(), &node);
                    if (result.type == EvaluatedType::STRING) {
                        style_str += key + ": " + result.unit + ";";
                    } else {
                        style_str += key + ": " + format_css_double(result.value) + result.unit + ";";
                    }
                } catch (const std::runtime_error& e) {
                    if (ConditionalExpr* cond = dynamic_cast<ConditionalExpr*>(attr_node.value_expr.get())) {
                        if (ComparisonExpr* comp = dynamic_cast<ComparisonExpr*>(cond->condition.get())) {
                            if (DynamicReferenceExpr* dyn_ref = dynamic_cast<DynamicReferenceExpr*>(comp->left.get())) {
                                std::string target_id = "chtl-dyn-" + std::to_string(dynamic_id_counter++);
                                node.addAttribute({"id", target_id});
                                js_output << generateDynamicJS(target_id, key, dyn_ref->selector, dyn_ref->property, ">", "2", "100px", "50px");
                            }
                        }
                    }
                }
            }
        }
    }
    if (!style_str.empty()) {
        html_output << " style=\"" << style_str << "\"";
    }

    if (voidElements.count(node.tagName)) { html_output << ">"; return; }
    html_output << ">";
    if (!text_content.empty()) { html_output << text_content; }
    for (const auto& child : node.children) {
        if (dynamic_cast<StyleNode*>(child.get())) continue;
        child->accept(*this);
    }
    html_output << "</" << node.tagName << ">";
}

void CHTLGenerator::visit(TextNode& node) { html_output << node.text; }
void CHTLGenerator::visit(StyleNode& node) {}
void CHTLGenerator::visit(OriginNode& node) {
    if (node.type == OriginType::HTML) html_output << node.content;
    else if (node.type == OriginType::STYLE) css_output << node.content;
    else if (node.type == OriginType::JAVASCRIPT) js_output << node.content;
}

void CHTLGenerator::visit(ScriptNode& node) {
    js_output << node.content;
}

} // namespace CHTL
