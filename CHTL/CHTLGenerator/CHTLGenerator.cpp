#include "CHTLGenerator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/OriginNode.h"
#include <unordered_set>
#include <algorithm> // For std::find_if

namespace CHTL {

const std::unordered_set<std::string> voidElements = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

CompilationResult CHTLGenerator::generate(BaseNode* root) {
    html_output.str("");
    css_output.str("");
    if (root) {
        root->accept(*this);
    }
    return {html_output.str(), css_output.str()};
}

void CHTLGenerator::visit(ElementNode& node) {
    // --- Automation and Global CSS Generation ---
    // First, iterate through style nodes to find global rules and inject attributes.
    for (const auto& child : node.children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            for (const auto& rule : styleNode->global_rules) {
                std::string selector = rule.selector;
                // Automation: if selector is a simple class or id, inject it.
                if (selector.rfind('.', 0) == 0) { // Starts with .
                    std::string className = selector.substr(1);
                    // Avoid adding duplicate classes
                    auto it = std::find_if(node.attributes.begin(), node.attributes.end(),
                                           [](const AttributeNode& attr){ return attr.key == "class"; });
                    if (it != node.attributes.end()) {
                        if (it->value.find(className) == std::string::npos) {
                            it->value += " " + className;
                        }
                    } else {
                        node.attributes.push_back({"class", className});
                    }
                } else if (selector.rfind('#', 0) == 0) { // Starts with #
                    node.attributes.push_back({"id", selector.substr(1)});
                }

                // Context deduction for '&'
                // A simple version: replace '&' with the element's tag name.
                size_t pos = selector.find('&');
                if (pos != std::string::npos) {
                    selector.replace(pos, 1, node.tagName);
                }

                // Add the rule to the global CSS output
                css_output << selector << " {\n";
                for (const auto& prop : rule.properties) {
                    css_output << "    " << prop.key << ": " << prop.value << ";\n";
                }
                css_output << "}\n";
            }
        }
    }

    // --- HTML Tag Generation ---
    html_output << "<" << node.tagName;

    // Append attributes from the element itself (now including injected ones)
    for (const auto& attr : node.attributes) {
        html_output << " " << attr.key << "=\"" << attr.value << "\"";
    }

    // Process inline styles
    std::string style_str;
    for (const auto& child : node.children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            for (const auto& prop : styleNode->inline_properties) {
                style_str += prop.key + ": " + prop.value + ";";
            }
        }
    }
    if (!style_str.empty()) {
        html_output << " style=\"" << style_str << "\"";
    }

    if (voidElements.find(node.tagName) != voidElements.end()) {
        html_output << ">";
        return;
    }

    html_output << ">";

    // Process children, skipping style nodes
    for (const auto& child : node.children) {
        if (dynamic_cast<StyleNode*>(child.get())) continue;
        child->accept(*this);
    }
    html_output << "</" << node.tagName << ">";
}

void CHTLGenerator::visit(TextNode& node) {
    html_output << node.text;
}

void CHTLGenerator::visit(StyleNode& node) {
    // This visitor method is intentionally left empty.
    // StyleNodes are processed specially within visit(ElementNode&).
}

void CHTLGenerator::visit(OriginNode& node) {
    switch (node.type) {
        case OriginType::HTML:
            html_output << node.content;
            break;
        case OriginType::STYLE:
            css_output << node.content;
            break;
        case OriginType::JAVASCRIPT:
            // Not handled yet, but will be placed in a future JS output.
            break;
    }
}

} // namespace CHTL
