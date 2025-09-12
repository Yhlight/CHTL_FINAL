#include "ExprNode.h"
#include <sstream>

namespace CHTL
{
    std::string NumberLiteralNode::toString() const {
        std::stringstream ss;
        ss << value << unit;
        return ss.str();
    }

    std::string BinaryOpNode::toString() const {
        std::stringstream ss;
        ss << "(" << op << " " << left->toString() << " " << right->toString() << ")";
        return ss.str();
    }

    std::string StringLiteralNode::toString() const {
        // Add quotes for visualization to distinguish from numbers
        return "\"" + value + "\"";
    }

    std::string AttributeReferenceNode::toString() const {
        return "(ref " + selector + "." + propertyName + ")";
    }
}
