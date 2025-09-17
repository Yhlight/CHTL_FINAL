#include "Generator.h"
#include <iostream>

// --- TemplateCollector Implementation ---

TemplateCollector::TemplateCollector(
    std::map<std::string, TemplateStyleNode*>& st,
    std::map<std::string, TemplateElementNode*>& et
) : style_templates(st), element_templates(et) {}

void TemplateCollector::collect(ProgramNode& root) {
    root.accept(*this);
}

void TemplateCollector::visit(ProgramNode& node) {
    for (const auto& child : node.children) {
        child->accept(*this);
    }
}

// We don't want to recurse into elements during the collection phase,
// so this method is intentionally shallow.
void TemplateCollector::visit(ElementNode& node) {
    // This visitor only cares about top-level template definitions.
}

void TemplateCollector::visit(TemplateStyleNode& node) {
    style_templates[node.name.lexeme] = &node;
}

void TemplateCollector::visit(TemplateElementNode& node) {
    element_templates[node.name.lexeme] = &node;
}


// --- Generator Implementation ---

const std::set<std::string> Generator::void_elements = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

Generator::Generator() {}

std::string Generator::generateHTML(ProgramNode& root) {
    // Clear state
    html_buffer.str("");
    html_buffer.clear();
    css_buffer.str("");
    css_buffer.clear();
    style_templates.clear();
    element_templates.clear();
    indent_level = 0;

    // 1. Pre-pass to collect all template definitions
    TemplateCollector collector(style_templates, element_templates);
    collector.collect(root);

    // 2. Main generation pass
    root.accept(*this);

    return html_buffer.str();
}

std::string Generator::getCSS() {
    return css_buffer.str();
}

void Generator::do_indent() {
    for (int i = 0; i < indent_level; ++i) {
        html_buffer << "  ";
    }
}

void Generator::generateTokenSequence(std::stringstream& buffer, const std::vector<Token>& tokens) {
    for (size_t i = 0; i < tokens.size(); ++i) {
        buffer << tokens[i].lexeme;
        if (i < tokens.size() - 1) {
            TokenType current_type = tokens[i].type;
            TokenType next_type = tokens[i+1].type;
            bool add_space = true;
            if (current_type == TokenType::NUMBER && next_type == TokenType::IDENTIFIER) add_space = false;
            else if (current_type == TokenType::DOT || current_type == TokenType::HASH || current_type == TokenType::AMPERSAND) add_space = false;
            else if (next_type == TokenType::MINUS && current_type == TokenType::IDENTIFIER) add_space = false;
            else if (current_type == TokenType::MINUS && next_type == TokenType::IDENTIFIER) add_space = false;
            else if (next_type == TokenType::COLON) add_space = false;
            else if (current_type == TokenType::COLON) add_space = false;
            if (add_space) buffer << " ";
        }
    }
}

void Generator::visit(ProgramNode& node) {
    for (const auto& child : node.children) {
        // Skip template definitions in the main generation pass
        if (dynamic_cast<TemplateDefinitionNode*>(child.get())) {
            continue;
        }
        child->accept(*this);
    }
}

void Generator::visit(ElementNode& node) {
    do_indent();
    html_buffer << "<" << node.tagName.lexeme;
    for (const auto& attr : node.attributes) {
        html_buffer << " ";
        attr->accept(*this);
    }

    if (node.styleBlock) {
        std::stringstream inline_styles_buffer;
        html_buffer.swap(inline_styles_buffer);
        style_context = StyleContext::INLINE;
        node.styleBlock->accept(*this);
        html_buffer.swap(inline_styles_buffer);
        std::string styles = inline_styles_buffer.str();
        if (!styles.empty()) {
            if (styles.back() == ' ') styles.pop_back();
            html_buffer << " style=\"" << styles << "\"";
        }
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
        if (std::string::npos != first) comment_text = comment_text.substr(first);
    }
    do_indent();
    html_buffer << "<!-- " << comment_text << " -->\n";
}

void Generator::visit(StringLiteralNode& node) {
    std::string val = node.value.lexeme;
    if (val.length() >= 2) value_buffer << val.substr(1, val.length() - 2);
}

void Generator::visit(UnquotedLiteralNode& node) {
    generateTokenSequence(value_buffer, node.tokens);
}

void Generator::visit(NumberLiteralNode& node) {
    value_buffer << node.value.lexeme;
}

void Generator::visit(StyleBlockNode& node) {
    for (const auto& content : node.contents) {
        content->accept(*this);
    }
}

void Generator::visit(StylePropertyNode& node) {
    value_buffer.str("");
    value_buffer.clear();
    node.value->accept(*this);
    std::stringstream* target_buffer = (style_context == StyleContext::INLINE) ? &html_buffer : &css_buffer;
    if (style_context == StyleContext::GLOBAL_RULE) *target_buffer << "  ";
    *target_buffer << node.name.lexeme << ": " << value_buffer.str() << ";";
    if (style_context == StyleContext::INLINE) *target_buffer << " ";
    else *target_buffer << "\n";
}

void Generator::visit(StyleRuleNode& node) {
    StyleContext original_context = style_context;
    style_context = StyleContext::GLOBAL_RULE;
    generateTokenSequence(css_buffer, node.selector);
    css_buffer << " {\n";
    for (const auto& prop : node.properties) {
        prop->accept(*this);
    }
    css_buffer << "}\n\n";
    style_context = original_context;
}

void Generator::visit(TemplateStyleNode& node) { /* Handled by collector */ }
void Generator::visit(TemplateElementNode& node) { /* Handled by collector */ }

void Generator::visit(UseStyleNode& node) {
    if (style_templates.count(node.name.lexeme)) {
        TemplateStyleNode* tpl = style_templates[node.name.lexeme];
        for (const auto& prop : tpl->properties) {
            prop->accept(*this);
        }
    } else {
        // Handle error: template not found
    }
}

void Generator::visit(UseElementNode& node) {
    if (element_templates.count(node.name.lexeme)) {
        TemplateElementNode* tpl = element_templates[node.name.lexeme];
        for (const auto& child : tpl->children) {
            child->accept(*this);
        }
    } else {
        // Handle error: template not found
    }
}
