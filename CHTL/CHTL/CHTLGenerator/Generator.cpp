#include "Generator.h"
#include <stdexcept>
#include <unordered_set>
#include <vector>
#include <iostream>

namespace CHTL {

const std::unordered_set<std::string> self_closing_tags = {
    "area", "base", "br", "col", "embed", "hr", "img", "input",
    "link", "meta", "param", "source", "track", "wbr"
};

Generator::Generator(Context& context) : m_context(context) {}

GenerationResult Generator::generate(Program* program) {
    m_main_out.str("");
    m_main_out.clear();
    m_global_css.str("");
    m_global_css.clear();

    visit(program, m_main_out);

    GenerationResult result;
    result.html = m_main_out.str();
    result.global_css = m_global_css.str();
    return result;
}

void Generator::visit(Node* node, std::stringstream& out) {
    if (auto p = dynamic_cast<Program*>(node)) {
        for (const auto& stmt : p->statements) {
            visit(stmt.get(), out);
        }
    }
    else if (auto el = dynamic_cast<ElementNode*>(node)) { visit(el, out); }
    else if (auto txt = dynamic_cast<TextNode*>(node)) { visit(txt, out); }
    else if (auto cmt = dynamic_cast<CommentNode*>(node)) { visit(cmt, out); }
    else if (auto attr = dynamic_cast<AttributeNode*>(node)) { visit(attr, out); }
    else if (auto id = dynamic_cast<Identifier*>(node)) { visit(id, out); }
    else if (auto sl = dynamic_cast<StringLiteral*>(node)) { visit(sl, out); }
    else if (auto sn = dynamic_cast<StyleNode*>(node)) { visit(sn, out); }
    else if (auto ti = dynamic_cast<TemplateInstantiationNode*>(node)) { visit(ti, out); }
    else if (auto rb = dynamic_cast<RawBlockNode*>(node)) { visit(rb, out); }
    else if (auto sp = dynamic_cast<StylePropertyNode*>(node)) { visit(sp, out); }
    else if (auto ie = dynamic_cast<InfixExpression*>(node)) { visit(ie, out); }
    else if (auto vs = dynamic_cast<VariableSubstitutionNode*>(node)) { visit(vs, out); }
    // DeleteNode is not called from the main dispatcher, but from within TemplateInstantiation
}

void Generator::visit(DeleteNode* node, std::vector<std::string>& deleted_names) {
    for (const auto& target : node->targets) {
        deleted_names.push_back(target.literal);
    }
}

void Generator::visit(TemplateInstantiationNode* node, std::stringstream& out) {
    TemplateNode* template_def = m_context.getTemplate(node->name.literal);
    if (!template_def) return;

    // Step 1: Collect all properties from the base template into a mutable map
    std::map<std::string, Statement*> properties;
    for (const auto& stmt : template_def->body) {
        if (auto prop = dynamic_cast<StylePropertyNode*>(stmt.get())) {
            properties[prop->key.literal] = prop;
        }
    }

    // Step 2: Process specialization body
    std::vector<std::string> deleted_names;
    std::vector<Statement*> overrides;
    for (const auto& stmt : node->body) {
        if (auto delete_node = dynamic_cast<DeleteNode*>(stmt.get())) {
            visit(delete_node, deleted_names);
        } else {
            overrides.push_back(stmt.get());
        }
    }

    // Step 3: Apply deletions
    for (const auto& name : deleted_names) {
        properties.erase(name);
    }

    // Step 4: Apply overrides
    for (const auto& stmt : overrides) {
        if (auto prop = dynamic_cast<StylePropertyNode*>(stmt)) {
            properties[prop->key.literal] = prop;
        }
    }

    // Step 5: Generate the final properties
    for (const auto& pair : properties) {
        visit(pair.second, out);
    }
}

void Generator::visit(VariableSubstitutionNode* node, std::stringstream& out) {
    // This assumes the var_group is a simple Identifier.
    auto group_ident = dynamic_cast<Identifier*>(node->var_group.get());
    if (!group_ident) return;

    TemplateNode* var_group_template = m_context.getTemplate(group_ident->value);
    if (!var_group_template || var_group_template->type.literal != "@Var") {
        return;
    }

    for (const auto& stmt : var_group_template->body) {
        if (auto prop = dynamic_cast<StylePropertyNode*>(stmt.get())) {
            if (prop->key.literal == node->variable.value) {
                visit(prop->value.get(), out);
                return;
            }
        }
    }
}

void Generator::visit(InfixExpression* node, std::stringstream& out) {
    out << "calc(";
    visit(node->left.get(), out);
    out << " " << node->op << " ";
    visit(node->right.get(), out);
    out << ")";
}

void Generator::visit(Program* node, std::stringstream& out) {
    // This is now handled in the main visit method to avoid any potential recursion issues.
    // The logic is moved directly into the if-block for Program*.
}

void Generator::visit(ElementNode* node, std::stringstream& out) {
    std::string tagName = node->token.literal;
    std::stringstream inline_styles_stream;
    std::vector<Statement*> non_style_children;

    for (const auto& child : node->children) {
        if (auto style_node = dynamic_cast<StyleNode*>(child.get())) {
            visit(style_node, inline_styles_stream);
        } else {
            non_style_children.push_back(child.get());
        }
    }

    out << "<" << tagName;

    for (const auto& attr : node->attributes) {
        out << " ";
        visit(attr.get(), out);
    }

    std::string inline_style_str = inline_styles_stream.str();
    if (!inline_style_str.empty()) {
        out << " style=\"" << inline_style_str << "\"";
    }

    if (self_closing_tags.count(tagName)) {
        out << " /";
    }

    out << ">";

    if (!self_closing_tags.count(tagName)) {
        for (const auto& child : non_style_children) {
            visit(child, out);
        }
        out << "</" << tagName << ">";
    }
}

void Generator::visit(StyleNode* node, std::stringstream& inline_out) {
    for (const auto& style_child : node->children) {
        if (auto prop = dynamic_cast<StylePropertyNode*>(style_child.get())) {
            visit(prop, inline_out);
        } else if (auto rule = dynamic_cast<StyleRuleNode*>(style_child.get())) {
            visit(rule);
        } else if (auto ti = dynamic_cast<TemplateInstantiationNode*>(style_child.get())) {
            visit(ti, inline_out);
        }
    }
}

void Generator::visit(TemplateInstantiationNode* node, std::stringstream& out) {
    TemplateNode* template_def = m_context.getTemplate(node->name.literal);
    if (template_def) {
        for (const auto& stmt : template_def->body) {
            visit(stmt.get(), out);
        }
    }
}

void Generator::visit(StyleRuleNode* node) {
    m_global_css << node->selector.literal << " {\n";
    for (const auto& prop : node->properties) {
        m_global_css << "\t";
        visit(prop.get(), m_global_css);
        m_global_css << "\n";
    }
    m_global_css << "}\n";
}

void Generator::visit(StylePropertyNode* node, std::stringstream& out) {
    out << node->key.literal << ": ";
    visit(node->value.get(), out);
    out << ";";
}

void Generator::visit(TextNode* node, std::stringstream& out) {
    out << node->value;
}

void Generator::visit(CommentNode* node, std::stringstream& out) {
    if (node->token.type == TokenType::GEN_COMMENT) {
        out << "<!--" << node->value << "-->";
    }
}

void Generator::visit(AttributeNode* node, std::stringstream& out) {
    out << node->key.literal << "=\"";
    visit(node->value.get(), out);
    out << "\"";
}

void Generator::visit(Identifier* node, std::stringstream& out) {
    out << node->value;
}

void Generator::visit(StringLiteral* node, std::stringstream& out) {
    out << node->value;
}

void Generator::visit(RawBlockNode* node, std::stringstream& out) {
    if (node->type == RawBlockType::CSS) {
        out << "<style>\n" << node->content << "\n</style>\n";
    } else if (node->type == RawBlockType::JS) {
        out << "<script>\n" << node->content << "\n</script>\n";
    }
}

} // namespace CHTL
