#pragma once

#include "ExpressionNode.h"
#include <string>

namespace CHTL {

class TemplateUsageNode : public ExpressionNode {
public:
    explicit TemplateUsageNode(std::string templateName)
        : m_templateName(std::move(templateName)) {}

    ExpressionNodeType GetType() const override { return ExpressionNodeType::TemplateUsage; }

    const std::string& GetTemplateName() const { return m_templateName; }

private:
    std::string m_templateName;
};

} // namespace CHTL
