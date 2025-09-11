#pragma once

#include "BaseNode.hpp"
#include "../CHTLLexer/Token.hpp"
#include <string>

namespace CHTL {

class ImportNode : public BaseNode {
public:
    void accept(AstVisitor& visitor) override;

    // For now, just store the path as a token
    Token path;
};

} // namespace CHTL
