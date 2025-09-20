#include "CHTLGenerator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/TemplateDeclarationNode.h"
#include "../CHTLNode/CustomDeclarationNode.h"
#include "../Expression/ExpressionEvaluator.h" // Include the new evaluator
#include <unordered_set>
#include <algorithm> // For std::find_if
#include <sstream>   // For std::stringstream

namespace CHTL {

const std::unordered_set<std::string> voidElements = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

CHTLGenerator::CHTLGenerator(const std::map<std::string, TemplateDefinitionNode>& templates)
    : templates(templates) {}

CompilationResult CHTLGenerator::generate(BaseNode* root) {
    html_output.str("");
    css_output.str("");
    this->doc_root = root; // Set the document root context
    if (root) {
        if (ElementNode* root_element = dynamic_cast<ElementNode*>(root)) {
            if (root_element->tagName == "<root>") {
                // If it's the special root node, don't print the tag, just its children.
                for (const auto& child : root_element->children) {
                    child->accept(*this);
                }
            } else {
                // It's a normal element, process as usual.
                root->accept(*this);
            }
        } else {
             // Not an element at the root (e.g., just a text node), process normally.
            root->accept(*this);
        }
    }
    return {html_output.str(), css_output.str()};
}

void CHTLGenerator::visit(ElementNode& node) {
    // --- Pre-computation Step ---
    // Find any local style blocks and apply their automatic classes/ids to this element *before* rendering.
    for (const auto& child : node.children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            // Automatic class/id injection from the parser's findings
            if (!styleNode->auto_id.empty()) {
                auto it = std::find_if(node.attributes.begin(), node.attributes.end(),
                                       [](const HtmlAttribute& attr){ return attr.key == "id"; });
                if (it == node.attributes.end()) {
                    node.attributes.push_back({"id", styleNode->auto_id});
                }
            }
            if (!styleNode->auto_class.empty()) {
                auto it = std::find_if(node.attributes.begin(), node.attributes.end(),
                                       [](const HtmlAttribute& attr){ return attr.key == "class"; });
                if (it != node.attributes.end()) {
                    if (it->value.find(styleNode->auto_class) == std::string::npos) {
                        it->value += " " + styleNode->auto_class;
                    }
                } else {
                    node.attributes.push_back({"class", styleNode->auto_class});
                }
            }

            // Set parent context on the style node for '&' selector resolution later
            for(const auto& attr : node.attributes) {
                if (attr.key == "id") styleNode->parent_element_id = attr.value;
                if (attr.key == "class") styleNode->parent_element_class = attr.value;
            }
        }
    }

    // --- HTML Tag Generation ---
    html_output << "<" << node.tagName;

    // Process attributes, checking for reactive values
    for (const auto& attr : node.attributes) {
        // This is a simplification. A proper implementation would parse the attribute value as an expression.
        if (attr.value.rfind("$", 0) == 0) { // Starts with $
            std::string varName = attr.value.substr(1, attr.value.length() - 2);
            html_output << " data-chtl-attr-" << attr.key << "=\"" << varName << "\"";
        } else {
            html_output << " " << attr.key << "=\"" << attr.value << "\"";
        }
    }

    // Process inline styles by evaluating their expression trees
    std::string style_str;
    for (const auto& child : node.children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            for (const auto& prop : styleNode->inline_properties) {
                ExpressionEvaluator evaluator(this->templates, this->doc_root);
                EvaluatedValue result = evaluator.evaluate(prop.value_expr.get(), &node);

                if (result.string_value.rfind("$(", 0) == 0) { // Check for reactive placeholder
                    std::string varName = result.string_value.substr(2, result.string_value.length() - 3);
                    html_output << " data-chtl-style-" << prop.key << "=\"" << varName << "\"";
                } else {
                    style_str += prop.key + ": ";
                    if (result.type == ValueType::STRING) {
                        style_str += result.string_value;
                    } else if (result.type == ValueType::NUMERIC) {
                        // Use a stringstream to format the number without trailing zeros
                        std::stringstream ss;
                        ss << result.numeric_value;
                        style_str += ss.str() + result.string_value;
                    }
                    style_str += ";";
                }
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

    // Process children
    for (const auto& child : node.children) {
        if (StyleNode* style_child = dynamic_cast<StyleNode*>(child.get())) {
            // Use the new visitor that passes the parent context
            visit(*style_child, &node);
        } else {
            child->accept(*this);
        }
    }
    html_output << "</" << node.tagName << ">";
}

void CHTLGenerator::visit(TextNode& node) {
    html_output << node.text;
}

void CHTLGenerator::visit(StyleNode& node) {
    // This overload exists to satisfy the Visitor interface.
    // The actual work is done in the version that takes the parent context.
    visit(node, nullptr);
}

void CHTLGenerator::visit(StyleNode& node, ElementNode* parent) {
    // This visitor method handles the generation of global CSS rules
    // that are defined within a local style block.
    if (!parent) return; // Cannot process a local style block without its parent element.

    for (const auto& rule : node.global_rules) {
        std::string selector = rule.selector;

        // Determine the parent selector for '&' replacement. ID is preferred.
        std::string parent_selector;
        if (!node.parent_element_id.empty()) {
            parent_selector = "#" + node.parent_element_id;
        } else if (!node.parent_element_class.empty()) {
            std::stringstream ss(node.parent_element_class);
            std::string first_class;
            ss >> first_class; // Use the first class if multiple exist
            parent_selector = "." + first_class;
        } else {
            // Fallback to the tag name if no id or class is available
            parent_selector = parent->tagName;
        }

        // Replace all occurrences of '&'
        size_t pos = selector.find('&');
        while (pos != std::string::npos) {
            selector.replace(pos, 1, parent_selector);
            pos = selector.find('&', pos + parent_selector.length());
        }

        css_output << selector << " {\n";
        for (const auto& prop : rule.properties) {
            ExpressionEvaluator evaluator(this->templates, this->doc_root);
            // Pass the parent element node to the evaluator for context
            EvaluatedValue result = evaluator.evaluate(prop.value_expr.get(), parent);
            css_output << "    " << prop.key << ": ";
            if (result.type == ValueType::STRING) {
                css_output << result.string_value;
            } else if (result.type == ValueType::NUMERIC) {
                std::stringstream ss;
                ss << result.numeric_value;
                css_output << ss.str() << result.string_value;
            }
            css_output << ";\n";
        }
        css_output << "}\n";
    }
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

void CHTLGenerator::visit(TemplateDeclarationNode& node) {
    // A template declaration does not produce any direct output.
    // It's processed by the parser and stored for use elsewhere.
    // So, this visitor method is intentionally empty.
}

void CHTLGenerator::visit(CustomDeclarationNode& node) {
    // A custom declaration does not produce any direct output.
    // It's processed by the parser and stored for use elsewhere.
    // So, this visitor method is intentionally empty.
}

void CHTLGenerator::visit(ImportNode& node) {
    // Imports are handled by the dispatcher, not the generator.
}

} // namespace CHTL
