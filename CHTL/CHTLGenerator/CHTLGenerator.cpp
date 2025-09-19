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
    js_output.str("");
    this->doc_root = root;
    if (root) {
        root->accept(*this);
    }
    return {html_output.str(), css_output.str(), js_output.str()};
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
                else { style_str += std::to_string(result.value) + result.unit; }
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
    else if (node.type == OriginType::JAVASCRIPT) js_output << node.content;
}

void CHTLGenerator::visit(ScriptNode& node) {
    CHTL_JS::CHTLJSLexer lexer(node.content);
    std::vector<CHTL_JS::Token> tokens = lexer.scanTokens();
    CHTL_JS::CHTLJSParser parser(tokens, node.content);
    auto js_nodes = parser.parse();

    for (const auto& js_node : js_nodes) {
        if (!js_node) continue;
        if (js_node->type == CHTL_JS::CHTLJSNodeType::Listen) {
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
