#include "AstPrinter.hpp"
#include "CHTLNode/ElementNode.hpp"
#include "CHTLNode/TextNode.hpp"
#include "CHTLNode/AttributeNode.hpp"
#include "CHTLNode/CommentNode.hpp"
#include "CHTLNode/StyleNode.hpp" // Include full definition
#include "CHTLNode/TemplateDefinitionNode.hpp"
#include "CHTLNode/TemplateUsageNode.hpp"
#include "CHTLNode/StyleRuleNode.hpp"
#include "CHTLNode/CustomDefinitionNode.hpp"
#include "CHTLNode/ImportNode.hpp"
#include "CHTLNode/NamespaceNode.hpp"

namespace CHTL {

std::string AstPrinter::print(const std::vector<NodePtr>& nodes) {
    m_result = "";
    for (const auto& node : nodes) {
        node->accept(*this);
        m_result += "\n";
    }
    return m_result;
}

void AstPrinter::visit(ElementNode& node) {
    m_result += "(";
    m_result += node.tagName;

    if (!node.attributes.empty()) {
        m_result += " ";
        parenthesize_attr("attributes", node.attributes);
    }

    if (!node.children.empty()) {
        m_result += " ";
        parenthesize("children", node.children);
    }

    m_result += ")";
}

void AstPrinter::visit(CustomDefinitionNode& node) {
    std::string type;
    switch(node.customType) {
        case TemplateType::STYLE: type = "Style"; break;
        case TemplateType::ELEMENT: type = "Element"; break;
        case TemplateType::VAR: type = "Var"; break;
    }
    m_result += "([Custom] @" + type + " " + node.name;

    if (!node.bodyNodes.empty()) {
        m_result += " ";
        parenthesize("body", node.bodyNodes);
    }
    m_result += ")";
}

void AstPrinter::visit(TextNode& node) {
    m_result += "(text \"";
    for (const auto& token : node.textTokens) {
        m_result += token.lexeme + " ";
    }
    // remove trailing space
    if (!node.textTokens.empty()) {
        m_result.pop_back();
    }
    m_result += "\")";
}

void AstPrinter::visit(AttributeNode& node) {
    m_result += "(attr " + node.name + " \"";
    for (const auto& token : node.valueTokens) {
        m_result += token.lexeme + " ";
    }
     if (!node.valueTokens.empty()) {
        m_result.pop_back();
    }
    m_result += "\")";
}

void AstPrinter::visit(CommentNode& node) {
    m_result += "(comment \"" + node.content + "\")";
}

void AstPrinter::visit(StyleNode& node) {
    m_result += "(style";
    for (const auto& child : node.children) {
        m_result += " ";
        child->accept(*this);
    }
    m_result += ")";
}

void AstPrinter::visit(TemplateDefinitionNode& node) {
    std::string type;
    switch(node.templateType) {
        case TemplateType::STYLE: type = "Style"; break;
        case TemplateType::ELEMENT: type = "Element"; break;
        case TemplateType::VAR: type = "Var"; break;
    }
    m_result += "([Template] @" + type + " " + node.name;

    if (!node.bodyNodes.empty()) {
        m_result += " ";
        parenthesize("body", node.bodyNodes);
    }
    // Could add printing for style rules too if needed
    m_result += ")";
}

void AstPrinter::visit(TemplateUsageNode& node) {
    m_result += "(" + node.type.lexeme + " " + node.name + ")";
}

void AstPrinter::visit(StyleRuleNode& node) {
    m_result += "(rule";
    for (const auto& token : node.rule) {
        m_result += " " + token.lexeme;
    }
    m_result += ")";
}

void AstPrinter::visit(ImportNode& node) {
    m_result += "([Import] from \"" + node.path.lexeme + "\")";
}

void AstPrinter::visit(NamespaceNode& node) {
    m_result += "([Namespace] " + node.name;
    m_result += " ";
    parenthesize("body", node.body);
    m_result += ")";
}

void AstPrinter::parenthesize(const std::string& name, const std::vector<NodePtr>& nodes) {
    m_result += "(" + name;
    for (const auto& node : nodes) {
        m_result += " ";
        node->accept(*this);
    }
    m_result += ")";
}

void AstPrinter::parenthesize_attr(const std::string& name, const std::vector<std::shared_ptr<AttributeNode>>& nodes) {
    m_result += "(" + name;
    for (const auto& node : nodes) {
        m_result += " ";
        node->accept(*this);
    }
    m_result += ")";
}

} // namespace CHTL
