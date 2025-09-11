#pragma once

#include "BaseNode.hpp"
#include "../CHTLLexer/Token.hpp"
#include <string>
#include <vector>

namespace CHTL {

class AttributeNode : public BaseNode {
public:
    void accept(AstVisitor& visitor) override;

    std::string name;
    // The value can be constructed from one or more tokens (e.g. for unquoted literals)
    std::vector<Token> valueTokens;
};

} // namespace CHTL
