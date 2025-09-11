#pragma once

#include "BaseNode.hpp"
#include <string>
#include <vector>

namespace CHTL {

class NamespaceNode : public BaseNode {
public:
    void accept(AstVisitor& visitor) override;

    std::string name;
    std::vector<NodePtr> body;
};

} // namespace CHTL
