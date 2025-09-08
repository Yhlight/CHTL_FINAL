#include "OriginNode.h"
#include "../CHTLGenerator/Generator.h"

void OriginNode::accept(AstVisitor& visitor) {
    visitor.visit(*this);
}
