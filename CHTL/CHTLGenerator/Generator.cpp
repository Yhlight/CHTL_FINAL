#include "Generator.h"
#include <iostream>

// Initialize the set of HTML5 void elements.
const std::set<std::string> Generator::void_elements = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

Generator::Generator() {}

std::string Generator::generate(ProgramNode& root) {
    html_buffer.str("");
    html_buffer.clear();
    css_buffer.str("");
    css_buffer.clear();
    indent_level = 0;
    root.accept(*this);
    return html_buffer.str();
}

void Generator::do_indent() {
    for (int i = 0; i < indent_level; ++i) {
        html_buffer << "  "; // 2 spaces for indentation
    }
}

void Generator::visit(ProgramNode& node) {
    for (const auto& child : node.children) {
        child->accept(*this);
    }
}

void Generator::visit(ElementNode& node) {
    // Handle style blocks separately later. For now, treat as a normal element.
    do_indent();
    html_buffer << "<" << node.tagName.lexeme;
    for (const auto& attr : node.attributes) {
        html_buffer << " ";
        attr->accept(*this);
    }

    const bool is_void = void_elements.count(node.tagName.lexeme) > 0;

    if (node.children.empty() && is_void) {
        html_buffer << ">\n";
        return;
    }

    html_buffer << ">\n";
    indent_level++;
    for (const auto& child : node.children) {
        child->accept(*this);
    }
    indent_level--;

    if (!is_void) {
        do_indent();
        html_buffer << "</" << node.tagName.lexeme << ">\n";
    }
}

void Generator::visit(AttributeNode& node) {
    value_buffer.str("");
    value_buffer.clear();
    node.value->accept(*this);
    html_buffer << node.name.lexeme << "=\"" << value_buffer.str() << "\"";
}

void Generator::visit(TextNode& node) {
    value_buffer.str("");
    value_buffer.clear();
    node.value->accept(*this);
    do_indent();
    html_buffer << value_buffer.str() << "\n";
}

void Generator::visit(CommentNode& node) {
    std::string comment_text = node.comment.lexeme;
    if (!comment_text.empty() && comment_text[0] == '#') {
        comment_text = comment_text.substr(1);
        size_t first = comment_text.find_first_not_of(" \t");
        if (std::string::npos != first) {
            comment_text = comment_text.substr(first);
        }
    }
    do_indent();
    html_buffer << "<!-- " << comment_text << " -->\n";
}

void Generator::visit(StringLiteralNode& node) {
    // The lexeme includes the quotes, so we strip them.
    std::string val = node.value.lexeme;
    if (val.length() >= 2) {
        value_buffer << val.substr(1, val.length() - 2);
    }
}

void Generator::visit(UnquotedLiteralNode& node) {
    for (size_t i = 0; i < node.tokens.size(); ++i) {
        value_buffer << node.tokens[i].lexeme;
        if (i < node.tokens.size() - 1) {
            value_buffer << " ";
        }
    }
}

void Generator::visit(NumberLiteralNode& node) {
    value_buffer << node.value.lexeme;
}
