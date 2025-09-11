#pragma once

#include "ExpressionNode.h"
#include "StyleNode.h" // For specialization block
#include <string>
#include <memory>

namespace CHTL {

class TemplateUsageNode : public ExpressionNode {
public:
    explicit TemplateUsageNode(std::string templateName, std::shared_ptr<StyleNode> specialization = nullptr)
        : m_templateName(std::move(templateName)), m_specialization(std::move(specialization)) {}

    ExpressionNodeType GetType() const override { return ExpressionNodeType::TemplateUsage; }

    const std::string& GetTemplateName() const { return m_templateName; }

    // The specialization block, e.g., the { ... } after @Style MyCustom { ... }
    const std::shared_ptr<StyleNode>& GetSpecialization() const { return m_specialization; }
    bool IsSpecialized() const { return m_specialization != nullptr; }

    ExpressionNodePtr Clone() const override {
        return std::make_shared<TemplateUsageNode>(m_templateName, m_specialization ? std::static_pointer_cast<StyleNode>(m_specialization->Clone()) : nullptr);
    }

private:
    std::string m_templateName;
    std::shared_ptr<StyleNode> m_specialization;
};

} // namespace CHTL
