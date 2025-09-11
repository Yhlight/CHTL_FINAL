#pragma once

#include "BaseNode.hpp"
#include "../CHTLLexer/Token.hpp"
#include <vector>
#include <map>

namespace CHTL {

// For now, a "rule" is just a list of tokens.
// This is flexible enough to represent `width: 100px;` and later `.box { ... }`
using StyleRule = std::vector<Token>;

class StyleNode : public BaseNode {
public:
    void accept(AstVisitor& visitor) override;

    std::vector<NodePtr> children;
};

} // namespace CHTL
