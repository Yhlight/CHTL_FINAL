#pragma once

#include "BaseNode.h"
#include "StylePropertyNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class StyleSelectorNode : public BaseNode {
public:
    explicit StyleSelectorNode(const std::string& selector) : m_selector(selector) {}

    NodeType getType() const override { return NodeType::StyleSelector; }

    std::unique_ptr<BaseNode> clone() const override {
        auto newNode = std::make_unique<StyleSelectorNode>(m_selector);
        for (const auto& prop : m_properties) {
            newNode->addProperty(std::unique_ptr<StylePropertyNode>(static_cast<StylePropertyNode*>(prop->clone().release())));
        }
        return newNode;
    }

    const std::string& getSelector() const { return m_selector; }

    void addProperty(std::unique_ptr<StylePropertyNode> prop) {
        m_properties.push_back(std::move(prop));
    }

    const std::vector<std::unique_ptr<StylePropertyNode>>& getProperties() const { return m_properties; }

private:
    std::string m_selector;
    std::vector<std::unique_ptr<StylePropertyNode>> m_properties;
};

} // namespace CHTL
