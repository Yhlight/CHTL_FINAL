#pragma once

#include "BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {

class ScriptNode : public BaseNode {
public:
    explicit ScriptNode(std::string content) : m_content(std::move(content)) {}

    NodeType getType() const override { return NodeType::Script; }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<ScriptNode>(m_content);
    }

    const std::string& getContent() const { return m_content; }

private:
    std::string m_content;
};

} // namespace CHTL
