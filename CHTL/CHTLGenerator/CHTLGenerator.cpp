#include "CHTLGenerator.h"
#include "../CHTLNode/Node.h" // Ensure full definitions are available
#include <iostream>
#include <algorithm>

CHTLGenerator::CHTLGenerator() {}

std::string CHTLGenerator::generate(Node& root) {
    root.accept(*this);

    // Combine HTML and CSS
    std::stringstream final_html;
    final_html << "<!DOCTYPE html>\n";

    std::string html_content = html_out.str();
    std::string css_content = css_out.str();

    // A more robust generator would properly merge into a <head> tag if it exists.
    // For now, we create a simple structure.
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
    // Start tag
    html_out << "<" << node.tagName;

    // Print attributes first
    for (auto& child : node.children) {
        if (dynamic_cast<AttributeNode*>(child.get())) {
            child->accept(*this);
        }
    }
    html_out << ">";

    // Then print nested content (elements, text, etc.)
    for (auto& child : node.children) {
        if (!dynamic_cast<AttributeNode*>(child.get())) {
            child->accept(*this);
        }
    }

    // End tag
    html_out << "</" << node.tagName << ">";
}

void CHTLGenerator::visit(AttributeNode& node) {
    if (in_style_block) {
        css_out << "  " << node.key << ": ";
        node.value->accept(*this);
        css_out << ";\n";
    } else {
        html_out << " " << node.key << "=";
        if (node.value->token.type == TokenType::STRING_LITERAL) {
            html_out << node.value->token.lexeme;
        } else {
            html_out << "\"" << node.value->token.lexeme << "\"";
        }
    }
}

void CHTLGenerator::visit(TextNode& node) {
    // The TextNode visitor is responsible for unquoting its content.
    html_out << unquote(node.content->token.lexeme);
}

void CHTLGenerator::visit(StyleNode& node) {
    bool originally_in_style = in_style_block;
    in_style_block = true;

    // The style node itself doesn't produce HTML output.
    // It populates the css_out stream by visiting its children.
    for (auto& prop : node.properties) {
        prop->accept(*this);
    }

    in_style_block = originally_in_style;
}

void CHTLGenerator::visit(ValueNode& node) {
    // This visitor is now only intended to be called for values inside a style block.
    // Other values (for attributes and text) are handled by their parent node's visitor.
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
