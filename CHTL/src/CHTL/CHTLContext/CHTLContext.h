#pragma once

#include "../CHTLNode/TemplateDefinitionNode.h"
#include <string>
#include <map>
#include <memory>

namespace CHTL {

// The CHTLContext class holds state that is shared across the entire
// compilation pipeline, such as the registry of defined templates.
class CHTLContext {
public:
    CHTLContext() = default;

    // Adds a parsed template definition to the registry.
    void registerTemplate(std::unique_ptr<TemplateDefinitionNode> templateNode);

    // Retrieves a template definition by name.
    const TemplateDefinitionNode* getTemplate(const std::string& name) const;

private:
    // The registry mapping template names to their AST definition nodes.
    std::map<std::string, std::unique_ptr<TemplateDefinitionNode>> m_templates;
};

} // namespace CHTL
