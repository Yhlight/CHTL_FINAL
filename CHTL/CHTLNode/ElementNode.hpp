#pragma once

#include "BaseNode.hpp"
#include "AttributeNode.hpp"
#include <string>
#include <vector>
#include <memory>

namespace CHTL {

class ElementNode : public BaseNode {
public:
    void accept(AstVisitor& visitor) override;

    std::string tagName;
    std::vector<std::shared_ptr<AttributeNode>> attributes;
    std::vector<NodePtr> children;
};

} // namespace CHTL
