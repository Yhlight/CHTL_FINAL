#include "ImportVisitor.h"
#include "CHTLNode/ElementNode.h"
#include "CHTLNode/StyleNode.h"

void ImportVisitor::run(const NodeList& ast) {
    for (const auto& node : ast) {
        node->accept(*this);
    }
}

void ImportVisitor::visit(ImportNode* node) {
    import_nodes.push_back(node);
}

void ImportVisitor::visit(ElementNode* node) {
    // Recursively visit children
    for (const auto& child : node->children) {
        child->accept(*this);
    }
}

void ImportVisitor::visit(StyleNode* node) {
    // Recursively visit children of a style block
    for (const auto& child : node->children) {
        child->accept(*this);
    }
}
