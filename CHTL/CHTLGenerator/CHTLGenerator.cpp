#include "CHTLGenerator.h"
#include "../CHTLNode/Node.h" // Ensure full definitions are available
#include <iostream>
#include <algorithm>
#include <set>
#include <map>

CHTLGenerator::CHTLGenerator() {}

std::string CHTLGenerator::generate(Node& root) {
    collectTemplateDefinitions(&root);
    root.accept(*this);

    std::stringstream final_html;
    final_html << "<!DOCTYPE html>\n";
    std::string html_content = html_out.str();
    std::string css_content = css_out.str();
    final_html << "<html>\n<head>\n";
    if (!css_content.empty()) {
        final_html << "<style>\n" << css_content << "</style>\n";
    }
    final_html << "</head>\n<body>\n";
    final_html << html_content;
    final_html << "\n</body>\n</html>";
    return final_html.str();
}

void CHTLGenerator::collectTemplateDefinitions(Node* node) {
    if (!node) return;
    if (auto* def_node = dynamic_cast<TemplateDefinitionNode*>(node)) {
        template_table[def_node->name.lexeme] = def_node;
    }
    if (auto* p_node = dynamic_cast<ProgramNode*>(node)) {
        for (auto& child : p_node->children) collectTemplateDefinitions(child.get());
    } else if (auto* e_node = dynamic_cast<ElementNode*>(node)) {
        for (auto& child : e_node->children) collectTemplateDefinitions(child.get());
    } else if (auto* s_node = dynamic_cast<StyleNode*>(node)) {
        for (auto& child : s_node->children) collectTemplateDefinitions(child.get());
    }
}

std::string CHTLGenerator::generateExpression(ExpressionNode* node, bool is_top_level) {
    if (auto* literal = dynamic_cast<LiteralNode*>(node)) {
        return literal->token.lexeme;
    }
    if (auto* binary = dynamic_cast<BinaryOpNode*>(node)) {
        std::string left = generateExpression(binary->left.get(), false);
        std::string right = generateExpression(binary->right.get(), false);
        std::string content = left + " " + binary->op.lexeme + " " + right;
        return is_top_level ? "calc(" + content + ")" : "(" + content + ")";
    }
    return "";
}

// --- Visitor Implementations ---

void CHTLGenerator::visit(ProgramNode& node) {
    for (auto& child : node.children) {
        if (child) child->accept(*this);
    }
}

void CHTLGenerator::visit(ElementNode& node) {
    std::vector<AttributeNode*> other_attributes;
    std::set<std::string> class_names;
    std::string id_value;
    StyleNode* style_node = nullptr;

    for (auto& child : node.children) {
        if (auto* attr_node = dynamic_cast<AttributeNode*>(child.get())) {
            if (attr_node->key == "id") {
                 if(auto* literal = dynamic_cast<LiteralNode*>(attr_node->value.get())) id_value = unquote(literal->token.lexeme);
            } else if (attr_node->key == "class") {
                if(auto* literal = dynamic_cast<LiteralNode*>(attr_node->value.get())) {
                    std::stringstream ss(unquote(literal->token.lexeme));
                    std::string individual_class;
                    while (ss >> individual_class) class_names.insert(individual_class);
                }
            } else {
                other_attributes.push_back(attr_node);
            }
        } else if (auto* sn = dynamic_cast<StyleNode*>(child.get())) {
            style_node = sn;
        }
    }

    if (style_node) {
        for (auto& style_child : style_node->children) {
            if (auto* selector_node = dynamic_cast<SelectorNode*>(style_child.get())) {
                const std::string& selector = selector_node->selector;
                if (selector.rfind('.', 0) == 0) class_names.insert(selector.substr(1));
                else if (selector.rfind('#', 0) == 0) {
                    if (id_value.empty()) id_value = selector.substr(1);
                }
            }
        }
    }

    html_out << "<" << node.tagName;

    if (!id_value.empty()) html_out << " id=\"" << id_value << "\"";
    if (!class_names.empty()) {
        html_out << " class=\"";
        bool first = true;
        for (const auto& cls : class_names) {
            if (!first) html_out << " ";
            html_out << cls;
            first = false;
        }
        html_out << "\"";
    }
    for (auto* attr_node : other_attributes) {
        attr_node->accept(*this);
    }

    std::stringstream inline_style_ss;
    if (style_node) {
        auto* old_stream = active_css_stream;
        active_css_stream = &inline_style_ss;
        generating_inline_style = true;

        style_node->accept(*this);

        generating_inline_style = false;
        active_css_stream = old_stream;
    }
    if (!inline_style_ss.str().empty()) {
        html_out << " style=\"" << inline_style_ss.str() << "\"";
    }

    html_out << ">";

    std::string old_selector = this->current_element_selector;
    this->current_element_selector = !id_value.empty() ? "#" + id_value : (!class_names.empty() ? "." + *class_names.begin() : "");
    for (auto& child : node.children) {
        if (!dynamic_cast<AttributeNode*>(child.get())) {
            child->accept(*this);
        }
    }
    this->current_element_selector = old_selector;

    html_out << "</" << node.tagName << ">";
}

