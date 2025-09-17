#include "Generator.h"
#include "StyleNode.h"
#include "TextNode.h"
#include "ElementNode.h"
// Program is defined in Node.h, which is included via Generator.h

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
                for (const auto& prop : style_node->properties) {
                    style_string += prop.first + ":" + prop.second + ";";
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
