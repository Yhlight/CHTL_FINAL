#include "CHTLGenerator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/DocumentNode.h"
#include "../CHTLNode/ScriptNode.h"
#include "../Expression/ExpressionEvaluator.h"
#include "../../CHTL_JS/CHTLJSProcessor.h"
#include <unordered_set>
#include <algorithm>
#include <map>

namespace CHTL {

const std::unordered_set<std::string> voidElements = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

CHTLGenerator::CHTLGenerator(const std::map<std::string, std::map<std::string, TemplateDefinitionNode>>& templates, const std::map<std::string, std::string>& placeholders)
    : templates(templates), placeholders(placeholders), doc_root(nullptr) {}

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

void CHTLGenerator::visit(DocumentNode& node) {
    ExpressionEvaluator evaluator(this->templates, this->doc_root);

    // First pass: collect all global CSS and all JS
    for (const auto& child : node.children) {
        collectAndGenerateCss(child.get(), evaluator);
        collectJs(child.get());
    }

    // Second pass: find the root element (e.g. <html>) and generate HTML
    for (const auto& child : node.children) {
        if (dynamic_cast<ElementNode*>(child.get())) {
            child->accept(*this);
            // Assume the first element node is the root for HTML generation
            break;
        }
    }

    // Inject the script tag before the closing body tag if JS exists
    std::string final_html = html_output.str();
    std::string script_content = js_output.str();
    if (!script_content.empty()) {
        std::string script_tag = "<script>" + script_content + "</script>";
        size_t body_end = final_html.rfind("</body>");
        if (body_end != std::string::npos) {
            final_html.insert(body_end, script_tag);
        } else {
            final_html += script_tag; // Append at the end if no body tag is found
        }
        html_output.str(final_html);
    }
}

void CHTLGenerator::collectAndGenerateCss(BaseNode* node, ExpressionEvaluator& evaluator) {
    if (!node) return;
    if (StyleNode* styleNode = dynamic_cast<StyleNode*>(node)) {
        if (!styleNode->placeholder_key.empty()) {
            if (placeholders.count(styleNode->placeholder_key)) {
                css_output << placeholders.at(styleNode->placeholder_key);
            }
        } else {
            for (const auto& rule : styleNode->global_rules) {
                css_output << rule.selector << " {\n";
                for (const auto& prop : rule.properties) {
                    PropertyValue result = evaluator.evaluate(prop.value_expr.get(), nullptr);
                    css_output << "    " << prop.key << ": " << result.toString() << ";\n";
                }
                css_output << "}\n";
            }
        }
    }
    if (ElementNode* elementNode = dynamic_cast<ElementNode*>(node)) {
        for (const auto& child : elementNode->children) {
            collectAndGenerateCss(child.get(), evaluator);
        }
    }
}

void CHTLGenerator::collectJs(BaseNode* node) {
    if (!node) return;
    // If we find a script node, visit it to append its content to js_output
    if (ScriptNode* scriptNode = dynamic_cast<ScriptNode*>(node)) {
        scriptNode->accept(*this);
    }
    // Recurse into element nodes to find local scripts
    if (ElementNode* elementNode = dynamic_cast<ElementNode*>(node)) {
        for (const auto& child : elementNode->children) {
            collectJs(child.get());
        }
    }
}

std::map<std::string, AttributeNode> CHTLGenerator::computeFinalStyles(ElementNode& node) {
    std::map<std::string, AttributeNode> final_props;
    StyleNode* styleNode = nullptr;
    for (const auto& child : node.children) {
        if (StyleNode* sn = dynamic_cast<StyleNode*>(child.get())) {
            styleNode = sn;
            break;
        }
    }
    if (!styleNode) return final_props;
    for (const auto& app : styleNode->template_applications) {
        std::vector<const TemplateDefinitionNode*> inheritance_chain;
        std::vector<const TemplateDefinitionNode*> processing_stack;
        const TemplateDefinitionNode* initial_def = nullptr;
        for (const auto& ns_pair : this->templates) {
            if (ns_pair.second.count(app.template_name)) {
                initial_def = &ns_pair.second.at(app.template_name);
                break;
            }
        }
        if (initial_def) processing_stack.push_back(initial_def);
        while (!processing_stack.empty()) {
            const TemplateDefinitionNode* current_def = processing_stack.back();
            processing_stack.pop_back();
            inheritance_chain.push_back(current_def);
            for (const auto& inheritance : current_def->inherits) {
                const TemplateDefinitionNode* parent_def = nullptr;
                for (const auto& ns_pair : this->templates) {
                    if (ns_pair.second.count(inheritance.name)) {
                         parent_def = &ns_pair.second.at(inheritance.name);
                         break;
                    }
                }
                if (parent_def) processing_stack.push_back(parent_def);
            }
        }
        std::reverse(inheritance_chain.begin(), inheritance_chain.end());
        for (const auto* def : inheritance_chain) {
            for (const auto& prop : def->style_properties) {
                final_props[prop.key] = prop.clone();
            }
            for (const auto& key_to_delete : def->deleted_properties) {
                final_props.erase(key_to_delete);
            }
        }
        for (const auto& key_to_delete : app.deleted_properties) {
            final_props.erase(key_to_delete);
        }
        for (const auto& prop : app.new_or_overridden_properties) {
            final_props[prop.key] = prop.clone();
        }
    }
    for (const auto& prop : styleNode->direct_properties) {
        final_props[prop.key] = prop.clone();
    }
    return final_props;
}

void CHTLGenerator::visit(ElementNode& node) {
    html_output << "<" << node.tagName;
    std::string text_content;
    ExpressionEvaluator attr_evaluator(this->templates, this->doc_root);
    for (const auto& attr : node.attributes) {
        PropertyValue result = attr_evaluator.evaluate(attr.value_expr.get(), &node);
        if (attr.key == "text") {
            text_content = result.toString();
        } else {
            html_output << " " << attr.key << "=\"" << result.toString() << "\"";
        }
    }
    std::string style_str;
    std::map<std::string, AttributeNode> final_styles = computeFinalStyles(node);
    ExpressionEvaluator style_evaluator(this->templates, this->doc_root);
    for (const auto& pair : final_styles) {
        PropertyValue result = style_evaluator.evaluate(pair.second.value_expr.get(), &node);
        style_str += pair.first + ": " + result.toString() + ";";
    }
    if (!style_str.empty()) {
        html_output << " style=\"" << style_str << "\"";
    }
    if (voidElements.count(node.tagName)) {
        html_output << ">";
        return;
    }
    html_output << ">";
    if (!text_content.empty()) {
        html_output << text_content;
    }
    for (const auto& child : node.children) {
        // Do not visit ScriptNodes here, they are collected in a separate pass
        if (dynamic_cast<ScriptNode*>(child.get())) continue;
        child->accept(*this);
    }
    html_output << "</" << node.tagName << ">";
}

void CHTLGenerator::visit(TextNode& node) {
    html_output << node.text;
}

void CHTLGenerator::visit(StyleNode& node) {
    // Style properties are handled by computeFinalStyles within visit(ElementNode).
    // Global CSS rules are handled by collectAndGenerateCss.
    // This visit function is for the HTML-generation pass, where style nodes are ignored.
}

void CHTLGenerator::visit(ScriptNode& node) {
    std::string raw_js;
    if (!node.placeholder_key.empty()) {
        if (placeholders.count(node.placeholder_key)) {
            raw_js = placeholders.at(node.placeholder_key);
        }
    } else {
        raw_js = node.content;
    }

    // Process the raw JS through the CHTL JS Processor
    CHTLJSProcessor processor(raw_js);
    js_output << processor.process();
}

void CHTLGenerator::visit(OriginNode& node) {
    if (node.type == OriginType::HTML) {
        html_output << node.content;
    } else if (node.type == OriginType::STYLE) {
        css_output << node.content;
    }
}

} // namespace CHTL
