#include "SemanticAnalyzer.h"
#include "CHTLNode/NamespaceNode.h"
#include "CHTLNode/TemplateNode.h"

SemanticAnalyzer::SemanticAnalyzer(NodeList& ast) : ast(ast) {}

void SemanticAnalyzer::analyze() {
    for (const auto& node : ast) {
        node->accept(*this);
    }
}

void SemanticAnalyzer::visit(NamespaceNode* node) {
    // Recurse into namespace
    for (const auto& child : node->children) {
        child->accept(*this);
    }
}

void SemanticAnalyzer::visit(ElementNode* node) {
    // First, find all constraints for this element
    std::set<std::string> forbidden_tags;
    std::set<std::string> forbidden_template_types;

    for (const auto& child : node->children) {
        if (auto* constraint = dynamic_cast<ConstraintNode*>(child.get())) {
            for (const auto& target : constraint->constrained_targets) {
                // This is a simplified check. CHTL.md specifies more complex
                // constraints like `[Template] @Var` or `[Custom]`.
                // For now, we'll just check for simple tag names.
                forbidden_tags.insert(target.value);
            }
        }
    }

    // If there are rules, check all other children against them
    if (!forbidden_tags.empty()) {
        for (const auto& child : node->children) {
            if (auto* element = dynamic_cast<ElementNode*>(child.get())) {
                if (forbidden_tags.count(element->tagName)) {
                    throw SemanticError("Constraint violation: Element <" + node->tagName + "> cannot contain <" + element->tagName + ">.");
                }
            }
            // TODO: Add checks for other node types like templates.
        }
    }

    // Recurse into child elements
    for (const auto& child : node->children) {
        // We only need to visit nodes that can contain other nodes.
        if (dynamic_cast<ElementNode*>(child.get()) || dynamic_cast<NamespaceNode*>(child.get())) {
            child->accept(*this);
        }
    }
}
