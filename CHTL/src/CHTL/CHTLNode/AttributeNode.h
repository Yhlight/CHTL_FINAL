#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

// Represents a key-value attribute of an element, e.g., `class="box"`.
class AttributeNode : public BaseNode {
public:
    AttributeNode(const std::string& key, const std::string& value)
        : m_key(key), m_value(value) {}

    NodeType getType() const override { return NodeType::Attribute; }

    const std::string& getKey() const { return m_key; }
    const std::string& getValue() const { return m_value; }

private:
    std::string m_key;
    std::string m_value;
};

} // namespace CHTL
