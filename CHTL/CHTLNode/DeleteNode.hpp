#pragma once

#include "BaseNode.hpp"
#include "../CHTLLexer/Token.hpp"
#include <vector>

namespace CHTL {

class DeleteNode : public BaseNode {
public:
    void accept(AstVisitor& visitor) override;

    // A list of things to delete, e.g., [span, color, @, Style, MyStyle]
    std::vector<Token> targets;
};

} // namespace CHTL