void CHTLGenerator::visit(AttributeNode& node) {
    if (generating_inline_style) {
        (*active_css_stream) << node.key << ": " << generateExpression(node.value.get()) << "; ";
    }
    else if (in_style_block) {
        (*active_css_stream) << "  " << node.key << ": " << generateExpression(node.value.get()) << ";\n";
    } else {
        html_out << " " << node.key << "=";
        if(auto* literal = dynamic_cast<LiteralNode*>(node.value.get())) {
            if (literal->token.type == TokenType::STRING_LITERAL) {
                html_out << literal->token.lexeme;
            } else {
                html_out << "\"" << literal->token.lexeme << "\"";
            }
        }
    }
}

void CHTLGenerator::visit(TextNode& node) {
    if(auto* literal = dynamic_cast<LiteralNode*>(node.content.get())) {
        html_out << unquote(literal->token.lexeme);
    }
}

void CHTLGenerator::visit(StyleNode& node) {
    for (auto& child : node.children) {
        if (generating_inline_style) {
            if (dynamic_cast<AttributeNode*>(child.get()) || dynamic_cast<TemplateUsageNode*>(child.get())) {
                 child->accept(*this);
            }
        } else {
            if (dynamic_cast<SelectorNode*>(child.get())) {
                child->accept(*this);
            }
        }
    }
}

void CHTLGenerator::visit(SelectorNode& node) {
    auto* old_stream = active_css_stream;
    active_css_stream = &css_out;
    bool originally_in_style = in_style_block;
    in_style_block = true;

    std::string final_selector = node.selector;
    size_t pos = final_selector.find('&');
    if (pos != std::string::npos && !this->current_element_selector.empty()) {
        final_selector.replace(pos, 1, this->current_element_selector);
    }
    (*active_css_stream) << final_selector << " {\n";
    for (auto& prop : node.properties) {
        prop->accept(*this);
    }
    (*active_css_stream) << "}\n";

    in_style_block = originally_in_style;
    active_css_stream = old_stream;
}

void CHTLGenerator::visit(TemplateDefinitionNode& node) {
    // Handled in pre-pass
}

void CHTLGenerator::visit(TemplateUsageNode& node) {
    if (template_table.count(node.name.lexeme) == 0) {
        std::cerr << "Error: Template '" << node.name.lexeme << "' not found." << std::endl;
        return;
    }
    TemplateDefinitionNode* def = template_table[node.name.lexeme];

    // Simple case: no customization body, just inline the definition
    if (node.body.empty()) {
        for (auto& child : def->children) {
            child->accept(*this);
        }
        return;
    }

    // Customization logic for @Style templates
    if (def->template_type.type == TokenType::AT_STYLE) {
        std::map<std::string, ExpressionNode*> final_properties;

        // 1. Load base properties
        for (auto& child : def->children) {
            if (auto* attr = dynamic_cast<AttributeNode*>(child.get())) {
                final_properties[attr->key] = attr->value.get();
            }
        }

        // 2. Apply customizations from usage body
        for (auto& rule : node.body) {
            if (auto* attr = dynamic_cast<AttributeNode*>(rule.get())) {
                final_properties[attr->key] = attr->value.get(); // Add or overwrite
            } else if (auto* del = dynamic_cast<DeleteNode*>(rule.get())) {
                final_properties.erase(del->identifier.lexeme); // Delete
            }
        }

        // 3. Generate the final CSS
        for (const auto& pair : final_properties) {
            if (pair.second != nullptr) { // Only generate properties that have a value
                 if (generating_inline_style) {
                    (*active_css_stream) << pair.first << ": " << generateExpression(pair.second) << "; ";
                 } else if (in_style_block) {
                    (*active_css_stream) << "  " << pair.first << ": " << generateExpression(pair.second) << ";\n";
                 }
            }
        }
    }
    // Logic for @Element customization would go here
}

void CHTLGenerator::visit(DeleteNode& node) {
    // This node is handled inside visit(TemplateUsageNode&), does nothing on its own.
}


std::string CHTLGenerator::unquote(const std::string& s) {
    if (s.length() >= 2 && (s.front() == '"' || s.front() == '\'') && s.front() == s.back()) {
        return s.substr(1, s.length() - 2);
    }
    return s;
}
