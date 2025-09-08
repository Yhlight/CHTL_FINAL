#pragma once

#include "BaseNode.h"
#include <string>
#include <memory>

namespace CHTL {

class AttributeNode : public BaseNode {
public:
    AttributeNode(const std::string& key, const std::string& value)
        : m_key(key), m_value(value) {}

    NodeType getType() const override { return NodeType::Attribute; }

    std::unique_ptr<BaseNode> clone() const override {
        return std::make_unique<AttributeNode>(m_key, m_value);
    }

    const std::string& getKey() const { return m_key; }
    const std::string& getValue() const { return m_value; }

private:
    std::string m_key;
    std::string m_value;
};

} // namespace CHTL
