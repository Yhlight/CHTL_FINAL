#include "CHTLContext.hpp"
#include "CHTLNode/CustomDefinitionNode.hpp"

namespace CHTL {

void CHTLContext::collect(const std::vector<NodePtr>& nodes) {
    for (const auto& node : nodes) {
        collectNode(node);
    }
}

void CHTLContext::collectNode(const NodePtr& node) {
    if (auto def = std::dynamic_pointer_cast<TemplateDefinitionNode>(node)) {
        if (def->templateType == TemplateType::ELEMENT) {
            elementTemplates[def->name] = def;
        } else if (def->templateType == TemplateType::STYLE) {
            styleTemplates[def->name] = def;
        }
    } else if (auto custom_def = std::dynamic_pointer_cast<CustomDefinitionNode>(node)) {
        // For now, treat custom definitions just like templates for collection
        if (custom_def->customType == TemplateType::ELEMENT) {
            // This is not quite right, we need a separate map for custom defs
            // but for now this will make it findable.
            // A better solution would be a single map that holds a base class.
            // Let's create a temporary TemplateDefinitionNode from the Custom one.
            auto temp_template_def = std::make_shared<TemplateDefinitionNode>();
            temp_template_def->name = custom_def->name;
            temp_template_def->templateType = custom_def->customType;
            temp_template_def->bodyNodes = custom_def->bodyNodes;
            temp_template_def->styleRules = custom_def->styleRules;
            elementTemplates[custom_def->name] = temp_template_def;
        }
    }
    else if (auto elem = std::dynamic_pointer_cast<ElementNode>(node)) {
        // Recurse on children to find nested definitions
        for (const auto& child : elem->children) {
            collectNode(child);
        }
    }
    // Note: This simple collector doesn't look inside namespaces yet.
    // That will be the next step.
}

} // namespace CHTL
