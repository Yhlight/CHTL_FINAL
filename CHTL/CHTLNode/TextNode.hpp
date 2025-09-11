#pragma once

#include "BaseNode.hpp"
#include "../CHTLLexer/Token.hpp"
#include <vector>

namespace CHTL {

class TextNode : public BaseNode {
public:
    void accept(AstVisitor& visitor) override;

    std::vector<Token> textTokens;
};

} // namespace CHTL
