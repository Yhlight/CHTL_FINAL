#include "CHTLGenerator.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/CommentNode.h"
// ... include all other node headers ...

namespace CHTL {

std::string CHTLGenerator::generate(BaseNode* root, CHTLContext& context) {
    this->context = &context;
    if (root) {
        root->accept(*this);
    }
    return output.str();
}

void CHTLGenerator::visit(ElementNode& node) {
    output << "<" << node.getTagName();

    for (const auto& attr : node.getAttributes()) {
        output << " " << attr.first << "=\"" << attr.second << "\"";
    }

    if (!node.getInlineStyles().empty()) {
        output << " style=\"";
        for (const auto& style : node.getInlineStyles()) {
            output << style.first << ":" << style.second << ";";
        }
        output << "\"";
    }

    output << ">";

    // If this is the head tag, inject the collected CSS
    if (node.getTagName() == "head" && context && !context->getCssRules().empty()) {
        output << "<style>";
        for (const auto& rule : context->getCssRules()) {
            output << rule.selector << "{" << rule.body << "}";
        }
        output << "</style>";
    }

    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }

    output << "</" << node.getTagName() << ">";
}

void CHTLGenerator::visit(TextNode& node) {
    output << node.getText();
}

void CHTLGenerator::visit(CommentNode& node) {
    output << "<!--" << node.getComment() << "-->";
}

// Add empty implementations for other node types
void CHTLGenerator::visit(TemplateNode& node) {}
void CHTLGenerator::visit(CustomNode& node) {}
void CHTLGenerator::visit(StyleNode& node) {}
void CHTLGenerator::visit(ScriptNode& node) {}
void CHTLGenerator::visit(OriginNode& node) {}
void CHTLGenerator::visit(ImportNode& node) {}
void CHTLGenerator::visit(ConfigNode& node) {}
void CHTLGenerator::visit(NamespaceNode& node) {}
void CHTLGenerator::visit(OperatorNode& node) {}

} // namespace CHTL
