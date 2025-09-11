#pragma once

#include "BaseNode.h"
#include <string>

namespace CHTL {

enum class OriginType {
    Html,
    Style,
    JavaScript,
};

class OriginNode : public BaseNode {
public:
    OriginNode(OriginType originType, const std::string& name, const std::string& content)
        : m_originType(originType), m_name(name), m_content(content) {}

    NodeType GetType() const override { return NodeType::Origin; }
    NodePtr Clone() const override {
        return std::make_shared<OriginNode>(m_originType, m_name, m_content);
    }

    OriginType GetOriginType() const { return m_originType; }
    const std::string& GetName() const { return m_name; }
    const std::string& GetContent() const { return m_content; }

private:
    OriginType m_originType;
    std::string m_name;
    std::string m_content;
};

} // namespace CHTL
