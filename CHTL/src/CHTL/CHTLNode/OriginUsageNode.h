#pragma once

#include "BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {

class OriginUsageNode : public BaseNode {
public:
    explicit OriginUsageNode(std::string name)
        : m_name(std::move(name)) {}

    NodeType getType() const override { return NodeType::OriginUsage; }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<OriginUsageNode>(m_name);
    }

    const std::string& getName() const { return m_name; }

private:
    std::string m_name;
};

} // namespace CHTL
