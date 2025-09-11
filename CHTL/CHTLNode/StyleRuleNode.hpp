#pragma once

#include "BaseNode.hpp"
#include "StyleNode.hpp" // For StyleRule

namespace CHTL {

class StyleRuleNode : public BaseNode {
public:
    void accept(AstVisitor& visitor) override;

    StyleRule rule;
};

} // namespace CHTL
