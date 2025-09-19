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
    if (node.is_virtual) {
        for (const auto& child : node.children) {
            child->accept(*this);
        }
        return;
    }
    // --- Global Style & Auto-Attribute Generation ---
    // This must happen before tag generation as it can modify attributes.
    for (const auto& child : node.children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            // Check if class/id attributes exist before processing rules for this style block
            bool class_attr_exists = std::any_of(node.attributes.begin(), node.attributes.end(),
                                                 [](const HtmlAttribute& attr){ return attr.key == "class"; });
            bool id_attr_exists = std::any_of(node.attributes.begin(), node.attributes.end(),
                                              [](const HtmlAttribute& attr){ return attr.key == "id"; });

            bool class_added_by_this_block = false;
            bool id_added_by_this_block = false;

            for (const auto& rule : styleNode->global_rules) {
                std::string selector = rule.selector;
                if (selector.empty()) continue;

                // Auto-add class/id attributes based on the *first* rule of its kind
                if (selector[0] == '.' && !config->disable_style_auto_add_class && !class_attr_exists && !class_added_by_this_block) {
                    std::string class_name = selector.substr(1);
                    // The parser might leave whitespace, so we trim it.
                    size_t first = class_name.find_first_not_of(" \t\n\r");
                    if (std::string::npos != first) {
                        size_t last = class_name.find_last_not_of(" \t\n\r");
                        class_name = class_name.substr(first, (last - first + 1));
                    }
                    node.addAttribute({"class", class_name});
                    class_added_by_this_block = true;
                } else if (selector[0] == '#' && !config->disable_style_auto_add_id && !id_attr_exists && !id_added_by_this_block) {
                    std::string id_name = selector.substr(1);
                    size_t first = id_name.find_first_not_of(" \t\n\r");
                    if (std::string::npos != first) {
                        size_t last = id_name.find_last_not_of(" \t\n\r");
                        id_name = id_name.substr(first, (last - first + 1));
                    }
                    node.addAttribute({"id", id_name});
                    id_added_by_this_block = true;
                }

                // Generate the CSS for the rule and add it to the global CSS output
                css_output << rule.selector << " {\n";
                ExpressionEvaluator evaluator(this->templates, this->doc_root);
                for (const auto& prop : rule.properties) {
                    EvaluatedValue result = evaluator.evaluate(prop.value_expr.get(), &node);
                    css_output << "  " << prop.key << ": ";
                    if (result.value == 0 && !result.unit.empty()) {
                        css_output << result.unit;
                    } else {
                        css_output << format_css_double(result.value) << result.unit;
                    }
                    css_output << ";\n";
                }
                css_output << "}\n";
            }
        }
    }

    // --- HTML Tag Generation ---
    html_output << "<" << node.tagName;
    for (const auto& attr : node.attributes) {
        html_output << " " << attr.key << "=\"" << attr.value << "\"";
    }

    // --- Inline Style Generation ---
    std::string style_str;
    for (const auto& child : node.children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
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
                style_str += pair.first + ": ";
                if (result.value == 0 && !result.unit.empty()) { style_str += result.unit; }
                else { style_str += format_css_double(result.value) + result.unit; }
                style_str += ";";
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
    for (const auto& child : node.children) {
        if (dynamic_cast<StyleNode*>(child.get())) continue;
        child->accept(*this);
    }
    html_output << "</" << node.tagName << ">";
}

void CHTLGenerator::visit(TextNode& node) { html_output << node.text; }
void CHTLGenerator::visitCommentNode(CommentNode& node) {
    html_output << "<!-- " << node.text << " -->";
}
void CHTLGenerator::visit(StyleNode& node) {} // Handled inside ElementNode visit
void CHTLGenerator::visit(OriginNode& node) {
    if (node.type == OriginType::HTML) html_output << node.content;
    else if (node.type == OriginType::STYLE) css_output << node.content;
    else if (node.type == OriginType::JAVASCRIPT) js_output << node.content;
}

