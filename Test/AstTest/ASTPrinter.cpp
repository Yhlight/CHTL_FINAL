#include "ASTPrinter.h"
#include "../../CHTL/CHTLNode/ElementNode.h"
#include "../../CHTL/CHTLNode/TextNode.h"
#include "../../CHTL/CHTLNode/StyleNode.h"
#include "../../CHTL/CHTLNode/OriginNode.h"
#include "../../CHTL/CHTLNode/TemplateDeclarationNode.h"
#include "../../CHTL/CHTLNode/CustomDeclarationNode.h"
#include "../../CHTL/CHTLNode/ImportNode.h"
#include "../../CHTL/CHTLNode/ScriptNode.h"
#include "../../CHTL/CHTLNode/NamespaceNode.h"
#include "../../CHTL/CHTLNode/CssRuleNode.h" // Include new node type
#include <iostream>

namespace CHTL {

void ASTPrinter::print(BaseNode* node) {
    if (node) {
        node->accept(*this);
    }
}

void ASTPrinter::visit(ElementNode& node) {
    std::cout << indentString() << "<" << node.tagName << ">" << std::endl;

    indent++;
    // Print attributes
    for (const auto& attr : node.attributes) {
        // TODO: Implement a proper expression printer
        std::cout << indentString() << "@" << attr.key << " = [expression]" << std::endl;
    }

    // Print children
    for (const auto& child : node.children) {
        child->accept(*this);
    }
    indent--;
}

void ASTPrinter::visit(TextNode& node) {
    std::cout << indentString() << "Text: \"" << node.text << "\"" << std::endl;
}

std::string ASTPrinter::indentString() {
    return std::string(indent * 2, ' ');
}

void ASTPrinter::visit(StyleNode& node) {
    std::cout << indentString() << "<style>" << std::endl;
    indent++;
    // Print inline properties
    if (!node.inline_properties.empty()) {
        std::cout << indentString() << "[Inline Props]" << std::endl;
        indent++;
        for (const auto& prop : node.inline_properties) {
            std::cout << indentString() << prop.key << ": [expression];" << std::endl;
        }
        indent--;
    }
    // Print global rules
    if (!node.global_rules.empty()) {
        std::cout << indentString() << "[Global Rules]" << std::endl;
        indent++;
        for (const auto& rule : node.global_rules) {
            std::cout << indentString() << rule.selector << " {" << std::endl;
            indent++;
            for (const auto& prop : rule.properties) {
                std::cout << indentString() << prop.key << ": [expression];" << std::endl;
            }
            indent--;
            std::cout << indentString() << "}" << std::endl;
        }
        indent--;
    }
    indent--;
}

void ASTPrinter::visit(OriginNode& node) {
    std::string type_str = "Html";
    if (node.type == OriginType::STYLE) type_str = "Style";
    else if (node.type == OriginType::JAVASCRIPT) type_str = "JavaScript";

    std::cout << indentString() << "[Origin @" << type_str << "]" << std::endl;
    indent++;
    std::cout << indentString() << "Content: \"" << node.content.substr(0, 40) << "...\"" << std::endl;
    indent--;
}

void ASTPrinter::visit(TemplateDeclarationNode& node) {
    std::cout << indentString() << "[Template] " << node.definition->name << std::endl;
}

void ASTPrinter::visit(CustomDeclarationNode& node) {
    std::cout << indentString() << "[Custom] " << node.definition->name << std::endl;
}

void ASTPrinter::visit(ImportNode& node) {
    std::cout << indentString() << "[Import] " << node.path << std::endl;
}

void ASTPrinter::visit(ScriptNode& node) {
    std::cout << indentString() << "<script>" << std::endl;
}

void ASTPrinter::visit(NamespaceNode& node) {
    std::cout << indentString() << "[Namespace] " << node.name << " {" << std::endl;
    indent++;
    for (const auto& child : node.children) {
        child->accept(*this);
    }
    indent--;
    std::cout << indentString() << "}" << std::endl;
}

} // namespace CHTL
