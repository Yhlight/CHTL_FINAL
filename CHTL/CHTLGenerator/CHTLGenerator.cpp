#include "CHTLGenerator.h"
#include "../CHTLNode/Node.h" // Ensure full definitions are available
#include <iostream>
#include <algorithm>
#include <set>

CHTLGenerator::CHTLGenerator() {}

std::string CHTLGenerator::generate(Node& root) {
    root.accept(*this);

    // Combine HTML and CSS
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

void CHTLGenerator::visit(ProgramNode& node) {
    for (auto& child : node.children) {
        if (child) child->accept(*this);
    }
}

void CHTLGenerator::visit(ElementNode& node) {
    // --- Collection Phase ---
    std::vector<AttributeNode*> other_attributes;
    std::set<std::string> class_names;
    std::string id_value;
    StyleNode* style_node = nullptr;

    // 1. Collect explicit attributes and find the style node
    for (auto& child : node.children) {
        if (auto* attr_node = dynamic_cast<AttributeNode*>(child.get())) {
            if (attr_node->key == "id") {
                id_value = unquote(attr_node->value->token.lexeme);
            } else if (attr_node->key == "class") {
                std::stringstream ss(unquote(attr_node->value->token.lexeme));
                std::string individual_class;
                while (ss >> individual_class) {
                    class_names.insert(individual_class);
                }
            } else {
                other_attributes.push_back(attr_node);
            }
        } else if (auto* sn = dynamic_cast<StyleNode*>(child.get())) {
            style_node = sn;
        }
    }

    // 2. Collect implicit attributes from selectors
    if (style_node) {
        for (auto& style_child : style_node->children) {
            if (auto* selector_node = dynamic_cast<SelectorNode*>(style_child.get())) {
                const std::string& selector = selector_node->selector;
                if (selector.rfind('.', 0) == 0) {
                    class_names.insert(selector.substr(1));
                } else if (selector.rfind('#', 0) == 0) {
                    if (id_value.empty()) {
                        id_value = selector.substr(1);
                    }
                }
            }
        }
    }

    // 3. Determine the contextual selector for '&' replacement
    std::string contextual_selector_for_ampersand;
    if (!id_value.empty()) {
        contextual_selector_for_ampersand = "#" + id_value;
    } else if (!class_names.empty()) {
        contextual_selector_for_ampersand = "." + *class_names.begin();
    }

    // --- Generation Phase ---
    html_out << "<" << node.tagName;

    // 4. Print attributes
    if (!id_value.empty()) {
        html_out << " id=\"" << id_value << "\"";
    }
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
        html_out << " " << attr_node->key << "=";
        if (attr_node->value->token.type == TokenType::STRING_LITERAL) {
            html_out << attr_node->value->token.lexeme;
        } else {
            html_out << "\"" << attr_node->value->token.lexeme << "\"";
        }
    }

    std::stringstream inline_style_ss;
    if (style_node) {
        for (auto& style_child : style_node->children) {
            if (auto* attr_node = dynamic_cast<AttributeNode*>(style_child.get())) {
                inline_style_ss << attr_node->key << ": " << unquote(attr_node->value->token.lexeme) << "; ";
            }
        }
    }
    if (!inline_style_ss.str().empty()) {
        html_out << " style=\"" << inline_style_ss.str() << "\"";
    }

    html_out << ">";

    // 5. Visit children for nested content and global styles
    std::string old_selector = this->current_element_selector;
    this->current_element_selector = contextual_selector_for_ampersand;
    for (auto& child : node.children) {
        if (!dynamic_cast<AttributeNode*>(child.get())) {
            child->accept(*this);
        }
    }
    this->current_element_selector = old_selector;

    html_out << "</" << node.tagName << ">";
}

void CHTLGenerator::visit(AttributeNode& node) {
    if (in_style_block) {
        css_out << "  " << node.key << ": ";
        node.value->accept(*this);
        css_out << ";\n";
    }
}

void CHTLGenerator::visit(TextNode& node) {
    html_out << unquote(node.content->token.lexeme);
}

void CHTLGenerator::visit(StyleNode& node) {
    for (auto& child : node.children) {
        if (dynamic_cast<SelectorNode*>(child.get())) {
            child->accept(*this);
        }
    }
}

void CHTLGenerator::visit(SelectorNode& node) {
    bool originally_in_style = in_style_block;
    in_style_block = true;

    std::string final_selector = node.selector;
    size_t pos = final_selector.find('&');
    if (pos != std::string::npos && !this->current_element_selector.empty()) {
        final_selector.replace(pos, 1, this->current_element_selector);
    }

    css_out << final_selector << " {\n";
    for (auto& prop : node.properties) {
        prop->accept(*this);
    }
    css_out << "}\n";

    in_style_block = originally_in_style;
}

void CHTLGenerator::visit(ValueNode& node) {
    if (in_style_block) {
        css_out << node.token.lexeme;
    }
}

std::string CHTLGenerator::unquote(const std::string& s) {
    if (s.length() >= 2 && s.front() == '"' && s.back() == '"') {
        return s.substr(1, s.length() - 2);
    }
    if (s.length() >= 2 && s.front() == '\'' && s.back() == '\'') {
        return s.substr(1, s.length() - 2);
    }
    return s;
}
