#pragma once

#include "BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {

class DeleteRuleNode : public BaseNode {
public:
    explicit DeleteRuleNode(std::string target)
        : m_target(std::move(target)) {}

    NodeType getType() const override { return NodeType::DeleteRule; }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<DeleteRuleNode>(m_target);
    }

    const std::string& getTarget() const { return m_target; }

private:
    std::string m_target;
};

} // namespace CHTL
