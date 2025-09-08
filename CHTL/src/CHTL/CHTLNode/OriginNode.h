#pragma once

#include "BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {

class OriginNode : public BaseNode {
public:
    OriginNode(std::string name, std::string content)
        : m_name(std::move(name)), m_content(std::move(content)) {}

    NodeType getType() const override { return NodeType::Origin; }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<OriginNode>(m_name, m_content);
    }

    const std::string& getName() const { return m_name; }
    const std::string& getContent() const { return m_content; }

private:
    std::string m_name; // Optional, for named blocks
    std::string m_content;
};

} // namespace CHTL
