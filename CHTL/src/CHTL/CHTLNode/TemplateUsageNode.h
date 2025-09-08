#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

// Represents the usage of a template, e.g., `@Style DefaultText;`
// This node acts as a placeholder that the generator will replace
// with the content of the referenced template.
class TemplateUsageNode : public BaseNode {
public:
    TemplateUsageNode(std::string type, std::string name)
        : m_templateType(std::move(type)), m_templateName(std::move(name)) {}

    NodeType getType() const override { return NodeType::TemplateUsage; }

    const std::string& getTemplateType() const { return m_templateType; }
    const std::string& getTemplateName() const { return m_templateName; }

private:
    std::string m_templateType;
    std::string m_templateName;
};

} // namespace CHTL
