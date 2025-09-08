#pragma once

#include "../CHTLNode/TemplateDefinitionNode.h"
#include "../CHTLNode/CustomDefinitionNode.h"
#include <string>
#include <map>
#include <memory>

namespace CHTL {

// The CHTLContext class holds state that is shared across the entire
// compilation pipeline, such as the registry of defined templates and custom blocks.
class CHTLContext {
public:
    CHTLContext() = default;

    // Template registry
    void registerTemplate(std::unique_ptr<TemplateDefinitionNode> templateNode);
    const TemplateDefinitionNode* getTemplate(const std::string& name) const;

    // Custom registry
    void registerCustom(std::unique_ptr<CustomDefinitionNode> customNode);
    const CustomDefinitionNode* getCustom(const std::string& name) const;

private:
    std::map<std::string, std::unique_ptr<TemplateDefinitionNode>> m_templates;
    std::map<std::string, std::unique_ptr<CustomDefinitionNode>> m_customs;
};

} // namespace CHTL
