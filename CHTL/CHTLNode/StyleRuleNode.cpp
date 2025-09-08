#include "StyleRuleNode.h"
#include "../CHTLGenerator/Generator.h"

void StyleRuleNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}
