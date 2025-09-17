#include "AstPrinter.h"
#include "../CHTL/Node/RootNode.h"
#include "../CHTL/Node/ElementNode.h"
#include "../CHTL/Node/AttributeNode.h"
#include "../CHTL/Node/TextNode.h"
#include "../CHTL/Node/StyleNode.h"
#include <iostream>

namespace CHTL {

std::string AstPrinter::print(const std::shared_ptr<BaseNode>& node) {
    m_output = "";
    visit(node, 0);
    return m_output;
}

void AstPrinter::visit(const std::shared_ptr<BaseNode>& node, int indentLevel) {
    std::string indent(indentLevel * 2, ' ');

    if (auto rootNode = std::dynamic_pointer_cast<RootNode>(node)) {
        m_output += indent + "RootNode\n";
        for (const auto& child : rootNode->children) {
            visit(child, indentLevel + 1);
        }
    } else if (auto elementNode = std::dynamic_pointer_cast<ElementNode>(node)) {
        m_output += indent + "ElementNode <" + elementNode->tagName + ">\n";
        for (const auto& attr : elementNode->attributes) {
             m_output += indent + "  - Attr: " + attr->key + " = \"" + attr->value + "\"\n";
        }
        for (const auto& child : elementNode->children) {
            visit(child, indentLevel + 1);
        }
    } else if (auto textNode = std::dynamic_pointer_cast<TextNode>(node)) {
        m_output += indent + "TextNode: \"" + textNode->content + "\"\n";
    } else if (auto styleNode = std::dynamic_pointer_cast<StyleNode>(node)) {
        m_output += indent + "StyleNode\n";
        for (const auto& prop : styleNode->properties) {
            m_output += indent + "  - Prop: " + prop.key + ": " + prop.value + ";\n";
        }
    } else {
        m_output += indent + "UnknownNode\n";
    }
}

} // namespace CHTL
