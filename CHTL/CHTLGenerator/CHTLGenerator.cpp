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
#include "../Expression/ExprVisitors.h"
#include "../Expression/JSExprGenerator.h"
#include <unordered_set>
#include <algorithm>
#include <map>
#include <sstream>

namespace CHTL {

// Helper function to convert kebab-case to camelCase
std::string toCamelCase(const std::string& s) {
    std::string result;
    bool upper = false;
    for (char c : s) {
        if (c == '-') {
            upper = true;
        } else if (upper) {
            result += std::toupper(c);
            upper = false;
        } else {
            result += c;
        }
    }
    return result;
}

// --- Helper to generate JS for a reactive attribute ---
void CHTLGenerator::generateReactiveAttributeJS(ElementNode& node, const std::string& attr_key, Expr* expr) {
    static int reactive_id_counter = 0;
    std::string element_id;
    for(const auto& attr : node.attributes) { if (attr.key == "id") { element_id = attr.value; break; } }
    if (element_id.empty()) {
        element_id = "chtl-reactive-" + std::to_string(reactive_id_counter++);
        node.addAttribute({"id", element_id});
    }

    if (auto* rv = dynamic_cast<ReactiveVarExpr*>(expr)) {
        std::string var_name = rv->name;
        if (defined_reactive_vars.find(var_name) == defined_reactive_vars.end()) {
            js_output << "{\n";
            js_output << "  let value = '';\n";
            js_output << "  const subscribers = new Map();\n";
            js_output << "  Object.defineProperty(window, '" << var_name << "', {\n";
            js_output << "    get() { return value; },\n";
            js_output << "    set(newValue) { value = newValue; subscribers.forEach(cb => cb(newValue)); },\n";
            js_output << "    configurable: true\n";
            js_output << "  });\n";
            js_output << "  window.subscribe_" << var_name << " = (id, cb) => { subscribers.set(id, cb); };\n";
            js_output << "}\n";
            defined_reactive_vars.insert(var_name);
        }
        js_output << "window.subscribe_" << var_name << "('" << element_id << "_" << attr_key << "', (newValue) => {\n";
        js_output << "  const el = document.getElementById('" << element_id << "');\n";
        js_output << "  if (el) el.setAttribute('" << attr_key << "', newValue);\n";
        js_output << "});\n";
    }
}

// --- Helper to generate JS for a reactive style property ---
void CHTLGenerator::generateReactiveStyleJS(ElementNode& node, const std::string& style_property, Expr* expr) {
    static int reactive_id_counter = 0;
    std::string element_id;
    for(const auto& attr : node.attributes) { if (attr.key == "id") { element_id = attr.value; break; } }
    if (element_id.empty()) {
        element_id = "chtl-reactive-" + std::to_string(reactive_id_counter++);
        node.addAttribute({"id", element_id});
    }

    if (auto* rv = dynamic_cast<ReactiveVarExpr*>(expr)) {
        std::string var_name = rv->name;
        if (defined_reactive_vars.find(var_name) == defined_reactive_vars.end()) {
            js_output << "{\n";
            js_output << "  let value = '';\n";
            js_output << "  const subscribers = new Map();\n";
            js_output << "  Object.defineProperty(window, '" << var_name << "', {\n";
            js_output << "    get() { return value; },\n";
            js_output << "    set(newValue) { value = newValue; subscribers.forEach(cb => cb(newValue)); },\n";
            js_output << "    configurable: true\n";
            js_output << "  });\n";
            js_output << "  window.subscribe_" << var_name << " = (id, cb) => { subscribers.set(id, cb); };\n";
            js_output << "}\n";
            defined_reactive_vars.insert(var_name);
        }
        js_output << "window.subscribe_" << var_name << "('" << element_id << "_" << style_property << "', (newValue) => {\n";
        js_output << "  const el = document.getElementById('" << element_id << "');\n";
        js_output << "  if (el) el.style." << toCamelCase(style_property) << " = newValue;\n";
        js_output << "});\n";
    }
}

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


CHTLGenerator::CHTLGenerator(const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates, std::shared_ptr<Configuration> config)
    : templates(templates), config(config), doc_root(nullptr) {}

CompilationResult CHTLGenerator::generate(BaseNode* root, bool use_html5_doctype) {
    html_output.str("");
    css_output.str("");
    js_output.str("");
    delegate_registry.clear();
    defined_reactive_vars.clear();
    this->doc_root = root;
    if (root) {
        root->accept(static_cast<Visitor&>(*this));
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

    // --- Process Attributes (including reactive ones) ---
    for (const auto& attr : node.attributes) {
        if (attr.key == "text") {
            text_content = attr.value;
        } else if (attr.value_expr) {
             ReactivityChecker checker;
             if (checker.check(attr.value_expr.get())) {
                generateReactiveAttributeJS(node, attr.key, attr.value_expr.get());
                html_output << " " << attr.key << "=\"\""; // Set initial empty value
             } else {
                // Statically evaluate the attribute expression
                ExpressionEvaluator evaluator(this->templates, this->doc_root);
                EvaluatedValue result = evaluator.evaluate(attr.value_expr.get(), &node);
                html_output << " " << attr.key << "=\"" << (result.type == ValueType::STRING ? result.unit : format_css_double(result.value)) << "\"";
             }
        } else {
            // Simple static attribute
            html_output << " " << attr.key << "=\"" << attr.value << "\"";
        }
    }

    // --- Inline Style Generation ---
    std::map<std::string, AttributeNode> final_props;

    // 1. Process StyleNode children to populate final_props
    for (const auto& child : node.children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            // 1.1 Apply all templates first
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

            // 1.2 Handle deletions and overrides from all applications
            for (const auto& app : styleNode->template_applications) {
                for (const auto& key_to_delete : app.deleted_properties) {
                    if (key_to_delete.rfind("@Style", 0) != 0) {
                        final_props.erase(key_to_delete);
                    }
                }
                for (const auto& prop : app.new_or_overridden_properties) {
                    final_props[prop.key] = prop.clone();
                }
            }

            // 1.3 Apply direct properties from the style block
            for (const auto& prop : styleNode->direct_properties) {
                final_props[prop.key] = prop.clone();
            }
        }
    }

    // This helper is needed to interpret the result of the expression evaluator.
    auto isTruthy = [](const EvaluatedValue& val) {
        if (val.type == ValueType::STRING) return !val.unit.empty();
        return val.value != 0;
    };

    // 2. Process IfNode children for conditional properties
    for (const auto& child : node.children) {
        if (IfNode* ifNode = dynamic_cast<IfNode*>(child.get())) {
            // We only process conditional properties here. Conditional rendering is handled later.
            if (!ifNode->properties.empty()) {
                ExpressionEvaluator evaluator(this->templates, this->doc_root);
                if (isTruthy(evaluator.evaluate(ifNode->condition.get(), &node))) {
                    for (const auto& prop : ifNode->properties) {
                        final_props[prop.key] = prop.clone();
                    }
                }
            }
        }
    }

    // 3. Generate CSS from the final property map
    std::string style_str;
    for (auto const& [key, attr_node] : final_props) {
        if (attr_node.value_expr == nullptr) {
             throw std::runtime_error("Valueless property '" + key + "' was not provided a value.");
        }

        ReactivityChecker checker;
        if (checker.check(attr_node.value_expr.get())) {
            generateReactiveStyleJS(node, key, attr_node.value_expr.get());
        } else {
            try {
                ExpressionEvaluator evaluator(this->templates, this->doc_root);
                EvaluatedValue result = evaluator.evaluate(attr_node.value_expr.get(), &node);
                if (result.type == ValueType::STRING) {
                    style_str += key + ": " + result.unit + ";";
                } else {
                    style_str += key + ": " + format_css_double(result.value) + result.unit + ";";
                }
            } catch (const std::runtime_error& e) {
                // Static evaluation failed, this could be a dynamic expression we don't fully support yet.
                // For now, we silently ignore the error.
            }
        }
    }
    if (!style_str.empty()) {
        html_output << " style=\"" << style_str << "\"";
    }

    if (voidElements.count(node.tagName)) { html_output << ">"; return; }
    html_output << ">";
    if (!text_content.empty()) { html_output << text_content; }

    // --- Child Node Rendering ---
    for (const auto& child : node.children) {
        if (dynamic_cast<StyleNode*>(child.get())) {
            continue; // Already processed
        }

        if (IfNode* ifNode = dynamic_cast<IfNode*>(child.get())) {
            // This is an 'if' for conditional rendering
            if (!ifNode->children.empty()) {
                ReactivityChecker checker;
                if (checker.check(ifNode->condition.get())) {
                    // --- DYNAMIC/REACTIVE CONDITIONAL RENDERING ---
                    static int template_id_counter = 0;
                    std::string template_id = "chtl-if-template-" + std::to_string(template_id_counter++);
                    std::string anchor_id = "chtl-if-anchor-" + std::to_string(template_id_counter++);

                    // Render the content inside a <template> tag
                    html_output << "<template id=\"" << template_id << "\">";
                    for (const auto& if_child : ifNode->children) {
                        if_child->accept(*this);
                    }
                    html_output << "</template>";
                    // Add a script anchor so we know where to insert the content
                    html_output << "<script type=\"text/chtl-anchor\" id=\"" << anchor_id << "\"></script>";

                    JSExprGenerator js_gen;
                    std::string js_condition = js_gen.generate(ifNode->condition.get());

                    // Find all reactive vars used in the condition
                    std::set<std::string> reactive_vars_in_cond;
                    class VarFinder : public ExprVisitor {
                    public:
                        std::set<std::string>& found_vars;
                        VarFinder(std::set<std::string>& vars) : found_vars(vars) {}
                        void visit(BinaryExpr& expr) override { expr.left->accept(*this); expr.right->accept(*this); }
                        void visit(LiteralExpr& expr) override {}
                        void visit(VarExpr& expr) override {}
                        void visit(ReferenceExpr& expr) override {}
                        void visit(ComparisonExpr& expr) override { expr.left->accept(*this); expr.right->accept(*this); }
                        void visit(LogicalExpr& expr) override { expr.left->accept(*this); expr.right->accept(*this); }
                        void visit(ConditionalExpr& expr) override { expr.condition->accept(*this); expr.then_branch->accept(*this); expr.else_branch->accept(*this); }
                        void visit(DynamicReferenceExpr& expr) override {}
                        void visit(ReactiveVarExpr& expr) override { found_vars.insert(expr.name); }
                    };
                    VarFinder finder(reactive_vars_in_cond);
                    ifNode->condition->accept(finder);

                    js_output << "{\n";
                    js_output << "  const template = document.getElementById('" << template_id << "');\n";
                    js_output << "  const anchor = document.getElementById('" << anchor_id << "');\n";
                    js_output << "  let is_rendered = false;\n";
                    js_output << "  const updateDOM = () => {\n";
                    js_output << "    const condition = " << js_condition << ";\n";
                    js_output << "    if (condition && !is_rendered) {\n";
                    js_output << "      const content = template.content.cloneNode(true);\n";
                    js_output << "      anchor.parentNode.insertBefore(content, anchor.nextSibling);\n";
                    js_output << "      is_rendered = true;\n";
                    js_output << "    } else if (!condition && is_rendered) {\n";
                    js_output << "      let next = anchor.nextSibling;\n";
                    js_output << "      while(next && next.nodeName !== 'SCRIPT') { const to_remove = next; next = next.nextSibling; to_remove.remove(); }\n";
                    js_output << "      is_rendered = false;\n";
                    js_output << "    }\n";
                    js_output << "  };\n";

                    for (const auto& var_name : reactive_vars_in_cond) {
                        // Ensure the reactive var is defined
                        if (defined_reactive_vars.find(var_name) == defined_reactive_vars.end()) {
                            js_output << "  let value = '';\n";
                            js_output << "  const subscribers = new Map();\n";
                            js_output << "  Object.defineProperty(window, '" << var_name << "', {\n";
                            js_output << "    get() { return value; },\n";
                            js_output << "    set(newValue) { value = newValue; subscribers.forEach(cb => cb(newValue)); },\n";
                            js_output << "    configurable: true\n";
                            js_output << "  });\n";
                            js_output << "  window.subscribe_" << var_name << " = (id, cb) => { subscribers.set(id, cb); };\n";
                            defined_reactive_vars.insert(var_name);
                        }
                        js_output << "  window.subscribe_" << var_name << "('if_" << anchor_id << "', updateDOM);\n";
                    }
                    js_output << "  updateDOM(); // Initial check\n";
                    js_output << "}\n";

                } else {
                    // --- STATIC CONDITIONAL RENDERING ---
                    ExpressionEvaluator evaluator(this->templates, this->doc_root);
                    if (isTruthy(evaluator.evaluate(ifNode->condition.get(), &node))) {
                        for (const auto& if_child : ifNode->children) {
                            if_child->accept(*this);
                        }
                    }
                }
            }
            // 'if' for properties was handled above, so we do nothing here for that case.
        } else {
            // It's a regular node
            child->accept(*this);
        }
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

void CHTLGenerator::visit(IfNode& node) {
    // Handled by the parent ElementNode visitor
}

} // namespace CHTL
