#pragma once

#include "ExpressionNode.h"
#include <string>

namespace CHTL {

class VariableUsageNode : public ExpressionNode {
public:
    VariableUsageNode(std::string groupName, std::string varName)
        : m_groupName(std::move(groupName)), m_variableName(std::move(varName)) {}

    ExpressionNodeType GetType() const override { return ExpressionNodeType::VariableUsage; }

    const std::string& GetGroupName() const { return m_groupName; }
    const std::string& GetVariableName() const { return m_variableName; }

private:
    std::string m_groupName;
    std::string m_variableName;
};

} // namespace CHTL
