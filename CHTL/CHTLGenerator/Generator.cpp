#include "Generator.h"
#include <stdexcept>
namespace CHTL {

std::string Generator::generate(RootNode* root) {
    if (!root) return "";
    // Stage 1: Generate CSS from global rules
    std::string css_rules = generate_global_styles(root);
    // Stage 2: Generate HTML body content
    std::stringstream body_stream;
    for (const auto& child : root->children) {
        generate_body_content(child.get(), body_stream);
    }
    // Stage 3: Assemble final document
    std::stringstream final_doc;
    final_doc << "<!DOCTYPE html>\n";
    final_doc << "<html>\n";
    final_doc << "<head>\n";
    if (!css_rules.empty()) {
        final_doc << "  <style>\n" << css_rules << "  </style>\n";
    }
    final_doc << "</head>\n";
    final_doc << "<body>\n" << body_stream.str() << "</body>\n";
    final_doc << "</html>\n";
    return final_doc.str();
}

std::string Generator::generate_global_styles(RootNode* root) {
    std::stringstream css_stream;
    for (const auto& rule : root->global_css_rules) {
        css_stream << "    " << rule->selector << " {\n";
        for (const auto& prop : rule->properties) {
            css_stream << "      " << prop->name << ": ";
            if (auto* id_val = dynamic_cast<IdentifierNode*>(prop->value.get())) { css_stream << id_val->value; }
            else if (auto* str_val = dynamic_cast<StringLiteralNode*>(prop->value.get())) { css_stream << str_val->value; }
            css_stream << ";\n";
        }
        css_stream << "    }\n";
    }
    return css_stream.str();
}

void Generator::generate_body_content(Node* node, std::stringstream& stream) {
    if (!node) return;
    if (auto* el = dynamic_cast<ElementNode*>(node)) { visit_element(el, stream); }
    else if (auto* txt = dynamic_cast<TextNode*>(node)) { visit_text(txt, stream); }
}

void Generator::indent(std::stringstream& stream) { stream << std::string(m_indent_level * 2, ' '); }

void Generator::visit_element(ElementNode* node, std::stringstream& stream) {
    indent(stream); stream << "<" << node->tagName;
    for (const auto& attr : node->attributes) {
        stream << " " << attr->name << "=\"";
        if (auto* id_val = dynamic_cast<IdentifierNode*>(attr->value.get())) { stream << id_val->value; }
        else if (auto* str_val = dynamic_cast<StringLiteralNode*>(attr->value.get())) { stream << str_val->value; }
        stream << "\"";
    }
    if (node->styleBlock) {
        stream << " style=\"";
        for (const auto& prop : node->styleBlock->inline_properties) {
            stream << prop->name << ": ";
            if (auto* id_val = dynamic_cast<IdentifierNode*>(prop->value.get())) { stream << id_val->value; }
            else if (auto* str_val = dynamic_cast<StringLiteralNode*>(prop->value.get())) { stream << str_val->value; }
            stream << "; ";
        }
        stream << "\"";
    }
    if (node->children.empty()) { stream << " />\n"; }
    else {
        stream << ">\n"; m_indent_level++;
        for (auto& child : node->children) { generate_body_content(child.get(), stream); }
        m_indent_level--; indent(stream); stream << "</" << node->tagName << ">\n";
    }
}
void Generator::visit_text(TextNode* node, std::stringstream& stream) {
    indent(stream);
    if (auto* id_val = dynamic_cast<IdentifierNode*>(node->textValue.get())) { stream << id_val->value << "\n"; }
    else if (auto* str_val = dynamic_cast<StringLiteralNode*>(node->textValue.get())) { stream << str_val->value << "\n"; }
}
}
