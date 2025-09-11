#pragma once

#include "BaseNode.h"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

// Represents an instruction to delete one or more elements within a [Custom] @Element usage.
// e.g., `delete span, div[1];`
class ElementDeleteNode : public BaseNode {
public:
    explicit ElementDeleteNode(std::vector<std::string> selectors)
        : m_selectors_to_delete(std::move(selectors)) {}

    NodeType GetType() const override { return NodeType::ElementDeletion; }

    const std::vector<std::string>& GetSelectors() const { return m_selectors_to_delete; }

    NodePtr Clone() const override {
        return std::make_shared<ElementDeleteNode>(m_selectors_to_delete);
    }

private:
    std::vector<std::string> m_selectors_to_delete;
};

using ElementDeleteNodePtr = std::shared_ptr<ElementDeleteNode>;

} // namespace CHTL
