#pragma once

#include "BaseNode.hpp"
#include "../CHTLLexer/Token.hpp"
#include <string>

namespace CHTL {

class TemplateUsageNode : public BaseNode {
public:
    void accept(AstVisitor& visitor) override;

    Token type; // @Style, @Element, etc.
    std::string name;
};

} // namespace CHTL