void CHTLGenerator::visit(ScriptNode& node) {
    CHTL_JS::CHTLJSLexer lexer(node.content);
    std::vector<CHTL_JS::Token> tokens = lexer.scanTokens();
    CHTL_JS::CHTLJSParser parser(tokens, node.content);
    auto js_nodes = parser.parse();

    for (const auto& js_node : js_nodes) {
        if (!js_node) continue;
        if (js_node->type == CHTL_JS::CHTLJSNodeType::Animate) {
            if (auto* animate_node = dynamic_cast<CHTL_JS::AnimateNode*>(js_node.get())) {
                js_output << "{\n";
                js_output << "  const targets = [";
                for (size_t i = 0; i < animate_node->targets.size(); ++i) {
                    js_output << "document.querySelector('" << animate_node->targets[i].selector_string << "')";
                    if (i < animate_node->targets.size() - 1) js_output << ", ";
                }
                js_output << "];\n";
                js_output << "  const duration = " << animate_node->duration.value_or(1000) << ";\n";
                // ... (generate other properties)
                js_output << "  let startTime = null;\n";
                js_output << "  function step(timestamp) {\n";
                js_output << "    if (!startTime) startTime = timestamp;\n";
                js_output << "    const progress = Math.min((timestamp - startTime) / duration, 1);\n";
                js_output << "    targets.forEach(target => {\n";
                // ... (generate style updates based on progress)
                js_output << "    });\n";
                js_output << "    if (progress < 1) {\n";
                js_output << "      requestAnimationFrame(step);\n";
                js_output << "    }\n";
                js_output << "  }\n";
                js_output << "  requestAnimationFrame(step);\n";
                js_output << "}\n";
            }
        } else if (js_node->type == CHTL_JS::CHTLJSNodeType::Delegate) {
            if (auto* delegate_node = dynamic_cast<CHTL_JS::DelegateNode*>(js_node.get())) {
                delegate_registry[delegate_node->parent_selector.selector_string].push_back(*delegate_node);
            }
        } else if (js_node->type == CHTL_JS::CHTLJSNodeType::Listen) {
            if (auto* listen_node = dynamic_cast<CHTL_JS::ListenNode*>(js_node.get())) {
                const auto& parsed = listen_node->selector;
                std::string selector_js;
                if (parsed.type == CHTL_JS::SelectorType::IndexedQuery) {
                    selector_js = "document.querySelectorAll('" + parsed.selector_string + "')[" + std::to_string(parsed.index.value_or(0)) + "]";
                } else {
                    if (!parsed.selector_string.empty() && parsed.selector_string[0] == '#') {
                        selector_js = "document.querySelector('" + parsed.selector_string + "')";
                    } else {
                        selector_js = "document.querySelectorAll('" + parsed.selector_string + "')";
                    }
                }

                if (!parsed.selector_string.empty() && parsed.selector_string[0] == '#') {
                     for (const auto& event : listen_node->events) {
                        js_output << selector_js << ".addEventListener('" << event.first << "', " << event.second << ");\n";
                    }
                } else {
                    js_output << selector_js << ".forEach(el => {\n";
                    for (const auto& event : listen_node->events) {
                        js_output << "  el.addEventListener('" << event.first << "', " << event.second << ");\n";
                    }
                    js_output << "});\n";
                }
            }
        } else if (js_node->type == CHTL_JS::CHTLJSNodeType::EventHandler) {
            if (auto* handler_node = dynamic_cast<CHTL_JS::EventHandlerNode*>(js_node.get())) {
                const auto& parsed = handler_node->selector;
                std::string selector_js;
                if (parsed.type == CHTL_JS::SelectorType::IndexedQuery) {
                    selector_js = "document.querySelectorAll('" + parsed.selector_string + "')[" + std::to_string(parsed.index.value_or(0)) + "]";
                } else {
                    if (!parsed.selector_string.empty() && parsed.selector_string[0] == '#') {
                        selector_js = "document.querySelector('" + parsed.selector_string + "')";
                    } else {
                        selector_js = "document.querySelectorAll('" + parsed.selector_string + "')";
                    }
                }
                 if (!parsed.selector_string.empty() && parsed.selector_string[0] == '#') {
                    for (const auto& event_name : handler_node->event_names) {
                        js_output << selector_js << ".addEventListener('" << event_name << "', " << handler_node->handler << ");\n";
                    }
                } else {
                    js_output << selector_js << ".forEach(el => {\n";
                    for (const auto& event_name : handler_node->event_names) {
                        js_output << "  el.addEventListener('" << event_name << "', " << handler_node->handler << ");\n";
                    }
                    js_output << "});\n";
                }
            }
        } else if (js_node->type == CHTL_JS::CHTLJSNodeType::RawJS) {
            if (auto* raw_node = dynamic_cast<CHTL_JS::RawJSNode*>(js_node.get())) {
                js_output << raw_node->content;
            }
        } else if (js_node->type == CHTL_JS::CHTLJSNodeType::EnhancedSelector) {
             if (auto* selector_node = dynamic_cast<CHTL_JS::EnhancedSelectorNode*>(js_node.get())) {
                const auto& parsed = selector_node->parsed_selector;
                if (parsed.type == CHTL_JS::SelectorType::IndexedQuery) {
                    js_output << "document.querySelectorAll('" << parsed.selector_string << "')[" << parsed.index.value_or(0) << "]";
                } else {
                    if (!parsed.selector_string.empty() && parsed.selector_string[0] == '#') {
                        js_output << "document.querySelector('" << parsed.selector_string << "')";
                    } else {
                        js_output << "document.querySelectorAll('" << parsed.selector_string << "')";
                    }
                }
            }
        }
    }
}

} // namespace CHTL
