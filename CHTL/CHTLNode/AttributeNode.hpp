#pragma once

#include "BaseNode.hpp"
#include "../CSSExt/Expression.hpp"
#include <string>

namespace CHTL {

class AttributeNode : public BaseNode {
public:
    void accept(AstVisitor& visitor) override;

    std::string name;
    // The value is now a parsed expression tree
    CSSExt::ExprPtr valueExpression;
};

} // namespace CHTL
