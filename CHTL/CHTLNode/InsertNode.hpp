#pragma once

#include "BaseNode.hpp"
#include "../CHTLLexer/Token.hpp"
#include <string>
#include <vector>

namespace CHTL {

class InsertNode : public BaseNode {
public:
    void accept(AstVisitor& visitor) override;

    Token position; // after, before, replace, etc.
    // For now, a simple string selector. This will need its own parser later.
    std::string selector;
    std::vector<NodePtr> nodesToInsert;
};

} // namespace CHTL
