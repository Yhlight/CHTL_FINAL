#include "CustomStyleTemplateNode.h"
#include "../CHTLGenerator/Generator.h"

void CustomStyleTemplateNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}
