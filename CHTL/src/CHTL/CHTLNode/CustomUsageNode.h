#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class CustomUsageNode : public BaseNode {
public:
    CustomUsageNode(std::string type, std::string name)
        : m_customType(std::move(type)), m_customName(std::move(name)) {}

    NodeType getType() const override { return NodeType::CustomUsage; }

    std::unique_ptr<BaseNode> clone() const override {
        auto newNode = std::make_unique<CustomUsageNode>(m_customType, m_customName);
        for (const auto& rule : m_specializationRules) {
            newNode->addSpecialization(rule->clone());
        }
        return newNode;
    }

    const std::string& getCustomType() const { return m_customType; }
    const std::string& getCustomName() const { return m_customName; }

    void addSpecialization(std::unique_ptr<BaseNode> rule) {
        m_specializationRules.push_back(std::move(rule));
    }

    const std::vector<std::unique_ptr<BaseNode>>& getSpecializations() const {
        return m_specializationRules;
    }

private:
    std::string m_customType;
    std::string m_customName;
    std::vector<std::unique_ptr<BaseNode>> m_specializationRules;
};

} // namespace CHTL
