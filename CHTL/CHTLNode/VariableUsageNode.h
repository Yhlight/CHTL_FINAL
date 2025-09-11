#pragma once

#include "ExpressionNode.h"
#include <string>
#include <map>
#include <memory>

namespace CHTL {

class VariableUsageNode : public ExpressionNode {
public:
    // For simple usage: VarGroup(varName)
    explicit VariableUsageNode(std::string groupName, std::string varName, std::string from = "")
        : m_groupName(std::move(groupName)), m_variableName(std::move(varName)), m_from(std::move(from)) {}

    // For specialized usage: VarGroup(varName = "value", ...)
    VariableUsageNode(std::string groupName, std::map<std::string, ExpressionNodePtr> specializations, std::string from = "")
        : m_groupName(std::move(groupName)), m_specializations(std::move(specializations)), m_from(std::move(from)) {}


    ExpressionNodeType GetType() const override { return ExpressionNodeType::VariableUsage; }

    const std::string& GetGroupName() const { return m_groupName; }
    const std::string& GetFrom() const { return m_from; }

    // This will be empty if specializations are used
    const std::string& GetVariableName() const { return m_variableName; }

    const std::map<std::string, ExpressionNodePtr>& GetSpecializations() const { return m_specializations; }
    bool IsSpecialized() const { return !m_specializations.empty(); }

    ExpressionNodePtr Clone() const override {
        if (IsSpecialized()) {
            std::map<std::string, ExpressionNodePtr> clonedSpecs;
            for(const auto& pair : m_specializations) {
                clonedSpecs[pair.first] = pair.second->Clone();
            }
            return std::make_shared<VariableUsageNode>(m_groupName, clonedSpecs, m_from);
        } else {
            return std::make_shared<VariableUsageNode>(m_groupName, m_variableName, m_from);
        }
    }

private:
    std::string m_groupName;
    std::string m_variableName; // Used for simple lookups
    std::string m_from;
    std::map<std::string, ExpressionNodePtr> m_specializations; // Used for specialized lookups
};

} // namespace CHTL
