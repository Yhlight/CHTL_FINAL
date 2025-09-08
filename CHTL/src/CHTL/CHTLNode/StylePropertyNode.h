#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

// Represents a single CSS-like property within a style block, e.g., `color: red;`
class StylePropertyNode : public BaseNode {
public:
    StylePropertyNode(const std::string& key, const std::string& value)
        : m_key(key), m_value(value) {}

    NodeType getType() const override { return NodeType::StyleProperty; }

    const std::string& getKey() const { return m_key; }
    const std::string& getValue() const { return m_value; }

private:
    std::string m_key;
    std::string m_value;
};

} // namespace CHTL
