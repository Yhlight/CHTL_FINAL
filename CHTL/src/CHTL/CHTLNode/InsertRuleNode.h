#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// Represents an `insert` specialization rule, e.g., `insert after div[0] { ... }`
class InsertRuleNode : public BaseNode {
public:
    InsertRuleNode(std::string mode, std::string target, std::vector<std::unique_ptr<BaseNode>> content)
        : m_mode(std::move(mode)), m_target(std::move(target)), m_content(std::move(content)) {}

    NodeType getType() const override { return NodeType::InsertRule; }

    std::unique_ptr<BaseNode> clone() const override {
        std::vector<std::unique_ptr<BaseNode>> clonedContent;
        for(const auto& node : m_content) {
            clonedContent.push_back(node->clone());
        }
        return std::make_unique<InsertRuleNode>(m_mode, m_target, std::move(clonedContent));
    }

    const std::string& getMode() const { return m_mode; }
    const std::string& getTarget() const { return m_target; }
    const std::vector<std::unique_ptr<BaseNode>>& getContent() const { return m_content; }

private:
    std::string m_mode;
    std::string m_target;
    std::vector<std::unique_ptr<BaseNode>> m_content;
};

} // namespace CHTL
