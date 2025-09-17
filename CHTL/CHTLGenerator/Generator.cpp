#include "Generator.h"
#include "StyleNode.h"
#include "TextNode.h"
#include "ElementNode.h"
#include "TemplateUsageNode.h"
#include "StylePropertyNode.h"
#include "TemplateDefinitionNode.h" // Needed for accessing template_def->body

namespace CHTL {

Generator::Generator(const Program& program) : m_program(program) {}

std::string Generator::Generate() {
    visit(&m_program);
    return m_output.str();
}

void Generator::visit(const Node* node) {
    if (auto p = dynamic_cast<const Program*>(node)) {
        for (const auto& stmt : p->statements) {
            visit(stmt.get());
        }
    } else if (auto e = dynamic_cast<const ElementNode*>(node)) {
        m_output << "<" << e->tagName;

        for (const auto& attr_pair : e->attributes) {
            m_output << " " << attr_pair.first << "=\"" << attr_pair.second << "\"";
        }

        std::string style_string;
        for (const auto& child : e->children) {
            if (auto style_node = dynamic_cast<const StyleNode*>(child.get())) {
                for (const auto& item : style_node->items) {
                    if (auto prop_node = dynamic_cast<const StylePropertyNode*>(item.get())) {
                        style_string += prop_node->key + ":" + prop_node->value + ";";
                    } else if (auto usage_node = dynamic_cast<const TemplateUsageNode*>(item.get())) {
                        // Look up the template in the registry
                        auto it = m_program.templateRegistry.find(usage_node->name);
                        if (it != m_program.templateRegistry.end()) {
                            const auto& template_def = it->second;
                            if (auto template_style_body = dynamic_cast<const StyleNode*>(template_def->body.get())) {
                                for (const auto& template_item : template_style_body->items) {
                                    if (auto template_prop = dynamic_cast<const StylePropertyNode*>(template_item.get())) {
                                        style_string += template_prop->key + ":" + template_prop->value + ";";
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        if (!style_string.empty()) {
            m_output << " style=\"" << style_string << "\"";
        }

        m_output << ">";

        for (const auto& child : e->children) {
            if (dynamic_cast<const StyleNode*>(child.get()) == nullptr) {
                visit(child.get());
            }
        }
        m_output << "</" << e->tagName << ">";

    } else if (auto t = dynamic_cast<const TextNode*>(node)) {
        m_output << t->value;
    }
    // StyleNode is handled within ElementNode logic, so it doesn't need its own case.
}

} // namespace CHTL
