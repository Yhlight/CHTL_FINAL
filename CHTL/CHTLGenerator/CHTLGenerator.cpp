#include "CHTLGenerator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include "../../CHTL JS/CHTLJSLexer/CHTLJSLexer.h"
#include "../../CHTL JS/CHTLJSParser/CHTLJSParser.h"
#include "../../CHTL JS/CHTLJSNode/RawJSNode.h"
#include "../../CHTL JS/CHTLJSNode/EnhancedSelectorNode.h"
#include "../../CHTL JS/CHTLJSNode/ListenNode.h"
#include "../../CHTL JS/CHTLJSNode/EventHandlerNode.h"
#include "../../CHTL JS/CHTLJSNode/DelegateNode.h"
#include "../../CHTL JS/CHTLJSNode/AnimateNode.h"
#include "../Expression/ExpressionEvaluator.h"
#include "../Expression/ResponsiveValueNode.h"
#include <unordered_set>
#include <algorithm>
#include <map>
#include <sstream>
#include <variant>

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

CHTLGenerator::CHTLGenerator(const std::map<std::string, std::map<std::string, std::shared_ptr<TemplateDefinitionNode>>>& templates, std::shared_ptr<Configuration> config)
    : templates(templates), config(config), doc_root(nullptr) {}

CompilationResult CHTLGenerator::generate(BaseNode* root, bool use_html5_doctype) {
    html_output.str("");
    css_output.str("");
    js_output.str("");
    delegate_registry.clear();
    reactive_bindings.clear();

    this->doc_root = root;
    if (root) {
        root->accept(*this);
    }

    // Process the delegate registry
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

    // Process reactive bindings
    if (!reactive_bindings.empty()) {
        js_output << "\n// --- CHTL Reactivity System ---\n";
        js_output << "const __chtl_reactivity_manager = {\n";
        js_output << "  _proxies: {},\n";
        js_output << "  createReactive: function(obj, varName, updateFunc) {\n";
        js_output << "    if (!this._proxies[varName]) {\n";
        js_output << "      let _value = obj[varName];\n";
        js_output << "      this._proxies[varName] = { dependents: [] };\n";
        js_output << "      Object.defineProperty(obj, varName, {\n";
        js_output << "        get: () => _value,\n";
        js_output << "        set: (newValue) => {\n";
        js_output << "          _value = newValue;\n";
        js_output << "          this._proxies[varName].dependents.forEach(dep => dep(newValue));\n";
        js_output << "        }\n";
        js_output << "      });\n";
        js_output << "    }\n";
        js_output << "    this._proxies[varName].dependents.push(updateFunc);\n";
        js_output << "    if (obj[varName] !== undefined) { updateFunc(obj[varName]); }\n";
        js_output << "  }\n";
        js_output << "};\n\n";

        for (const auto& binding : reactive_bindings) {
            js_output << "__chtl_reactivity_manager.createReactive(window, '" << binding.variable_name
                      << "', (newValue) => { document.getElementById('" << binding.element_id
                      << "')." << binding.attribute_name << " = newValue; });\n";
        }
    }


    std::string final_html = use_html5_doctype ? "<!DOCTYPE html>\n" : "";
    final_html += html_output.str();

    return {final_html, css_output.str(), js_output.str()};
}

void CHTLGenerator::visit(ElementNode& node) {
    // --- Global Style & Auto-Attribute Generation ---
    for (const auto& child : node.children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            // ... (style processing logic)
        }
    }

    // --- HTML Tag Generation ---
    html_output << "<" << node.tagName;

    // --- Attribute Generation with Reactivity ---
    // First, determine if we need to add a reactive ID
    bool needs_reactive_id = false;
    std::string element_id;
    bool has_id = false;

    for(const auto& attr : node.attributes) {
        if (std::holds_alternative<ResponsiveValueNode>(attr.value)) {
            needs_reactive_id = true;
        }
        if (attr.key == "id" && std::holds_alternative<std::string>(attr.value)) {
            element_id = std::get<std::string>(attr.value);
            has_id = true;
        }
    }

    if (needs_reactive_id && !has_id) {
        static int id_counter = 0;
        element_id = "__chtl_reactive_id_" + std::to_string(id_counter++);
        node.addAttribute({"id", element_id});
    }

    // Now, generate the attributes
    for (const auto& attr : node.attributes) {
        if (attr.key == "text") continue;

        std::visit([&](auto&& arg) {
            using T = std::decay_t<decltype(arg)>;
            if constexpr (std::is_same_v<T, std::string>) {
                html_output << " " << attr.key << "=\"" << arg << "\"";
            } else if constexpr (std::is_same_v<T, ResponsiveValueNode>) {
                std::string element_id;
                bool has_id = false;
                for(const auto& a : node.attributes) {
                    if (a.key == "id" && std::holds_alternative<std::string>(a.value)) {
                        element_id = std::get<std::string>(a.value);
                        has_id = true;
                        break;
                    }
                }
                if (!has_id) {
                    static int id_counter = 0;
                    element_id = "__chtl_reactive_id_" + std::to_string(id_counter++);
                    node.addAttribute({"id", element_id});
                    html_output << " id=\"" << element_id << "\"";
                }

                std::string js_prop = (attr.key == "class") ? "className" : attr.key;
                reactive_bindings.push_back({element_id, js_prop, arg.variable_name});
            }
        }, attr.value);
    }

    // --- Inline Style Generation with Reactivity ---
    std::string style_str;
    for (const auto& child : node.children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            std::map<std::string, AttributeNode> final_props;
            for (const auto& app : styleNode->template_applications) {
                auto def = app.definition;
                if (def) {
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
                if (result.is_responsive) {
                    // Handle reactive styles later
                } else {
                    style_str += pair.first + ": ";
                    if (result.value == 0 && !result.unit.empty()) { style_str += result.unit; }
                    else { style_str += format_css_double(result.value) + result.unit; }
                    style_str += ";";
                }
            }
        }
    }
    if (!style_str.empty()) {
        html_output << " style=\"" << style_str << "\"";
    }

    if (voidElements.count(node.tagName)) {
        html_output << ">";
        return;
    }
    html_output << ">";
    for (const auto& attr : node.attributes) {
        if (attr.key == "text" && std::holds_alternative<std::string>(attr.value)) {
            html_output << std::get<std::string>(attr.value);
        }
    }
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

void CHTLGenerator::visit(NamespaceNode& node) {
    for (const auto& child : node.children) {
        child->accept(*this);
    }
}

void CHTLGenerator::visit(ScriptNode& node) {
    js_output << node.content;
}

}
