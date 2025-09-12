#include "TemplateProcessor.h"
#include "../CHTLNode/ElementNode.h"
#include "../CHTLNode/TextNode.h"
#include "../CHTLNode/CommentNode.h"
#include "../CHTLNode/StyleNode.h"
#include "../CHTLNode/StyleTemplateNode.h"
#include "../CHTLNode/ElementTemplateNode.h"
#include "../CHTLNode/ElementTemplateUsageNode.h"
#include "../CHTLContext.h"
#include <iostream>

void TemplateProcessor::process(std::shared_ptr<BaseNode> root, CHTLContext& context) {
    if (root) {
        this->context = &context;
        root->accept(*this);
    }
}

void TemplateProcessor::visit(ElementNode& node) {
    // We need to build a new list of children, because we might replace
    // one child (the usage node) with multiple new children.
    std::vector<std::shared_ptr<BaseNode>> newChildren;

    for (const auto& child : node.children) {
        if (auto usageNode = std::dynamic_pointer_cast<ElementTemplateUsageNode>(child)) {
            // This is a template usage, expand it.
            if (context->elementTemplates.count(usageNode->name)) {
                auto& templateToExpand = context->elementTemplates.at(usageNode->name);
                // For now, we assume no deep copy is needed. This might need revisiting.
                newChildren.insert(newChildren.end(), templateToExpand->children.begin(), templateToExpand->children.end());
            } else {
                std::cerr << "Error: Undefined element template '" << usageNode->name << "'." << std::endl;
            }
        } else {
            // Not a usage node, so just keep it.
            newChildren.push_back(child);
        }
    }

    // Replace the old children with the new, expanded list.
    node.children = newChildren;

    // Now, recursively visit the children to expand any nested templates.
    for (const auto& child : node.children) {
        child->accept(*this);
    }
}

// Other visit methods are empty as this processor only cares about expanding element templates.
void TemplateProcessor::visit(TextNode& node) {}
void TemplateProcessor::visit(CommentNode& node) {}
void TemplateProcessor::visit(StyleNode& node) {}
void TemplateProcessor::visit(StyleTemplateNode& node) {}
void TemplateProcessor::visit(ElementTemplateNode& node) {
    // This processor does not act on template definitions, only usages.
}
void TemplateProcessor::visit(ElementTemplateUsageNode& node) {
    // This should not be called directly if the logic in visit(ElementNode&) is correct.
}
