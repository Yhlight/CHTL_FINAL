#pragma once

#include "ExpressionNode.h"
#include "StyleNode.h" // For specialization block
#include <string>
#include <memory>

namespace CHTL {

class TemplateUsageNode : public ExpressionNode {
public:
    explicit TemplateUsageNode(std::string templateName, std::shared_ptr<StyleNode> specialization = nullptr, std::string from = "")
        : m_templateName(std::move(templateName)), m_specialization(std::move(specialization)), m_from(std::move(from)) {}

    ExpressionNodeType GetType() const override { return ExpressionNodeType::TemplateUsage; }

    const std::string& GetTemplateName() const { return m_templateName; }
    const std::string& GetFrom() const { return m_from; }

    // The specialization block, e.g., the { ... } after @Style MyCustom { ... }
    const std::shared_ptr<StyleNode>& GetSpecialization() const { return m_specialization; }
    bool IsSpecialized() const { return m_specialization != nullptr; }

    ExpressionNodePtr Clone() const override {
        return std::make_shared<TemplateUsageNode>(m_templateName, m_specialization ? std::static_pointer_cast<StyleNode>(m_specialization->Clone()) : nullptr, m_from);
    }

private:
    std::string m_templateName;
    std::string m_from;
    std::shared_ptr<StyleNode> m_specialization;
};

} // namespace CHTL
