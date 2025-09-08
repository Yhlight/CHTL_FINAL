#pragma once

#include "BaseNode.h"
#include "VariableDefinitionNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class VariableGroupDefinitionNode : public BaseNode {
public:
    explicit VariableGroupDefinitionNode(std::string name) : m_name(std::move(name)) {}

    NodeType getType() const override { return NodeType::VariableGroupDefinition; }

    std::unique_ptr<BaseNode> clone() const override {
        auto newNode = std::make_unique<VariableGroupDefinitionNode>(m_name);
        for (const auto& child : m_variables) {
            newNode->addVariable(std::unique_ptr<VariableDefinitionNode>(static_cast<VariableDefinitionNode*>(child->clone().release())));
        }
        return newNode;
    }

    const std::string& getName() const { return m_name; }
    const std::vector<std::unique_ptr<BaseNode>>& getVariables() const { return m_variables; }

    void addVariable(std::unique_ptr<VariableDefinitionNode> var) {
        m_variables.push_back(std::move(var));
    }

private:
    std::string m_name;
    std::vector<std::unique_ptr<BaseNode>> m_variables;
};

} // namespace CHTL
