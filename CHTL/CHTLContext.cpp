#include "CHTLContext.hpp"

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
    } else if (auto elem = std::dynamic_pointer_cast<ElementNode>(node)) {
        // Recurse on children to find nested definitions
        for (const auto& child : elem->children) {
            collectNode(child);
        }
    }
    // Note: This simple collector doesn't look inside namespaces yet.
    // That will be the next step.
}

} // namespace CHTL
