#include "CHTLGenerator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../CHTLNode/IfNode.h"
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
#include <iomanip>
#include <map>
#include <sstream>

namespace CHTL {

// Helper to format doubles cleanly for CSS output
std::string format_css_double(double val) {
    std::ostringstream oss;
    oss << val;
    return oss.str();
}

// Helper to escape strings for use in JavaScript literals
std::string escape_js_string(const std::string& s) {
    std::ostringstream o;
    for (auto c = s.cbegin(); c != s.cend(); c++) {
        switch (*c) {
            case '"': o << "\\\""; break;
            case '\\': o << "\\\\"; break;
            case '\b': o << "\\b"; break;
            case '\f': o << "\\f"; break;
            case '\n': o << "\\n"; break;
            case '\r': o << "\\r"; break;
            case '\t': o << "\\t"; break;
            default:
                if ('\x00' <= *c && *c <= '\x1f') {
                    o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(*c);
                } else {
                    o << *c;
                }
        }
    }
    return o.str();
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

    std::string final_html = use_html5_doctype ? "<!DOCTYPE html>\n" : "";
    final_html += html_output.str();

    return {final_html, css_output.str(), js_output.str()};
}

void CHTLGenerator::visit(ElementNode& node) {
    // --- Global Style & Auto-Attribute Generation ---
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
                    // NOTE: Dynamic expressions are NOT handled in global rules.
                    ExpressionEvaluator evaluator(this->templates, this->doc_root);
                    EvaluatedValue result = evaluator.evaluate(prop.value_expr.get(), &node);
                    css_output << "  " << prop.key << ": " << format_css_double(result.value) << result.unit << ";\n";
                }
                css_output << "}\n";
            }
        }
    }

    // --- HTML Tag Generation ---
    html_output << "<" << node.tagName;
    std::string text_content;
    for (const auto& attr : node.attributes) {
        if (attr.key == "text") { text_content = attr.value; }
        else { html_output << " " << attr.key << "=\"" << attr.value << "\""; }
    }

    // --- Inline Style Generation ---
    std::string style_str;
    int dynamic_id_counter = 0;
    for (const auto& child : node.children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            std::map<std::string, AttributeNode> final_props;

            // --- Style Specialization Logic ---
            // 1. Apply all templates first
            for (const auto& app : styleNode->template_applications) {
                const TemplateDefinitionNode* def = nullptr;
                for (const auto& ns_pair : this->templates) {
                    if (ns_pair.second.count(app.template_name)) {
                        def = &ns_pair.second.at(app.template_name);
                        break;
                    }
                }
                if (def && def->type == TemplateType::STYLE) {
                    for (const auto& prop : def->style_properties) {
                        final_props[prop.key] = prop.clone();
                    }
                }
            }

            // 2. Handle deletions and overrides from all applications
            for (const auto& app : styleNode->template_applications) {
                 // Delete specified properties
                for (const auto& key_to_delete : app.deleted_properties) {
                     // This is a simplification. A real implementation would track the origin
                     // of each property to correctly delete inherited styles.
                     // For now, we only delete properties by name.
                    if (key_to_delete.rfind("@Style", 0) != 0) {
                        final_props.erase(key_to_delete);
                    }
                }
                // Apply new or overridden properties (fills in valueless props)
                for (const auto& prop : app.new_or_overridden_properties) {
                    final_props[prop.key] = prop.clone();
                }
            }

            // 3. Apply direct properties from the style block
            for (const auto& prop : styleNode->direct_properties) {
                final_props[prop.key] = prop.clone();
            }

            // 4. Generate CSS from the final property map
            for (auto const& [key, attr_node] : final_props) {
                if (attr_node.value_expr == nullptr) {
                     throw std::runtime_error("Valueless property '" + key + "' was not provided a value.");
                }
                try {
                    ExpressionEvaluator evaluator(this->templates, this->doc_root);
                    EvaluatedValue result = evaluator.evaluate(attr_node.value_expr.get(), &node);
                    style_str += key + ": " + format_css_double(result.value) + result.unit + ";";
                } catch (const std::runtime_error& e) {
                    // This is likely a DynamicReferenceExpr, which can't be evaluated statically.
                    // We need to generate JS for it.
                    if (ConditionalExpr* cond = dynamic_cast<ConditionalExpr*>(attr_node.value_expr.get())) {
                        if (ComparisonExpr* comp = dynamic_cast<ComparisonExpr*>(cond->condition.get())) {
                            if (DynamicReferenceExpr* dyn_ref = dynamic_cast<DynamicReferenceExpr*>(comp->left.get())) {
                                std::string target_id = "chtl-dyn-" + std::to_string(dynamic_id_counter++);
                                node.addAttribute({"id", target_id});
                                // This is a massive simplification for the demo
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
    // This logic is now handled by the dispatcher and scanner
    js_output << node.content;
}

std::string CHTLGenerator::renderBranch(const std::vector<std::unique_ptr<BaseNode>>& branch) {
    // Save the current html_output stream by swapping it with a temporary one.
    std::stringstream temp_stream;
    std::swap(this->html_output, temp_stream);

    // Now this->html_output is empty. We can render the branch into it.
    for (const auto& node : branch) {
        node->accept(*this);
    }

    // Capture the generated HTML for the branch.
    std::string result = this->html_output.str();

    // Restore the original html_output stream.
    std::swap(this->html_output, temp_stream);

    // The original content is now back in html_output, and we have the branch content in `result`.
    return result;
}


void CHTLGenerator::visit(IfNode& node) {
    if (!node.condition->isDynamic()) {
        // --- Static Evaluation Logic ---
        ExpressionEvaluator evaluator(this->templates, this->doc_root);
        EvaluatedValue condition_result = evaluator.evaluate(node.condition.get(), nullptr);

        if (evaluator.isTruthy(condition_result)) {
            for (const auto& child : node.then_branch) {
                child->accept(*this);
            }
        } else {
            if (node.else_if_branch) {
                node.else_if_branch->accept(*this);
            } else if (!node.else_branch.empty()) {
                for (const auto& child : node.else_branch) {
                    child->accept(*this);
                }
            }
        }
    } else {
        // --- Dynamic JS Generation Logic ---
        JSExpressionGenerator js_expr_gen;
        std::string js_condition = js_expr_gen.generate(node.condition.get());

        std::string then_html = renderBranch(node.then_branch);
        std::string else_html = "";

        if (node.else_if_branch) {
            std::vector<std::unique_ptr<BaseNode>> else_if_wrapper;
            else_if_wrapper.push_back(node.else_if_branch->clone());
            else_html = renderBranch(else_if_wrapper);
        } else if (!node.else_branch.empty()) {
            else_html = renderBranch(node.else_branch);
        }

        std::string placeholder_id = "chtl-if-" + std::to_string(dynamic_if_counter++);
        html_output << "<div id=\"" << placeholder_id << "\"></div>";

        // This is a highly simplified listener. A real implementation would need
        // to parse the expression, find all dynamic sources, and attach listeners
        // (e.g., MutationObservers) to them. For now, we'll just check on a timer.
        js_output << "(function() {\n";
        js_output << "  const placeholder = document.getElementById('"<< placeholder_id << "');\n";
        js_output << "  const then_content = \"" << escape_js_string(then_html) << "\";\n";
        js_output << "  const else_content = \"" << escape_js_string(else_html) << "\";\n";
        js_output << "  const update = () => {\n";
        js_output << "    try {\n";
        js_output << "      if (" << js_condition << ") {\n";
        js_output << "        placeholder.innerHTML = then_content;\n";
        js_output << "      } else {\n";
        js_output << "        placeholder.innerHTML = else_content;\n";
        js_output << "      }\n";
        js_output << "    } catch (e) { /* Do nothing if selector is not found yet */ }\n";
        js_output << "  };\n";
        js_output << "  setInterval(update, 200); // Simple polling for demo purposes\n";
        js_output << "  update(); // Initial call\n";
        js_output << "})();\n";
    }
}

} // namespace CHTL
