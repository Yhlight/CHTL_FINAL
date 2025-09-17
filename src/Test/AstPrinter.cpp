#include "AstPrinter.h"
#include "../CHTL/Node/RootNode.h"
#include "../CHTL/Node/ElementNode.h"
#include "../CHTL/Node/AttributeNode.h"
#include "../CHTL/Node/TextNode.h"
#include "../CHTL/Node/StyleNode.h"
#include "../CHTL/Node/TemplateNode.h"
#include "../CHTL/Node/TemplateUsageNode.h"
#include "../CHTL/Node/CssPropertyNode.h"
#include "../CHTL/Node/VarDeclarationNode.h"
#include "../CHTL/Node/VarUsageNode.h"
#include "../CHTL/Node/LiteralNode.h"
#include "../CHTL/Node/CustomNode.h"
#include "../CHTL/Node/CustomUsageNode.h"
#include "../CHTL/Node/DeleteNode.h"
#include <iostream>

namespace CHTL {

std::string AstPrinter::print(const std::shared_ptr<BaseNode>& node) {
    m_output = "";
    visit(node, 0);
    return m_output;
}

std::string AstPrinter::valueNodeToString(const std::shared_ptr<BaseNode>& node) {
    if (!node) return "nullptr";
    if (auto literal = std::dynamic_pointer_cast<LiteralNode>(node)) {
        return "\"" + literal->value + "\"";
    }
    if (auto varUsage = std::dynamic_pointer_cast<VarUsageNode>(node)) {
        return varUsage->groupName + "(" + varUsage->varName + ")";
    }
    return "UNKNOWN_VALUE";
}

void AstPrinter::visit(const std::shared_ptr<BaseNode>& node, int indentLevel) {
    std::string indent(indentLevel * 2, ' ');

    if (!node) {
        m_output += indent + "nullptr\n";
        return;
    }

    if (auto rootNode = std::dynamic_pointer_cast<RootNode>(node)) {
        m_output += indent + "RootNode\n";
        for (const auto& child : rootNode->children) {
            visit(child, indentLevel + 1);
        }
    } else if (auto elementNode = std::dynamic_pointer_cast<ElementNode>(node)) {
        m_output += indent + "ElementNode <" + elementNode->tagName + ">\n";
        for (const auto& attr : elementNode->attributes) {
             m_output += indent + "  - Attr: " + attr->key + " = " + valueNodeToString(attr->value) + "\n";
        }
        for (const auto& child : elementNode->children) {
            visit(child, indentLevel + 1);
        }
    } else if (auto textNode = std::dynamic_pointer_cast<TextNode>(node)) {
        m_output += indent + "TextNode: \"" + textNode->content + "\"\n";
    } else if (auto styleNode = std::dynamic_pointer_cast<StyleNode>(node)) {
        m_output += indent + "StyleNode\n";
        for (const auto& child : styleNode->children) {
            visit(child, indentLevel + 1);
        }
    } else if (auto propNode = std::dynamic_pointer_cast<CssPropertyNode>(node)) {
        m_output += indent + "CssPropertyNode: " + propNode->key + ": " + valueNodeToString(propNode->value) + "\n";
    } else if (auto templateUsageNode = std::dynamic_pointer_cast<TemplateUsageNode>(node)) {
        m_output += indent + "TemplateUsageNode (Name: " + templateUsageNode->name + ")\n";
    } else if (auto varUsageNode = std::dynamic_pointer_cast<VarUsageNode>(node)) {
        m_output += indent + "VarUsageNode: " + varUsageNode->groupName + "(" + varUsageNode->varName + ")\n";
    } else if (auto literalNode = std::dynamic_pointer_cast<LiteralNode>(node)) {
        m_output += indent + "LiteralNode: \"" + literalNode->value + "\"\n";
    } else if (auto varDeclNode = std::dynamic_pointer_cast<VarDeclarationNode>(node)) {
        m_output += indent + "VarDeclarationNode: " + varDeclNode->name + " = \"" + varDeclNode->value + "\"\n";
    } else if (auto deleteNode = std::dynamic_pointer_cast<DeleteNode>(node)) {
        m_output += indent + "DeleteNode (Targets: ";
        for(size_t i = 0; i < deleteNode->targets.size(); ++i) {
            m_output += deleteNode->targets[i] + (i == deleteNode->targets.size() - 1 ? "" : ", ");
        }
        m_output += ")\n";
    } else if (auto templateNode = std::dynamic_pointer_cast<TemplateNode>(node)) {
        m_output += indent + "TemplateNode (Name: " + templateNode->name + ")\n";
        for (const auto& child : templateNode->body) {
            visit(child, indentLevel + 1);
        }
    } else if (auto customNode = std::dynamic_pointer_cast<CustomNode>(node)) {
        m_output += indent + "CustomNode (Name: " + customNode->name + ")\n";
        for (const auto& child : customNode->body) {
            visit(child, indentLevel + 1);
        }
    } else if (auto customUsageNode = std::dynamic_pointer_cast<CustomUsageNode>(node)) {
        m_output += indent + "CustomUsageNode (Name: " + customUsageNode->name + ")\n";
        for (const auto& child : customUsageNode->specializationBody) {
            visit(child, indentLevel + 1);
        }
    } else {
        m_output += indent + "UnknownNode\n";
    }
}

} // namespace CHTL
