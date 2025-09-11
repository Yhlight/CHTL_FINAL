#pragma once

#include "BaseNode.h"
#include "StyleNode.h"
#include <string>
#include <memory>

namespace CHTL {

// Represents an instruction to modify an element within a [Custom] @Element usage.
// e.g., `div[1] { style { ... } }`
class ElementModificationNode : public BaseNode {
public:
    ElementModificationNode(std::string selector, std::shared_ptr<StyleNode> style)
        : m_selector(std::move(selector)), m_style(std::move(style)) {}

    NodeType GetType() const override { return NodeType::ElementModification; } // Will need to add this to BaseNode.h

    const std::string& GetSelector() const { return m_selector; }
    const std::shared_ptr<StyleNode>& GetStyle() const { return m_style; }

    NodePtr Clone() const override {
        return std::make_shared<ElementModificationNode>(m_selector, std::static_pointer_cast<StyleNode>(m_style->Clone()));
    }

private:
    std::string m_selector;
    std::shared_ptr<StyleNode> m_style;
};

using ElementModificationNodePtr = std::shared_ptr<ElementModificationNode>;

} // namespace CHTL
