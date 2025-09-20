#include "CHTLGenerator.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/OriginNode.h"
#include "../CHTLNode/TemplateDeclarationNode.h"
#include "../CHTLNode/CustomDeclarationNode.h"
#include "../CHTLNode/NamespaceNode.h"
#include "../CHTLNode/TemplateUsageNode.h"
#include "../Expression/ExpressionEvaluator.h" // Include the new evaluator
#include "../CHTLContext.h"
#include <unordered_set>
#include <algorithm> // For std::find_if
#include <sstream>   // For std::stringstream

namespace CHTL {

const std::unordered_set<std::string> voidElements = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

CHTLGenerator::CHTLGenerator(CHTLContext& context)
    : context(context) {}

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
    // --- Automation and Global CSS Generation ---
    for (const auto& child : node.children) {
        if (StyleNode* styleNode = dynamic_cast<StyleNode*>(child.get())) {
            for (const auto& rule : styleNode->global_rules) {
                std::string selector = rule.selector;
                // Automation: if selector is a simple class or id, inject it.
                if (selector.rfind('.', 0) == 0) { // Starts with .
                    size_t end = 1;
                    while (end < selector.length() && isalnum(selector[end])) {
                        end++;
                    }
                    std::string className = selector.substr(1, end - 1);

                    auto it = std::find_if(node.attributes.begin(), node.attributes.end(),
                                           [](const HtmlAttribute& attr){ return attr.key == "class"; });
                    if (it != node.attributes.end()) {
                        if (it->value.find(className) == std::string::npos) {
                            it->value += " " + className;
                        }
                    } else {
                        node.attributes.push_back({"class", className});
                    }
                } else if (selector.rfind('#', 0) == 0) { // Starts with #
                     size_t end = 1;
                    while (end < selector.length() && isalnum(selector[end])) {
                        end++;
                    }
                    std::string idName = selector.substr(1, end - 1);
                    auto it = std::find_if(node.attributes.begin(), node.attributes.end(),
                                           [](const HtmlAttribute& attr){ return attr.key == "id"; });
                    if (it != node.attributes.end()) {
                        it->value = idName;
                    } else {
                        node.attributes.push_back({"id", idName});
                    }
                }

                size_t pos = selector.find('&');
                if (pos != std::string::npos) {
                    std::string primary_selector;
                    auto id_it = std::find_if(node.attributes.begin(), node.attributes.end(),
                                              [](const HtmlAttribute& attr){ return attr.key == "id"; });
                    if (id_it != node.attributes.end()) {
                        primary_selector = "#" + id_it->value;
                    } else {
                        auto class_it = std::find_if(node.attributes.begin(), node.attributes.end(),
                                                     [](const HtmlAttribute& attr){ return attr.key == "class"; });
                        if (class_it != node.attributes.end()) {
                            std::string first_class = class_it->value.substr(0, class_it->value.find(' '));
                            primary_selector = "." + first_class;
                        } else {
                            primary_selector = node.tagName;
                        }
                    }
                    selector.replace(pos, 1, primary_selector);
                }

                css_output << selector << " {\n";
                for (const auto& prop : rule.properties) {
                    ExpressionEvaluator evaluator(this->context, this->doc_root);
                    EvaluatedValue result = evaluator.evaluate(prop.value_expr.get(), &node);
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
    }

    // --- HTML Tag Generation ---
    html_output << "<" << node.tagName;

    for (const auto& attr : node.attributes) {
        html_output << " " << attr.key << "=\"" << attr.value << "\"";
    }

    // Resolve all styles (templates, specializations, and inline) for this element
    std::vector<AttributeNode> final_styles = context.resolveStyles(&node);

    std::string style_str;
    // Generate style string from the final, processed properties
    for (const auto& prop : final_styles) {
        if (!prop.value_expr) continue; // Skip valueless properties
        ExpressionEvaluator evaluator(this->context, this->doc_root);
        EvaluatedValue result = evaluator.evaluate(prop.value_expr.get(), &node);
        style_str += prop.key + ": ";
        if (result.type == ValueType::STRING) {
            style_str += result.string_value;
        } else if (result.type == ValueType::NUMERIC) {
            std::stringstream ss;
            ss << result.numeric_value;
            style_str += ss.str() + result.string_value;
        }
        style_str += ";";
    }

    if (!style_str.empty()) {
        html_output << " style=\"" << style_str << "\"";
    }

    if (voidElements.find(node.tagName) != voidElements.end()) {
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
    // Import nodes are handled by the CHTLCompiler and do not generate
    // any direct output themselves, so this visitor is empty.
}

void CHTLGenerator::visit(ScriptNode& node) {
    // For now, we do nothing. This content will be passed to the JS compiler.
    // In a future implementation, this might generate a <script> tag if it contains plain JS.
}

void CHTLGenerator::visit(NamespaceNode& node) {
    // A namespace declaration does not produce any direct output.
    // It's processed by the parser and stored for use elsewhere.
    // The generator might need to handle children if they can exist outside of a root element.
    for (const auto& child : node.children) {
        child->accept(*this);
    }
}

void CHTLGenerator::visit(TemplateUsageNode& node) {
    const TemplateDefinitionNode* def = context.getTemplateDefinition(node.template_name, node.from_namespace);
    if (def) {
        for (const auto& child : def->element_body) {
            child->accept(*this);
        }
    }
    // Note: Error handling for not found templates should be in the generator or a linking phase.
}

} // namespace CHTL
