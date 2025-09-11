#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>

namespace CHTL {

class ElementTemplateUsageNode : public BaseNode {
public:
    explicit ElementTemplateUsageNode(std::string name, NodeList instructions = {})
        : m_templateName(std::move(name)), m_specialization_instructions(std::move(instructions)) {}

    NodeType GetType() const override { return NodeType::ElementTemplateUsage; }

    const std::string& GetTemplateName() const { return m_templateName; }
    const NodeList& GetInstructions() const { return m_specialization_instructions; }
    bool IsSpecialized() const { return !m_specialization_instructions.empty(); }

    NodePtr Clone() const override {
        NodeList clonedInstructions;
        for (const auto& instruction : m_specialization_instructions) {
            clonedInstructions.push_back(instruction->Clone());
        }
        return std::make_shared<ElementTemplateUsageNode>(m_templateName, clonedInstructions);
    }

private:
    std::string m_templateName;
    NodeList m_specialization_instructions;
};

} // namespace CHTL
