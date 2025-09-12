#include "CHTLGenerator.h"
#include "CHTL/CHTLNode/ElementNode.h"
#include "CHTL/CHTLNode/TextNode.h"
#include "CHTL/CHTLNode/CommentNode.h"
#include "CHTL/CHTLNode/TemplateNode.h"
#include "CHTL/CHTLNode/CustomNode.h"
#include "CHTL/CHTLNode/StyleNode.h"
#include "CHTL/CHTLNode/ScriptNode.h"
#include "CHTL/CHTLNode/OriginNode.h"
#include "CHTL/CHTLNode/ImportNode.h"
#include "CHTL/CHTLNode/ConfigNode.h"
#include "CHTL/CHTLNode/NamespaceNode.h"
#include "CHTL/CHTLNode/OperatorNode.h"

namespace CHTL {

std::string CHTLGenerator::generate(BaseNode* root) {
    if (root) {
        root->accept(*this);
    }
    return output.str();
}

void CHTLGenerator::visit(ElementNode& node) {
    output << "<" << node.getTagName();

    // Append attributes
    for (const auto& attr : node.getAttributes()) {
        output << " " << attr.first << "=\"" << attr.second << "\"";
    }

    // Append inline styles
    if (!node.getInlineStyles().empty()) {
        output << " style=\"";
        for (const auto& style : node.getInlineStyles()) {
            output << style.first << ":" << style.second << ";";
        }
        output << "\"";
    }

    output << ">";

    // Recursively visit children
    for (const auto& child : node.getChildren()) {
        child->accept(*this);
    }

    output << "</" << node.getTagName() << ">";
}

void CHTLGenerator::visit(TextNode& node) {
    // In a real implementation, we would escape HTML entities here.
    output << node.getText();
}

// Add empty implementations for other node types to make the class concrete
void CHTLGenerator::visit(CommentNode& node) {
    output << "<!--" << node.getComment() << "-->";
}
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
